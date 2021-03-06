// MIT License
//
// Copyright (c) 2018 André Tupinambá
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "bubble_sort.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <boost/timer/timer.hpp>

bool g_verbose = true;
namespace sa = simd_algorithms;

template< class Cont_T, typename TAG_T >
struct stl_sort
{
	using container_type = Cont_T;

    void sort( container_type& cont )
    {
        std::sort( std::begin( cont ), std::end( cont ) );
    }
};

template< class Cont_T, typename TAG_T >
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

template< class Cont_T, template< typename...> class Sort_T, typename TAG_T >
uint64_t bench( const std::string& name, size_t size, size_t loop )
{
	using container_type = Cont_T;
    using sort_type = Sort_T< container_type, TAG_T >;

    boost::timer::cpu_timer timer;
    container_type org;

    srand(1);
    std::generate_n( std::back_inserter(org), size, &rand );

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
    if( g_verbose )
        std::cout << "Sort all " << name << ": " << timer.format();

    return timer.elapsed().wall;
}

int main(int argc, char* /*argv*/[])
{
    constexpr size_t runSize = 0x00010000;
    constexpr size_t loop = 1;
    if( argc > 1 )
    {
        g_verbose = false;
        std::cout << "count,Bubble sort,SSE Bubble 1,SSE Bubble 2,AVX Bubble 1,AVX Bubble 2" << std::endl;
    }
    else
    {
        std::cout << "\nsize: 0x" << std::hex << std::setw(8) << std::setfill( '0') << runSize << std::endl << std::endl;
    }
    size_t cnt = 0;
    while( 1 )
    {
        bench< sa::aligned_vector< int32_t >, stl_sort,
               sa::sse_tag >( "STL sort ........", runSize, loop );

        uint64_t bsort = bench< sa::aligned_vector< int32_t >,
                              basic_bubble_sort,
                              sa::sse_tag >( "Bubble sort .....", runSize, loop );
        uint64_t ssesort = bench< sa::aligned_vector< int32_t >,
                                sa::sort::bubble,
                                sa::sse_tag >( "SSE Bubble sort .", runSize, loop );
        uint64_t ssesort2 = bench< sa::aligned_vector< int32_t >,
                                 sa::sort::bubble2,
                                 sa::sse_tag >( "SSE Bubble2 sort ", runSize, loop );
        uint64_t avxsort = bench< sa::aligned_vector< int32_t >,
                                sa::sort::bubble,
                                sa::avx_tag >( "AVX Bubble sort .", runSize, loop );
        uint64_t avxsort2 = bench< sa::aligned_vector< int32_t >,
                                 sa::sort::bubble2,
                                 sa::avx_tag >( "AVX Bubble2 sort ", runSize, loop );


        if( g_verbose )
        {
            std::cout
                << std::endl << "SSE Speed up ......: " << std::fixed << std::setprecision(2)
                << static_cast<float>(bsort)/static_cast<float>(ssesort) << "x"
                << std::endl << "SSE2 Speed up .....: " << std::fixed << std::setprecision(2)
                << static_cast<float>(bsort)/static_cast<float>(ssesort2) << "x"
                << std::endl << "SSE2/SSE Speed up .: " << std::fixed << std::setprecision(2)
                << static_cast<float>(ssesort)/static_cast<float>(ssesort2) << "x"

                << std::endl << "AVX Speed up ......: " << std::fixed << std::setprecision(2)
                << static_cast<float>(bsort)/static_cast<float>(avxsort) << "x"
                << std::endl << "AVX2 Speed up .....: " << std::fixed << std::setprecision(2)
                << static_cast<float>(bsort)/static_cast<float>(avxsort2) << "x"
                << std::endl << "AVX2/AVX Speed up .: " << std::fixed << std::setprecision(2)
                << static_cast<float>(avxsort)/static_cast<float>(avxsort2) << "x"

                << std::endl << std::endl;
        }
        else
        {
            std::cout
                << ++cnt << ","
                << bsort << ","
                << ssesort << ","
                << ssesort2 << ","
                << avxsort << ","
                << avxsort2
                << std::endl;
        }
    }
    return 0;
}
