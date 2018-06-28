//#include "bubble_sort.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/timer/timer.hpp>


template< class Cont_T >
struct stl_sort 
{
	using container_type = Cont_T;

    void sort( container_type& cont )
    {
        std::sort( std::begin( cont ), std::end( cont ) );
    }
};

template< class Cont_T >
struct bubble_sort 
{
	using container_type = Cont_T;
    using iterator = typename container_type::iterator;

    void sort( container_type& cont )
    {
        bool sorted;
        do
        {
            sorted = true;
            auto it = std::begin( cont );
            auto last = it;

            for( ++it; it != std::end( cont ); ++it )
            {
                if( *last > *it )
                {
                    std::swap( *last, *it );
                    sorted = false;
                }
            }

        } while( !sorted );
    }
};

template< class Cont_T, template< typename...> class Sort_T >
size_t bench( const std::string& name, size_t size, size_t loop )
{
	using container_type = Cont_T;
    using sort_type = Sort_T< container_type >;

    boost::timer::cpu_timer timer;
    container_type org;

    srand( 1 );
    std::generate_n( std::back_inserter(org), size, &rand );

    sort_type sort;

    timer.start();
    for( size_t j = 0; j < loop; ++j )
    {
        container_type temp( org );
        sort.sort( temp );

        auto it = std::begin( temp );
        auto last = *it;
        for( ++it; it < std::end( temp ); ++it )
        {
            if( *it < last )
            {
                std::cout << last << " gt " << *it;
                break;
            }
        }
    }
    timer.stop();
    std::cout << "Sort all " << name << ": " << timer.format();

    return timer.elapsed().wall;
}

int main(int /*argc*/, char* /*argv*/[])
{
    std::cout << "\nsize: 0x00ff'ffff\n\n";
    while( 1 )
    {
        /*size_t base =*/ bench< std::vector< uint32_t >, stl_sort>( "STL sort ...", 0x00ffffff, 1 );

//        std::cout << std::endl << "Index Nocahe Diff: " << std::fixed << std::setprecision(2)
//                  << 100.0f * (((float) nocache)/((float) base) - 1.0f) << "%"
//                  << std::endl << "Index Cache Diff: " << std::fixed << std::setprecision(2)
//                  << 100.0f * (((float) cache)/((float) base) - 1.0f) << "%"
//                  << std::endl << "Index Cache / Index Nocache Diff: " << std::fixed << std::setprecision(2)
//                  << 100.0f * (((float) cache)/((float) nocache) - 1.0f) << "%"
//                  << std::endl << std::endl;
    }
    return 0;
}
