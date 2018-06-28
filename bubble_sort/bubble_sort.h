#ifndef SIMD_ALGORITHMS_BUBBLE_SORT_H
#define SIMD_ALGORITHMS_BUBBLE_SORT_H

#include <immintrin.h>
#include <x86intrin.h>
#include <iostream>
#include "simd_compare.h"

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
        size_t end = (cont.size() & ~(array_size - 1)) - array_size + 1;
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

}} // namespace simd_algoriths::bubble_sort

#endif // SIMD_ALGORITHMS_BUBBLE_SORT_H
