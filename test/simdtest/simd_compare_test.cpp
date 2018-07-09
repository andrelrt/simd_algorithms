#include "../../simd_compare.h"
#include "gtest/gtest.h"

TEST(SimdCompareTest, LessThanSSE)
{
    namespace sa = simd_algorithms;
    using simd32 = typename sa::traits< uint32_t >::simd_type;
    constexpr size_t size32 = sa::traits< uint32_t >::simd_size;

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

    EXPECT_EQ( 0xffff, sa::less_than_mask< uint32_t >(  5, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t >(  5, cmp );
    EXPECT_EQ( 0x0fff, sa::less_than_mask< uint32_t >( 15, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t >( 15, cmp );
    EXPECT_EQ( 0x00ff, sa::less_than_mask< uint32_t >( 25, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t >( 25, cmp );
    EXPECT_EQ( 0x000f, sa::less_than_mask< uint32_t >( 35, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t >( 35, cmp );
    EXPECT_EQ( 0x0000, sa::less_than_mask< uint32_t >( 45, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t >( 45, cmp );

    EXPECT_EQ( 0, sa::less_than_index< uint32_t >(  5, cmp ) );
    EXPECT_EQ( 1, sa::less_than_index< uint32_t >( 15, cmp ) );
    EXPECT_EQ( 2, sa::less_than_index< uint32_t >( 25, cmp ) );
    EXPECT_EQ( 3, sa::less_than_index< uint32_t >( 35, cmp ) );
    EXPECT_EQ( 4, sa::less_than_index< uint32_t >( 45, cmp ) );
}

TEST(SimdCompareTest, LessThanAVX)
{
    namespace sa = simd_algorithms;
    using simd32 = typename sa::traits< uint32_t, sa::avx_tag >::simd_type;
    constexpr size_t size32 = sa::traits< uint32_t, sa::avx_tag >::simd_size;

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

    EXPECT_EQ( 0xffffffff, (sa::less_than_mask< uint32_t, sa::avx_tag >(  5, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >(  5, cmp );
    EXPECT_EQ( 0x0fffffff, (sa::less_than_mask< uint32_t, sa::avx_tag >( 15, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 15, cmp );
    EXPECT_EQ( 0x00ffffff, (sa::less_than_mask< uint32_t, sa::avx_tag >( 25, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 25, cmp );
    EXPECT_EQ( 0x000fffff, (sa::less_than_mask< uint32_t, sa::avx_tag >( 35, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 35, cmp );
    EXPECT_EQ( 0x0000ffff, (sa::less_than_mask< uint32_t, sa::avx_tag >( 45, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 45, cmp );
    EXPECT_EQ( 0x00000fff, (sa::less_than_mask< uint32_t, sa::avx_tag >( 55, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 55, cmp );
    EXPECT_EQ( 0x000000ff, (sa::less_than_mask< uint32_t, sa::avx_tag >( 65, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 65, cmp );
    EXPECT_EQ( 0x0000000f, (sa::less_than_mask< uint32_t, sa::avx_tag >( 75, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 75, cmp );
    EXPECT_EQ( 0x00000000, (sa::less_than_mask< uint32_t, sa::avx_tag >( 85, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::less_than_mask< uint32_t, sa::avx_tag >( 85, cmp );

    EXPECT_EQ( 0, (sa::less_than_index< uint32_t, sa::avx_tag >(  5, cmp )) );
    EXPECT_EQ( 1, (sa::less_than_index< uint32_t, sa::avx_tag >( 15, cmp )) );
    EXPECT_EQ( 2, (sa::less_than_index< uint32_t, sa::avx_tag >( 25, cmp )) );
    EXPECT_EQ( 3, (sa::less_than_index< uint32_t, sa::avx_tag >( 35, cmp )) );
    EXPECT_EQ( 4, (sa::less_than_index< uint32_t, sa::avx_tag >( 45, cmp )) );
    EXPECT_EQ( 5, (sa::less_than_index< uint32_t, sa::avx_tag >( 55, cmp )) );
    EXPECT_EQ( 6, (sa::less_than_index< uint32_t, sa::avx_tag >( 65, cmp )) );
    EXPECT_EQ( 7, (sa::less_than_index< uint32_t, sa::avx_tag >( 75, cmp )) );
    EXPECT_EQ( 8, (sa::less_than_index< uint32_t, sa::avx_tag >( 85, cmp )) );
}

