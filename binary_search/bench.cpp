#include "binary_search.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/timer/timer.hpp>


template< class Cont_T >
struct container_only
{
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using const_iterator = typename container_type::const_iterator;

    container_only( const container_type& ref ) : ref_( ref ){}

    void build_index(){}

    const_iterator find( const value_type& key )
    {
        auto first = std::lower_bound( ref_.begin(), ref_.end(), key );
        return (first!=ref_.end() && !(key<*first)) ? first : ref_.end();
    }
private:
    const container_type& ref_;
};

template< class Cont_T, template < typename... > class Index_T >
size_t bench( const std::string& name, size_t size, size_t loop )
{
	using container_type = Cont_T;
    using index_type = Index_T< container_type >;

    boost::timer::cpu_timer timer;
    container_type org;

    size_t cnt = 0;
    std::generate_n( std::back_inserter(org), size, [&cnt](){ return cnt++; } );
    index_type index( org );

    index.build_index();

    timer.start();
    for( size_t j = 0; j < loop; ++j )
    {
        for( auto i : org )
        {
            auto ret = index.find( i );

            if( ret == org.end() )
            {
                std::cout << "end- " << std::hex << i << std::endl;
                break;
            }
            else
            {
                if( *ret != i )
                {
                    std::cout << *ret << "," << i << "-";
                }
            }
        }
    }
    timer.stop();
    std::cout << "Find all " << name << ": " << timer.format();

    return timer.elapsed().wall;
}

int main(int /*argc*/, char* /*argv*/[])
{
    std::cout << "\nsize: 0x00ff'ffff\n\n";
    while( 1 )
    {
        size_t base = bench< std::vector< uint32_t >, container_only >( "lower_bound ...", 0x00ffffff, 10 );
        size_t nocache = bench< std::vector< uint32_t >,
                                simd_algorithms::binary_search::index_nocache >( "index_nocache .", 0x00ffffff, 10 );

        size_t cache = bench< std::vector< uint32_t >,
                              simd_algorithms::binary_search::index_cache >( "index_cache ...", 0x00ffffff, 10 );

        std::cout << std::endl << "Index Nocahe Diff: " << std::fixed << std::setprecision(2)
                  << 100.0f * (((float) nocache)/((float) base) - 1.0f) << "%"
                  << std::endl << "Index Cache Diff: " << std::fixed << std::setprecision(2)
                  << 100.0f * (((float) cache)/((float) base) - 1.0f) << "%"
                  << std::endl << "Index Cache / Index Nocache Diff: " << std::fixed << std::setprecision(2)
                  << 100.0f * (((float) cache)/((float) nocache) - 1.0f) << "%"
                  << std::endl << std::endl;
    }
    return 0;
}
