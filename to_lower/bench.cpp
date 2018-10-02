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

#include "to_lower.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <map>
#include <boost/timer/timer.hpp>

bool g_verbose = true;
namespace sa = simd_algorithms;

struct std_to_lower
{
    void operator()( sa::aligned_string& str )
    {
        for( size_t i = 0; i < str.size(); ++i )
        {
            str[i] = ( 'A' <= str[i] && str[i] <= 'Z' ) ? str[i] + 0x20 : str[i];
        }
    }
};

struct cachesize_to_lower
{
    void operator()( sa::aligned_string& str )
    {
        size_t sz = str.size();
        for( size_t i = 0; i < sz; ++i )
        {
            str[i] = ( 'A' <= str[i] && str[i] <= 'Z' ) ? str[i] + 0x20 : str[i];
        }
    }
};

struct autovec_to_lower
{
    void operator()( sa::aligned_string& str )
    {
        size_t sz = str.size();
        char* s = (char*) str.data();
        for( size_t i = 0; i < sz; ++i )
        {
            s[i] = ( 'A' <= s[i] && s[i] <= 'Z' ) ? s[i] + 0x20 : s[i];
        }
    }
};

void do_nothing( const sa::aligned_string& );

template< typename TO_LOWER_T >
uint64_t bench( const sa::aligned_string& name, size_t size, size_t loop )
{
	using functor = TO_LOWER_T;

    boost::timer::cpu_timer timer;
    functor toLower;

    sa::aligned_string str( size, 'C' );

    timer.start();
    for( size_t j = 0; j < loop; ++j )
    {
        toLower( str );
        do_nothing( str );
    }
    timer.stop();
    if( g_verbose )
        std::cout << "To lower " << name << ": " << timer.format();

    return timer.elapsed().wall;
}

int main(int argc, char* /*argv*/[])
{
    constexpr size_t runSize = 0x00100001;
    constexpr size_t loop = 10000;
    if( argc > 1 )
    {
        g_verbose = false;
        std::cout << "base,sse,avx" << std::endl;
    }
    else
    {
        std::cout << "\nsize: 0x" << std::hex << std::setw(8) << std::setfill( '0') << runSize << std::endl << std::endl;
    }
    while( 1 )
    {
        uint64_t base = bench< cachesize_to_lower >( "Scalar ", runSize, loop );
        uint64_t sse = bench< sa::string_algo::to_lower< sa::sse_tag > >( "SSE ...", runSize, loop );
        uint64_t avx = bench< sa::string_algo::to_lower< sa::avx_tag > >( "AVX ...", runSize, loop );


        if( g_verbose )
        {
            bench< std_to_lower >( "STD ...", runSize, loop );
            bench< autovec_to_lower >( "Autovec", runSize, loop );
            std::cout
                      << std::endl << "Index Speed up SSE.......: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(sse) << "x"

                      << std::endl << "Index Speed up AVX.......: " << std::fixed << std::setprecision(2)
                      << static_cast<float>(base)/static_cast<float>(avx) << "x"

                      << std::endl << std::endl;
        }
        else
        {
            std::cout
                << base << ","
                << sse << ","
                << avx
                << std::endl;
        }
    }
    return 0;
}
