#ifndef SIMD_ALGORITHMS_LESS_THAN_H
#define SIMD_ALGORITHMS_LESS_THAN_H

#include <immintrin.h>
#include <x86intrin.h>
#include <type_traits>
#include <limits>

namespace simd_algorithms {

namespace traits {
struct sse_tag {};
struct avx_tag {};

template< typename ValueType_T, typename Tag_T = sse_tag > struct simd_traits { };

template< typename ValueType_T > struct simd_traits< ValueType_T, sse_tag >
{
    using simd_type = __m128i;
    constexpr static size_t simd_size = sizeof(simd_type)/sizeof(ValueType_T);

    static typename std::enable_if< std::is_same< ValueType_T,
                                                  uint32_t >::value,
                                    simd_type >::type max()
    {
        return _mm_set1_epi32( std::numeric_limits<uint32_t>::max() );
    }
};

template< typename ValueType_T > struct simd_traits< ValueType_T, avx_tag >
{
    using simd_type = __m256i;
    constexpr static size_t simd_size = sizeof(simd_type)/sizeof(ValueType_T);
};

} // namespace traits

//
namespace compare{

template< typename ValueType_T, typename Tag_T = traits::sse_tag > struct greater_than { };
template< typename ValueType_T, typename Tag_T = traits::sse_tag > struct less_than { };
template< typename ValueType_T, typename Tag_T = traits::sse_tag > struct low_inserter {};

template<> struct low_inserter< uint32_t, traits::sse_tag >
{
    inline __m128i operator()( __m128i cmp, uint32_t key ) {
        return _mm_insert_epi32( _mm_srli_si128( cmp, 4 ), key, 3 );
    }
};

template<> struct less_than< uint32_t, traits::sse_tag >
{
    inline size_t operator()( __m128i cmp, uint32_t key ) {
        uint32_t mask = _mm_movemask_epi8( _mm_cmpgt_epi32( _mm_set1_epi32( key ), cmp ) );
        return (mask == 0) ? 0 : (_bit_scan_reverse( mask ) >> 2) + 1;
    }
};

// SSE2 - SSE4.2
template<> struct greater_than< uint8_t, traits::sse_tag >
{
    inline size_t operator()( uint8_t key, __m128i cmp ) {
        uint32_t mask = _mm_movemask_epi8( _mm_cmpgt_epi8( _mm_set1_epi8( key ), cmp ) );
        return _bit_scan_reverse( mask + 1 );
    }
};

template<> struct greater_than< uint16_t, traits::sse_tag >
{
    inline size_t operator()( uint16_t key, __m128i cmp ) {
        uint32_t mask = _mm_movemask_epi8( _mm_cmpgt_epi16( _mm_set1_epi16( key ), cmp ) );
        return _bit_scan_reverse( mask + 1 ) >> 1;
    }
};

template<> struct greater_than< uint32_t, traits::sse_tag >
{
    inline size_t operator()( uint32_t key, __m128i cmp ) {
        uint32_t mask = _mm_movemask_epi8( _mm_cmpgt_epi32( _mm_set1_epi32( key ), cmp ) );
        return _bit_scan_reverse( mask + 1 ) >> 2;
    }
};

template<> struct greater_than< uint64_t, traits::sse_tag >
{
    inline size_t operator()( uint64_t key, __m128i cmp ) {
        uint32_t mask = _mm_movemask_epi8( _mm_cmpgt_epi64( _mm_set1_epi64x( key ), cmp ) );
        return _bit_scan_reverse( mask + 1 ) >> 3;
    }
};

// AVX - AVX2
template<> struct greater_than< uint8_t, traits::avx_tag >
{
    inline size_t operator()( uint8_t key, __m256i cmp ) {
        uint32_t mask = _mm256_movemask_epi8( _mm256_cmpgt_epi8( _mm256_set1_epi8( key ), cmp ) );
        return (mask == 0) ? 0 : (_bit_scan_reverse( mask ) + 1);
    }
};

template<> struct greater_than< uint16_t, traits::avx_tag >
{
    inline size_t operator()( uint16_t key, __m256i cmp ) {
        uint32_t mask = _mm256_movemask_epi8( _mm256_cmpgt_epi16( _mm256_set1_epi16( key ), cmp ) );
        return (mask == 0) ? 0 : (_bit_scan_reverse( mask ) + 1) >> 1;
    }
};

template<> struct greater_than< uint32_t, traits::avx_tag >
{
    inline size_t operator()( uint32_t key, __m256i cmp ) {
        uint32_t mask = _mm256_movemask_epi8( _mm256_cmpgt_epi32( _mm256_set1_epi32( key ), cmp ) );
        return (mask == 0) ? 0 : (_bit_scan_reverse( mask ) + 1) >> 2;
    }
};

template<> struct greater_than< uint64_t, traits::avx_tag >
{
    inline size_t operator()( uint64_t key, __m256i cmp ) {
        uint32_t mask = _mm256_movemask_epi8( _mm256_cmpgt_epi64( _mm256_set1_epi64x( key ), cmp ) );
        return (mask == 0) ? 0 : (_bit_scan_reverse( mask ) + 1) >> 3;
    }
};

}} // namespace simd_algorithms::compare

#endif // SIMD_ALGORITHMS_BINARY_SEARCH_H
