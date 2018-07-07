#include "../../simd_compare.h"
#include "gtest/gtest.h"

TEST(SimdCompareTest, LessThan)
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
    // cmp = {10, 20, 30, 40}

    sac::less_than< uint32_t > simdLess;
    EXPECT_EQ( 0, simdLess( cmp,  5 ) );
    EXPECT_EQ( 1, simdLess( cmp, 15 ) );
    EXPECT_EQ( 2, simdLess( cmp, 25 ) );
    EXPECT_EQ( 3, simdLess( cmp, 35 ) );
    EXPECT_EQ( 4, simdLess( cmp, 45 ) );
}

