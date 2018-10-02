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

#ifndef SIMD_ALGORITHMS_TO_LOWER_H
#define SIMD_ALGORITHMS_TO_LOWER_H

#include <iostream>
#include <iomanip>
#include "../simd_compare.h"

namespace simd_algorithms{
namespace string_algo{

template< typename TAG_T >
struct to_lower
{
    void operator()( aligned_string& str )
    {
        constexpr static size_t array_size = traits< char, TAG_T >::simd_size;
        using simd_type = typename traits< char, TAG_T >::simd_type;

        simd_type* data = (simd_type*) str.data();

        size_t sz = str.size() & ~(array_size-1);
        for( size_t i = 0; i < sz; i += array_size )
        {
            *data = iif< TAG_T >( mask_and< TAG_T >( greater_than< char, TAG_T >( *data, 'A'-1 ),
                                                     greater_than< char, TAG_T >( 'Z'+1, *data ) ),
                                  add< char, TAG_T >( *data, 0x20 ),
                                  *data );
            ++data;
        }

        size_t end = str.size();
        for( size_t i = sz; i < end; ++i )
        {
            str[i] = ( 'A' <= str[i] && str[i] <= 'Z' ) ? str[i] + 0x20 : str[i];
        }

    }
};


}} // namespace simd_algorithms::string_algo

#endif // SIMD_ALGORITHMS_TO_LOWER_H

