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

#include "../../simd_compare.h"
#include "gtest/gtest.h"

TEST(SimdCompareTest, LessThanSSE)
{
    namespace sa = simd_algorithms;
    using simd32 = typename sa::traits< int32_t >::simd_type;
    constexpr size_t size32 = sa::traits< int32_t >::simd_size;

    simd32 cmp;
    int32_t* pCmp = reinterpret_cast<int32_t*>( &cmp );
    int32_t val = 10;
    for( size_t i = 0; i < size32; ++i )
    {
        pCmp[ i ] = val;
        val += 10;
    }
    // cmp       = {40, 30, 20, 10}
    // cmp[i], i =   3   2   1   0

    EXPECT_EQ( 0x0000u, sa::greater_than_mask< int32_t >(  5, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 5, cmp );
    EXPECT_EQ( 0x000fu, sa::greater_than_mask< int32_t >( 15, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 15, cmp );
    EXPECT_EQ( 0x00ffu, sa::greater_than_mask< int32_t >( 25, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 25, cmp );
    EXPECT_EQ( 0x0fffu, sa::greater_than_mask< int32_t >( 35, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 35, cmp );
    EXPECT_EQ( 0xffffu, sa::greater_than_mask< int32_t >( 45, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 45, cmp );

    EXPECT_EQ( 0u, sa::greater_than_index< int32_t >(  5, cmp ) );
    EXPECT_EQ( 0u, sa::greater_than_index< int32_t >( 10, cmp ) );
    EXPECT_EQ( 1u, sa::greater_than_index< int32_t >( 15, cmp ) );
    EXPECT_EQ( 1u, sa::greater_than_index< int32_t >( 20, cmp ) );
    EXPECT_EQ( 2u, sa::greater_than_index< int32_t >( 25, cmp ) );
    EXPECT_EQ( 2u, sa::greater_than_index< int32_t >( 30, cmp ) );
    EXPECT_EQ( 3u, sa::greater_than_index< int32_t >( 35, cmp ) );
    EXPECT_EQ( 3u, sa::greater_than_index< int32_t >( 40, cmp ) );
    EXPECT_EQ( 4u, sa::greater_than_index< int32_t >( 45, cmp ) );

    pCmp[3] = 0x00007891;
    pCmp[2] = 0x00005576;
    pCmp[1] = 0x000041a7;
    pCmp[0] = 0x00002b83;

    EXPECT_EQ( 0x000fu, sa::greater_than_mask< int32_t >( 0x41a7, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 0x41a7, cmp );
}

TEST(SimdCompareTest, LessThanAVX)
{
    namespace sa = simd_algorithms;
    using simd32 = typename sa::traits< int32_t, sa::avx_tag >::simd_type;
    constexpr size_t size32 = sa::traits< int32_t, sa::avx_tag >::simd_size;

    simd32 cmp;
    int32_t* pCmp = reinterpret_cast<int32_t*>( &cmp );
    int32_t val = 10;
    for( size_t i = 0; i < size32; ++i )
    {
        pCmp[ i ] = val;
        val += 10;
    }
    // cmp       = {80, 70, 60, 50, 40, 30, 20, 10}
    // cmp[i], i =   7   6   5   4   3   2   1   0

    EXPECT_EQ( 0x00000000u, (sa::greater_than_mask< int32_t, sa::avx_tag >(  5, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >(  5, cmp );
    EXPECT_EQ( 0x0000000fu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 15, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 15, cmp );
    EXPECT_EQ( 0x000000ffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 25, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 25, cmp );
    EXPECT_EQ( 0x00000fffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 35, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 35, cmp );
    EXPECT_EQ( 0x0000ffffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 45, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 45, cmp );
    EXPECT_EQ( 0x000fffffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 55, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 55, cmp );
    EXPECT_EQ( 0x00ffffffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 65, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 65, cmp );
    EXPECT_EQ( 0x0fffffffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 75, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 75, cmp );
    EXPECT_EQ( 0xffffffffu, (sa::greater_than_mask< int32_t, sa::avx_tag >( 85, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 85, cmp );

    EXPECT_EQ( 0u, (sa::greater_than_index< int32_t, sa::avx_tag >(  5, cmp )) );
    EXPECT_EQ( 0u, (sa::greater_than_index< int32_t, sa::avx_tag >( 10, cmp )) );
    EXPECT_EQ( 1u, (sa::greater_than_index< int32_t, sa::avx_tag >( 15, cmp )) );
    EXPECT_EQ( 1u, (sa::greater_than_index< int32_t, sa::avx_tag >( 20, cmp )) );
    EXPECT_EQ( 2u, (sa::greater_than_index< int32_t, sa::avx_tag >( 25, cmp )) );
    EXPECT_EQ( 2u, (sa::greater_than_index< int32_t, sa::avx_tag >( 30, cmp )) );
    EXPECT_EQ( 3u, (sa::greater_than_index< int32_t, sa::avx_tag >( 35, cmp )) );
    EXPECT_EQ( 3u, (sa::greater_than_index< int32_t, sa::avx_tag >( 40, cmp )) );
    EXPECT_EQ( 4u, (sa::greater_than_index< int32_t, sa::avx_tag >( 45, cmp )) );
    EXPECT_EQ( 4u, (sa::greater_than_index< int32_t, sa::avx_tag >( 50, cmp )) );
    EXPECT_EQ( 5u, (sa::greater_than_index< int32_t, sa::avx_tag >( 55, cmp )) );
    EXPECT_EQ( 5u, (sa::greater_than_index< int32_t, sa::avx_tag >( 60, cmp )) );
    EXPECT_EQ( 6u, (sa::greater_than_index< int32_t, sa::avx_tag >( 65, cmp )) );
    EXPECT_EQ( 6u, (sa::greater_than_index< int32_t, sa::avx_tag >( 70, cmp )) );
    EXPECT_EQ( 7u, (sa::greater_than_index< int32_t, sa::avx_tag >( 75, cmp )) );
    EXPECT_EQ( 7u, (sa::greater_than_index< int32_t, sa::avx_tag >( 80, cmp )) );
    EXPECT_EQ( 8u, (sa::greater_than_index< int32_t, sa::avx_tag >( 85, cmp )) );
}

