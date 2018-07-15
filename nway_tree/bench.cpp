#include "nway_tree.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <map>
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
struct map_index
{
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using const_iterator = typename container_type::const_iterator;

    map_index( const container_type& ref ) : ref_( ref ){}

    void build_index()
    {
        for( auto it = ref_.begin(); it != ref_.end(); ++it )
        {
            index_[ *it ] = it;
        }
    }

    const_iterator find( const value_type& key )
    {
        auto it = index_.find( key );
        return (it != index_.end()) ? it->second : ref_.end() ;
    }

private:
    const container_type& ref_;
    std::map< value_type, const_iterator > index_;
};

template< class Cont_T, template < typename... > class Index_T, typename TAG_T >
uint64_t bench( const std::string& name, size_t size, size_t loop )
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
    constexpr size_t loop = 10;
    if( argc > 1 )
    {
        g_verbose = false;
        std::cout << "base,index_sse,index_avx" << std::endl;
    }
    else
    {
        std::cout << "\nsize: 0x" << std::hex << std::setw(8) << std::setfill( '0') << runSize << std::endl << std::endl;
    }
    while( 1 )
    {
        uint64_t base = bench< sa::aligned_vector< int32_t >,
                             container_only,
                             sa::sse_tag >( "lower_bound .", runSize, loop );

        uint64_t index1 = bench< sa::aligned_vector< int32_t >,
                               simd_algorithms::nway_tree::index,
                               sa::sse_tag >( "index SSE ...", runSize, loop );

        uint64_t index2 = bench< sa::aligned_vector< int32_t >,
                               simd_algorithms::nway_tree::index,
                               sa::avx_tag >( "index AVX ...", runSize, loop );

        if( g_verbose )
        {
            bench< sa::aligned_vector< int32_t >, map_index,
                   sa::sse_tag >( "std::map ....", runSize, loop );

            std::cout
                      << std::endl << "Index Speed up SSE.......: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(index1) << "x"

                      << std::endl << "Index Speed up AVX.......: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(index2) << "x"

                      << std::endl << std::endl;
        }
        else
        {
            std::cout
                << base << ","
                << index1 << ","
                << index2
                << std::endl;
        }
    }
    return 0;
}
