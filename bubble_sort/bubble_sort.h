#ifndef SIMD_ALGORITHMS_BUBBLE_SORT_H
#define SIMD_ALGORITHMS_BUBBLE_SORT_H

#include <immintrin.h>
#include <x86intrin.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "../simd_compare.h"

std::ostream& operator<<( std::ostream& out, __m128i val )
{
    uint32_t* pval = reinterpret_cast<uint32_t*>( &val );
    out << std::hex << "("
       << std::setw(8) << std::setfill('0') << pval[0] << ","
       << std::setw(8) << std::setfill('0') << pval[1] << ","
       << std::setw(8) << std::setfill('0') << pval[2] << ","
       << std::setw(8) << std::setfill('0') << pval[3] << ")";
    return out;
}

namespace simd_algorithms{
namespace sort{

template< class Cont_T >
class bubble
{
public:
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using simd_type      = typename traits::simd_traits< value_type >::simd_type;

    void sort( container_type& cont )
    {
        static auto lt = compare::less_than< value_type >();
        bool sorted;
        size_t end = std::max<int64_t>( 0, static_cast<int64_t>(cont.size()) - array_size );
        do
        {
            sorted = true;
            for( size_t i = 0; i < end; ++i )
            {
                auto cmp = reinterpret_cast<simd_type*>( &cont[i+1] );
                uint32_t off = lt( *cmp, cont[i] );

                if( off != 0 )
                {
                    std::swap( cont[i], cont[i + off] );
                    sorted = false;
                }
            }

            for( size_t i = end; i < cont.size(); ++i )
            {
                if( cont[i-1] > cont[i] )
                {
                    std::swap( cont[i-1], cont[i] );
                    sorted = false;
                }
            }

        } while( !sorted );
    }

private:
    constexpr static size_t array_size = traits::simd_traits< value_type >::simd_size;
};


template< class Cont_T >
class bubble2
{
public:
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using simd_type      = typename traits::simd_traits< value_type >::simd_type;

    void sort( container_type& cont )
    {
        static auto lt = compare::less_than< value_type >();
        static auto ins = compare::low_inserter< value_type >();
        bool sorted;
        size_t end = std::max<int64_t>( 0, static_cast<int64_t>(cont.size()) - array_size );
        do
        {
            sorted = true;
            auto cmp = *reinterpret_cast<simd_type*>( &cont[0] );
            //simd_type cmp;
            //for( size_t i = 0; i < array_size; ++i )
            //{
            //    cmp[ i ] = cont[ i ];
            //}
    //        std::cerr << "S  " << cmp << std::endl;
            for( size_t i = 0; i < end; ++i )
            {
                cmp = ins( cmp, cont[ i + array_size ] );
     //           std::cerr << "S" << i << " " << cmp << std::endl;
                uint32_t off = lt( cmp, cont[i] );

                if( off != 0 )
                {
                    std::swap( cont[i], cont[i + off] );
                    sorted = false;
                }
            }

            for( size_t i = end; i < cont.size(); ++i )
            {
                if( cont[i-1] > cont[i] )
                {
                    std::swap( cont[i-1], cont[i] );
                    sorted = false;
                }
            }

        } while( !sorted );
    }

private:
    constexpr static size_t array_size = traits::simd_traits< value_type >::simd_size;
};

}} // namespace simd_algoriths::bubble_sort

#endif // SIMD_ALGORITHMS_BUBBLE_SORT_H
