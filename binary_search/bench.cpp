#include "binary_search.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <boost/timer/timer.hpp>

bool g_verbose = true;

template< class Cont_T, typename TAG_T >
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

template< class Cont_T, typename TAG_T >
struct container_simd_lb
{
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using const_iterator = typename container_type::const_iterator;

    container_simd_lb( const container_type& ref ) : ref_( ref ){}

    void build_index(){}

    const_iterator find( const value_type& key )
    {
        auto first = simd_algorithms::binary_search::lower_bound
            <const_iterator, value_type, TAG_T>( ref_.begin(), ref_.end(), key );

        return (first!=ref_.end() && !(key<*first)) ? first : ref_.end();
    }
private:
    const container_type& ref_;
};

template< class Cont_T, template < typename... > class Index_T, typename TAG_T >
size_t bench( const std::string& name, size_t size, size_t loop )
{
	using container_type = Cont_T;
    using index_type = Index_T< container_type, TAG_T >;

    boost::timer::cpu_timer timer;
    container_type org;

//    size_t cnt = 0;
//    std::generate_n( std::back_inserter(org), size, [&cnt](){ return cnt++; } );
//    index_type index( org );

    srand( 1 );
    std::generate_n( std::back_inserter(org), size, &rand );
    container_type sorted( org );
    std::sort( sorted.begin(), sorted.end() );
    index_type index( sorted );

    index.build_index();

    timer.start();
    for( size_t j = 0; j < loop; ++j )
    {
        size_t cnt = 0;
        for( auto i : org )
        {
            auto ret = index.find( i );

            if( ret == sorted.end() )
            {
                std::cout << "end: 0x" << std::hex << i << ", i: 0x" << cnt << std::endl;
                break;
            }
            else
            {
                if( *ret != i )
                {
                    std::cout << *ret << "," << i << "-";
                }
            }
            ++cnt;
        }
    }
    timer.stop();
    if( g_verbose )
        std::cout << "Find all " << name << ": " << timer.format();

    return timer.elapsed().wall;
}

namespace sa = simd_algorithms;
int main(int argc, char* /*argv*/[])
{
    constexpr size_t runSize = 0x00400000;
    constexpr size_t loop = 5;
    if( argc > 1 )
    {
        g_verbose = false;
        std::cout << "base,nocache_sse,cache_sse,nocache_avx,cache_avx" << std::endl;
    }
    else
    {
        std::cout << "\nsize: 0x" << std::hex << std::setw(8) << std::setfill( '0') << runSize << std::endl << std::endl;
    }
    while( 1 )
    {
        size_t base = bench< std::vector< int32_t >,
                             container_only,
                             sa::sse_tag >( "lower_bound ........", runSize, loop );

        size_t nocache = bench< std::vector< int32_t >,
                                simd_algorithms::binary_search::index_nocache,
                                sa::sse_tag >( "index_nocache SSE...", runSize, loop );
        size_t cache = bench< std::vector< int32_t >,
                              simd_algorithms::binary_search::index_cache,
                              sa::sse_tag >( "index_cache SSE.....", runSize, loop );
        size_t simdlb = bench< std::vector< int32_t >,
                               container_simd_lb,
                               sa::sse_tag >( "SIMD lower_bound SSE", runSize, loop );

        size_t nocache2 = bench< std::vector< int32_t >,
                                simd_algorithms::binary_search::index_nocache, 
                                sa::avx_tag >( "index_nocache AVX...", runSize, loop );
        size_t cache2 = bench< std::vector< int32_t >,
                               simd_algorithms::binary_search::index_cache,
                               sa::avx_tag >( "index_cache AVX.....", runSize, loop );
        size_t simdlb2 = bench< std::vector< int32_t >,
                                container_simd_lb,
                                sa::avx_tag >( "SIMD lower_bound AVX", runSize, loop );

        if( g_verbose )
        {
            std::cout
                      << std::endl << "Index Nocahe Speed up SSE.......: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(nocache) << "x"
                      << std::endl << "Index Cache Speed up SSE........: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(cache) << "x"
                      << std::endl << "SIMD lower_bound Speed up SSE...: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(simdlb) << "x"
                      << std::endl << "Index Cache/Nocache Speed up SSE: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(nocache)/static_cast<float>(cache) << "x"

                      << std::endl << "Index Nocahe Speed up AVX.......: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(nocache2) << "x"
                      << std::endl << "Index Cache Speed up AVX........: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(cache2) << "x"
                      << std::endl << "SIMD lower_bound Speed up AVX...: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(simdlb2) << "x"
                      << std::endl << std::endl;
        }
        else
        {
            std::cout
                << base << "," 
                << nocache << "," 
                << cache << "," 
                << nocache2 << "," 
                << cache2
                << std::endl;
        }
    }
    return 0;
}
