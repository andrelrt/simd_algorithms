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

#ifndef SIMD_ALGORITHMS_LESS_THAN_H
#define SIMD_ALGORITHMS_LESS_THAN_H

#include <immintrin.h>
#include <x86intrin.h>
#include <type_traits>
#include <limits>
#include <vector>
#include <boost/align/aligned_allocator.hpp>

namespace simd_algorithms {

struct sse_tag {};
struct avx_tag {};

// Aligned vector
// ------------------------------------------------------------------------------------------------
template< typename Val_T >
using aligned_vector = std::vector< Val_T, boost::alignment::aligned_allocator<Val_T, 64> >;

// Traits
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag > struct traits { };

template< typename ValueType_T > struct traits< ValueType_T, sse_tag >
{
    using simd_type = __m128i;
    constexpr static size_t simd_size = sizeof(simd_type)/sizeof(ValueType_T);

//    static typename std::enable_if< std::is_same< ValueType_T, int8_t >::value,
//                                    simd_type >::type max()
//    {
//        return _mm_set1_epi8( std::numeric_limits<int8_t>::max() );
//    }
//
//    static typename std::enable_if< std::is_same< ValueType_T, int32_t >::value,
//                                    simd_type >::type max()
//    {
//        return _mm_set1_epi32( std::numeric_limits<int32_t>::max() );
//    }

    static typename std::enable_if< std::is_integral< ValueType_T >::value,
                                    simd_type >::type zero()
    {
        return _mm_setzero_si128();
    }
};

template< typename ValueType_T > struct traits< ValueType_T, avx_tag >
{
    using simd_type = __m256i;
    constexpr static size_t simd_size = sizeof(simd_type)/sizeof(ValueType_T);

//    static typename std::enable_if< std::is_same< ValueType_T, int32_t >::value,
//                                    simd_type >::type max()
//    {
//        return _mm256_set1_epi32( std::numeric_limits<int32_t>::max() );
//    }

    static typename std::enable_if< std::is_integral< ValueType_T >::value,
                                    simd_type >::type zero()
    {
        return _mm256_setzero_si256();
    }
};

// Mask operations
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T >
inline uint32_t mask_to_index( uint32_t mask )
{
    return (mask == 0)
        ? 0
        : (_bit_scan_reverse( mask ) + 1) / sizeof(ValueType_T);
}

template< typename ValueType_T, typename Tag_T >
uint32_t result_to_mask( typename traits< ValueType_T, Tag_T >::simd_type )
{
    return 0;
}

template<> inline uint32_t
result_to_mask<int32_t, sse_tag>( __m128i retMask )
{
    return _mm_movemask_epi8( retMask );
}

template<> inline uint32_t
result_to_mask<int32_t, avx_tag>( __m256i retMask )
{
    return _mm256_movemask_epi8( retMask );
}

// Select item
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
typename traits< ValueType_T, Tag_T >::simd_type
select( typename traits< ValueType_T, Tag_T >::simd_type lhs, int8_t /*index*/ )
{
    return lhs;
}

template<> inline __m128i
select<int32_t, sse_tag>( __m128i lhs, int8_t index )
{
    int32_t i32 = index << 2;
    return _mm_shuffle_epi8( lhs, _mm_set_epi8( i32+3, i32+2, i32+1, i32,
                                                i32+3, i32+2, i32+1, i32,
                                                i32+3, i32+2, i32+1, i32,
                                                i32+3, i32+2, i32+1, i32 ) );
}

template<> inline __m256i
select<int32_t, avx_tag>( __m256i lhs, int8_t index )
{
    return _mm256_permutevar8x32_epi32( lhs, _mm256_set_epi32( index, index, index, index,
                                                               index, index, index, index  ) );
}

// Invert bits
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
typename traits< ValueType_T, Tag_T >::simd_type
invert( typename traits< ValueType_T, Tag_T >::simd_type lhs )
{
    return lhs;
}

template<> inline __m128i
invert<int32_t, sse_tag>( __m128i lhs )
{
    return _mm_xor_si128( _mm_set1_epi8( 0xff ), lhs );
}

template<> inline __m256i
invert<int32_t, avx_tag>( __m256i lhs )
{
    return _mm256_xor_si256( _mm256_set1_epi8( 0xff ), lhs );
}

// iif - Ternary operator
// ------------------------------------------------------------------------------------------------
template< typename Tag_T = sse_tag >
typename traits< int8_t, Tag_T >::simd_type
iif( typename traits< int8_t, Tag_T >::simd_type mask,
     typename traits< int8_t, Tag_T >::simd_type /*iftrue*/,
     typename traits< int8_t, Tag_T >::simd_type /*iffalse*/ )
{
    return mask;
}

template<> inline __m128i
iif<sse_tag>( __m128i mask, __m128i iftrue, __m128i iffalse )
{
    return _mm_blendv_epi8( iffalse, iftrue, mask );
}

template<> inline __m256i
iif<avx_tag>( __m256i mask, __m256i iftrue, __m256i iffalse )
{
    return _mm256_blendv_epi8( iffalse, iftrue, mask );
}

// Bit AND
// ------------------------------------------------------------------------------------------------
template< typename Tag_T = sse_tag >
typename traits< int8_t, Tag_T >::simd_type
mask_and( typename traits< int8_t, Tag_T >::simd_type lhs,
          typename traits< int8_t, Tag_T >::simd_type /*rhs*/ )
{
    return lhs;
}

template<> inline __m128i
mask_and<sse_tag>( __m128i lhs, __m128i rhs )
{
    return _mm_and_si128( lhs, rhs );
}

template<> inline __m256i
mask_and<avx_tag>( __m256i lhs, __m256i rhs )
{
    return _mm256_and_si256( lhs, rhs );
}

// Add
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
typename traits< ValueType_T, Tag_T >::simd_type
add( typename traits< ValueType_T, Tag_T >::simd_type, ValueType_T )
{
    return traits< ValueType_T, Tag_T >::zero();
}

template<> inline typename traits< char, sse_tag >::simd_type
add< char, sse_tag >( __m128i sval, char val )
{
    return _mm_add_epi8( sval, _mm_set1_epi8( val ) );
}

template<> inline typename traits< char, avx_tag >::simd_type
add< char, avx_tag >( __m256i sval, char val )
{
    return _mm256_add_epi8( sval, _mm256_set1_epi8( val ) );
}


// Greater than
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
typename traits< ValueType_T, Tag_T >::simd_type
greater_than( typename traits< ValueType_T, Tag_T >::simd_type, ValueType_T )
{
    return traits< ValueType_T, Tag_T >::zero();
}


template<> inline typename traits< char, sse_tag >::simd_type
greater_than< char, sse_tag >( __m128i cmp, char key )
{
    return _mm_cmpgt_epi8( cmp, _mm_set1_epi8( key ) );
}

template<> inline typename traits< char, avx_tag >::simd_type
greater_than< char, avx_tag >( __m256i cmp, char key )
{
    return _mm256_cmpgt_epi8( cmp, _mm256_set1_epi8( key ) );
}

template< typename ValueType_T, typename Tag_T = sse_tag >
typename traits< ValueType_T, Tag_T >::simd_type
greater_than( ValueType_T, typename traits< ValueType_T, Tag_T >::simd_type )
{
    return traits< ValueType_T, Tag_T >::zero();
}

template<> inline typename traits< int32_t, sse_tag >::simd_type
greater_than< int32_t, sse_tag >( int32_t key, __m128i cmp )
{
    return _mm_cmpgt_epi32( _mm_set1_epi32( key ), cmp );
}

template<> inline typename traits< char, sse_tag >::simd_type
greater_than< char, sse_tag >( char key, __m128i cmp )
{
    return _mm_cmpgt_epi8( _mm_set1_epi8( key ), cmp );
}

template<> inline typename traits< int32_t, avx_tag >::simd_type
greater_than< int32_t, avx_tag >( int32_t key, __m256i cmp )
{
    return _mm256_cmpgt_epi32( _mm256_set1_epi32( key ), cmp );
}

template<> inline typename traits< char, avx_tag >::simd_type
greater_than< char, avx_tag >( char key, __m256i cmp )
{
    return _mm256_cmpgt_epi8( _mm256_set1_epi8( key ), cmp );
}

template< typename ValueType_T, typename Tag_T = sse_tag >
typename traits< ValueType_T, Tag_T >::simd_type
greater_than( typename traits< ValueType_T, Tag_T >::simd_type,
              typename traits< ValueType_T, Tag_T >::simd_type )
{
    return traits< ValueType_T, Tag_T >::zero();
}

template<> inline typename traits< int32_t, sse_tag >::simd_type
greater_than< int32_t, sse_tag >( __m128i lhs, __m128i rhs )
{
    return _mm_cmpgt_epi32( lhs, rhs );
}

template<> inline typename traits< int32_t, avx_tag >::simd_type
greater_than< int32_t, avx_tag >( __m256i lhs, __m256i rhs )
{
    return _mm256_cmpgt_epi32( lhs, rhs );
}

// Greater than mask
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
inline uint32_t greater_than_mask( ValueType_T, typename traits< ValueType_T, Tag_T >::simd_type )
{
    return 0;
}

template<> inline uint32_t
greater_than_mask< int32_t, sse_tag >( int32_t key, __m128i cmp )
{
    return _mm_movemask_epi8( _mm_cmpgt_epi32( _mm_set1_epi32( key ), cmp ) );
}

template<> inline uint32_t
greater_than_mask< int32_t, avx_tag >( int32_t key, __m256i cmp )
{
    return _mm256_movemask_epi8( _mm256_cmpgt_epi32( _mm256_set1_epi32( key ), cmp ) );
}

// Mask to index
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
inline uint32_t greater_than_index( ValueType_T val,
                                    typename traits< ValueType_T, Tag_T >::simd_type simdVal )
{
    return mask_to_index< ValueType_T, Tag_T >(
            greater_than_mask< ValueType_T, Tag_T >( val, simdVal ) );
}

// Equal mask
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
inline uint32_t equal_mask( ValueType_T, typename traits< ValueType_T, Tag_T >::simd_type )
{
    return 0;
}

template<> inline uint32_t
equal_mask< int32_t, sse_tag >( int32_t key, __m128i cmp )
{
    return _mm_movemask_epi8( _mm_cmpeq_epi32( _mm_set1_epi32( key ), cmp ) );
}

template<> inline uint32_t
equal_mask< int32_t, avx_tag >( int32_t key, __m256i cmp )
{
    return _mm256_movemask_epi8( _mm256_cmpeq_epi32( _mm256_set1_epi32( key ), cmp ) );
}

// Equal index
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
inline uint32_t equal_index( ValueType_T val,
                             typename traits< ValueType_T, Tag_T >::simd_type simdVal )
{
    return mask_to_index< ValueType_T, Tag_T >(
            equal_mask< ValueType_T, Tag_T >( val, simdVal ) ) - 1;
}

// Low insert
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag >
inline typename traits< ValueType_T, Tag_T >::simd_type
low_insert( typename traits< ValueType_T, Tag_T >::simd_type, ValueType_T )
{
    return traits< ValueType_T, Tag_T >::zero();
}

template<> inline __m128i
low_insert< int32_t, sse_tag >( __m128i vec, int32_t val )
{
    return _mm_insert_epi32( _mm_shuffle_epi32( vec, _MM_SHUFFLE( 3, 3, 2, 1 ) ), val, 3 );
}

template<> inline __m256i
low_insert< int32_t, avx_tag >( __m256i vec, int32_t val )
{
    return _mm256_insert_epi32( _mm256_permutevar8x32_epi32( vec, _mm256_set_epi32( 7, 7, 6, 5, 4, 3, 2, 1 ) ),
                                val, 7 );
}

} //namespace simd_algorithms

#endif // SIMD_ALGORITHMS_BINARY_SEARCH_H
