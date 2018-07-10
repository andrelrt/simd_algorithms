#ifndef SIMD_ALGORITHMS_LESS_THAN_H
#define SIMD_ALGORITHMS_LESS_THAN_H

#include <immintrin.h>
#include <x86intrin.h>
#include <type_traits>
#include <limits>

namespace simd_algorithms {

struct sse_tag {};
struct avx_tag {};

// Traits
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T = sse_tag > struct traits { };

template< typename ValueType_T > struct traits< ValueType_T, sse_tag >
{
    using simd_type = __m128i;
    constexpr static size_t simd_size = sizeof(simd_type)/sizeof(ValueType_T);

    static typename std::enable_if< std::is_same< ValueType_T, int32_t >::value,
                                    simd_type >::type max()
    {
        return _mm_set1_epi32( std::numeric_limits<int32_t>::max() );
    }

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

    static typename std::enable_if< std::is_same< ValueType_T, int32_t >::value,
                                    simd_type >::type max()
    {
        return _mm256_set1_epi32( std::numeric_limits<int32_t>::max() );
    }

    static typename std::enable_if< std::is_integral< ValueType_T >::value,
                                    simd_type >::type zero()
    {
        return _mm256_setzero_si256();
    }
};

// Mask to left index
// ------------------------------------------------------------------------------------------------
template< typename ValueType_T, typename Tag_T >
inline uint32_t mask_to_index( uint32_t mask )
{
    return (mask == 0)
        ? 0
        : (_bit_scan_reverse( mask ) + 1) / sizeof(ValueType_T);
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

// Less than index
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
            equal_mask< ValueType_T, Tag_T >( val, simdVal ) );
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
    return _mm_insert_epi32( _mm_srli_si128( vec, 4 ), val, 3 );
}

} //namespace simd_algorithms

#endif // SIMD_ALGORITHMS_BINARY_SEARCH_H
