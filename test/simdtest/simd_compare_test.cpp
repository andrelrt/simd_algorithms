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

    EXPECT_EQ( 0x0000, sa::greater_than_mask< int32_t >(  5, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 5, cmp );
    EXPECT_EQ( 0x000f, sa::greater_than_mask< int32_t >( 15, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 15, cmp );
    EXPECT_EQ( 0x00ff, sa::greater_than_mask< int32_t >( 25, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 25, cmp );
    EXPECT_EQ( 0x0fff, sa::greater_than_mask< int32_t >( 35, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 35, cmp );
    EXPECT_EQ( 0xffff, sa::greater_than_mask< int32_t >( 45, cmp ) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t >( 45, cmp );

    EXPECT_EQ( 0, sa::greater_than_index< int32_t >(  5, cmp ) );
    EXPECT_EQ( 0, sa::greater_than_index< int32_t >( 10, cmp ) );
    EXPECT_EQ( 1, sa::greater_than_index< int32_t >( 15, cmp ) );
    EXPECT_EQ( 1, sa::greater_than_index< int32_t >( 20, cmp ) );
    EXPECT_EQ( 2, sa::greater_than_index< int32_t >( 25, cmp ) );
    EXPECT_EQ( 2, sa::greater_than_index< int32_t >( 30, cmp ) );
    EXPECT_EQ( 3, sa::greater_than_index< int32_t >( 35, cmp ) );
    EXPECT_EQ( 3, sa::greater_than_index< int32_t >( 40, cmp ) );
    EXPECT_EQ( 4, sa::greater_than_index< int32_t >( 45, cmp ) );

    pCmp[3] = 0x00007891;
    pCmp[2] = 0x00005576;
    pCmp[1] = 0x000041a7;
    pCmp[0] = 0x00002b83;

    EXPECT_EQ( 0x000f, sa::greater_than_mask< int32_t >( 0x41a7, cmp ) ) << "hex: 0x" << std::hex
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

    EXPECT_EQ( 0x00000000, (sa::greater_than_mask< int32_t, sa::avx_tag >(  5, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >(  5, cmp );
    EXPECT_EQ( 0x0000000f, (sa::greater_than_mask< int32_t, sa::avx_tag >( 15, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 15, cmp );
    EXPECT_EQ( 0x000000ff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 25, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 25, cmp );
    EXPECT_EQ( 0x00000fff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 35, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 35, cmp );
    EXPECT_EQ( 0x0000ffff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 45, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 45, cmp );
    EXPECT_EQ( 0x000fffff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 55, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 55, cmp );
    EXPECT_EQ( 0x00ffffff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 65, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 65, cmp );
    EXPECT_EQ( 0x0fffffff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 75, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 75, cmp );
    EXPECT_EQ( 0xffffffff, (sa::greater_than_mask< int32_t, sa::avx_tag >( 85, cmp )) ) << "hex: 0x" << std::hex
        << std::setw(8) << std::setfill( '0' ) << sa::greater_than_mask< int32_t, sa::avx_tag >( 85, cmp );

    EXPECT_EQ( 0, (sa::greater_than_index< int32_t, sa::avx_tag >(  5, cmp )) );
    EXPECT_EQ( 0, (sa::greater_than_index< int32_t, sa::avx_tag >( 10, cmp )) );
    EXPECT_EQ( 1, (sa::greater_than_index< int32_t, sa::avx_tag >( 15, cmp )) );
    EXPECT_EQ( 1, (sa::greater_than_index< int32_t, sa::avx_tag >( 20, cmp )) );
    EXPECT_EQ( 2, (sa::greater_than_index< int32_t, sa::avx_tag >( 25, cmp )) );
    EXPECT_EQ( 2, (sa::greater_than_index< int32_t, sa::avx_tag >( 30, cmp )) );
    EXPECT_EQ( 3, (sa::greater_than_index< int32_t, sa::avx_tag >( 35, cmp )) );
    EXPECT_EQ( 3, (sa::greater_than_index< int32_t, sa::avx_tag >( 40, cmp )) );
    EXPECT_EQ( 4, (sa::greater_than_index< int32_t, sa::avx_tag >( 45, cmp )) );
    EXPECT_EQ( 4, (sa::greater_than_index< int32_t, sa::avx_tag >( 50, cmp )) );
    EXPECT_EQ( 5, (sa::greater_than_index< int32_t, sa::avx_tag >( 55, cmp )) );
    EXPECT_EQ( 5, (sa::greater_than_index< int32_t, sa::avx_tag >( 60, cmp )) );
    EXPECT_EQ( 6, (sa::greater_than_index< int32_t, sa::avx_tag >( 65, cmp )) );
    EXPECT_EQ( 6, (sa::greater_than_index< int32_t, sa::avx_tag >( 70, cmp )) );
    EXPECT_EQ( 7, (sa::greater_than_index< int32_t, sa::avx_tag >( 75, cmp )) );
    EXPECT_EQ( 7, (sa::greater_than_index< int32_t, sa::avx_tag >( 80, cmp )) );
    EXPECT_EQ( 8, (sa::greater_than_index< int32_t, sa::avx_tag >( 85, cmp )) );
}

