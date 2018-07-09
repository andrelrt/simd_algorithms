#include "../../simd_compare.h"
#include "gtest/gtest.h"

TEST(SimdCompareTest, LessThanSSE)
{
    namespace sac = simd_algorithms::compare;
    namespace sat = simd_algorithms::traits;
    using simd32 = typename sat::simd_traits< uint32_t >::simd_type;
    constexpr size_t size32 = sat::simd_traits< uint32_t >::simd_size;

    simd32 cmp;
    uint32_t* pCmp = reinterpret_cast<uint32_t*>( &cmp );
    uint32_t val = 10 * size32;
    for( size_t i = 0; i < size32; ++i )
    {
        pCmp[ i ] = val;
        val -= 10;
    }
    // cmp       = {10, 20, 30, 40}
    // cmp[i], i =   3   2   1   0

    sac::less_than_mask< uint32_t > simdLessMask;
    EXPECT_EQ( 0xffff, simdLessMask(  5, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask(  5, cmp );
    EXPECT_EQ( 0x0fff, simdLessMask( 15, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 15, cmp );
    EXPECT_EQ( 0x00ff, simdLessMask( 25, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 25, cmp );
    EXPECT_EQ( 0x000f, simdLessMask( 35, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 35, cmp );
    EXPECT_EQ( 0x0000, simdLessMask( 45, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 45, cmp );

    sac::less_than_index< uint32_t > simdLess;
    EXPECT_EQ( 0, simdLess(  5, cmp ) );
    EXPECT_EQ( 1, simdLess( 15, cmp ) );
    EXPECT_EQ( 2, simdLess( 25, cmp ) );
    EXPECT_EQ( 3, simdLess( 35, cmp ) );
    EXPECT_EQ( 4, simdLess( 45, cmp ) );
}

TEST(SimdCompareTest, LessThanAVX)
{
    namespace sac = simd_algorithms::compare;
    namespace sat = simd_algorithms::traits;
    using simd32 = typename sat::simd_traits< uint32_t, sat::avx_tag >::simd_type;
    constexpr size_t size32 = sat::simd_traits< uint32_t, sat::avx_tag >::simd_size;

    simd32 cmp;
    uint32_t* pCmp = reinterpret_cast<uint32_t*>( &cmp );
    uint32_t val = 10 * size32;
    for( size_t i = 0; i < size32; ++i )
    {
        pCmp[ i ] = val;
        val -= 10;
    }
    // cmp       = {10, 20, 30, 40, 50, 60, 70, 80}
    // cmp[i], i =   7   6   5   4   3   2   1   0

    sac::less_than_mask< uint32_t, sat::avx_tag > simdLessMask;
    EXPECT_EQ( 0xffffffff, simdLessMask(  5, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask(  5, cmp );
    EXPECT_EQ( 0x0fffffff, simdLessMask( 15, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 15, cmp );
    EXPECT_EQ( 0x00ffffff, simdLessMask( 25, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 25, cmp );
    EXPECT_EQ( 0x000fffff, simdLessMask( 35, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 35, cmp );
    EXPECT_EQ( 0x0000ffff, simdLessMask( 45, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 45, cmp );
    EXPECT_EQ( 0x00000fff, simdLessMask( 55, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 55, cmp );
    EXPECT_EQ( 0x000000ff, simdLessMask( 65, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 65, cmp );
    EXPECT_EQ( 0x0000000f, simdLessMask( 75, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 75, cmp );
    EXPECT_EQ( 0x00000000, simdLessMask( 85, cmp ) ) << "hex: 0x"
        << std::hex << std::setw(8) << std::setfill( '0' ) << simdLessMask( 85, cmp );

    sac::less_than_index< uint32_t, sat::avx_tag > simdLess;
    EXPECT_EQ( 0, simdLess(  5, cmp ) );
    EXPECT_EQ( 1, simdLess( 15, cmp ) );
    EXPECT_EQ( 2, simdLess( 25, cmp ) );
    EXPECT_EQ( 3, simdLess( 35, cmp ) );
    EXPECT_EQ( 4, simdLess( 45, cmp ) );
    EXPECT_EQ( 5, simdLess( 55, cmp ) );
    EXPECT_EQ( 6, simdLess( 65, cmp ) );
    EXPECT_EQ( 7, simdLess( 75, cmp ) );
    EXPECT_EQ( 8, simdLess( 85, cmp ) );
}

