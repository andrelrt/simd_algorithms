#include "bubble_sort.h"

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
struct basic_bubble_sort
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
                last = it;
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

    srand(1);
    std::generate_n( std::back_inserter(org), size, &rand );
    //size_t cnt = size;
    //std::generate_n( std::back_inserter(org), size, [&cnt](){ return --cnt; } );

    sort_type sort;
    container_type warmup( org );
    sort.sort( warmup );

    timer.start();
    for( size_t j = 0; j < loop; ++j )
    {
        container_type temp( org );
        sort.sort( temp );

        auto it = std::begin( temp );
        auto last = it;
        for( ++it; it < std::end( temp ); ++it )
        {
            if( *it < *last )
            {
                std::cout << *last << " gt " << *it << " - ";
            }
            last = it;
        }
    }
    timer.stop();
    std::cout << "Sort all " << name << ": " << timer.format();

    return timer.elapsed().wall;
}

int main(int /*argc*/, char* /*argv*/[])
{
    std::cout << "\nsize: 0x0001'0000\n\n";
    while( 1 )
    {
        size_t stlsort = bench< std::vector< int32_t >, stl_sort>( "STL sort .........", 0x00010000, 1 );
        size_t bsort = bench< std::vector< int32_t >, basic_bubble_sort>( "Bubble sort ......", 0x00010000, 1 );
        size_t simdsort = bench< std::vector< int32_t >, simd_algorithms::sort::bubble>( "SIMD Bubble sort .", 0x00010000, 1 );
        size_t simdsort2 = bench< std::vector< int32_t >, simd_algorithms::sort::bubble2>( "SIMD Bubble2 sort ", 0x00010000, 1 );


        std::cout << std::endl << "SIMD Speed up ......: " << std::fixed << std::setprecision(2)
                  << static_cast<float>(bsort)/static_cast<float>(simdsort) << "x"
                  << std::endl << "SIMD2 Speed up .....: " << std::fixed << std::setprecision(2)
                  << static_cast<float>(bsort)/static_cast<float>(simdsort2) << "x"
                  << std::endl << "SIMD2/SIMD Speed up : " << std::fixed << std::setprecision(2)
                  << static_cast<float>(simdsort)/static_cast<float>(simdsort2) << "x"
                  << std::endl << "STL Speed up .......: " << std::fixed << std::setprecision(2)
                  << static_cast<float>(bsort)/static_cast<float>(stlsort) << "x"
                  << std::endl << "STL/SIMD2 Speed up .: " << std::fixed << std::setprecision(2)
                  << static_cast<float>(simdsort2)/static_cast<float>(stlsort) << "x"
                  << std::endl << std::endl;
    }
    return 0;
}
