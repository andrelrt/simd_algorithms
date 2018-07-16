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

#ifndef SIMD_ALGORITHMS_BINARY_SEARCH_H
#define SIMD_ALGORITHMS_BINARY_SEARCH_H

#include <iostream>
#include <immintrin.h>
#include <x86intrin.h>
#include <array>
#include <iterator>
#include "../simd_compare.h"

namespace simd_algorithms{
namespace binary_search{

template< class Cont_T, typename TAG_T >
class index_nocache
{
public:
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using const_iterator = typename container_type::const_iterator;

    index_nocache( const container_type& ref )
        : ref_( ref ){}

    void build_index()
    {
        size_t step = ref_.size() / (array_size + 1);

        const_iterator end = ref_.begin();
        value_type* pCmp = reinterpret_cast< value_type* >( &cmp_ );
        for( size_t i = 0; i < array_size; ++i )
        {
            std::advance( end, step );
            pCmp[i] = *end;
        }
    }

    const_iterator find( const value_type& key ) const
    {
        size_t i = greater_than_index< value_type, TAG_T >( key, cmp_ );
        size_t step = ref_.size() / (array_size + 1);

        const_iterator beg = ref_.begin();
        std::advance( beg, i * step );
        const_iterator end;
        if( i == array_size )
        {
            end = ref_.end();
        }
        else
        {
            end = beg;
            std::advance( end, step + 1 );
        }

        auto first = std::lower_bound( beg, end, key );
        return (first!=end && !(key<*first)) ? first : ref_.end();
    }

private:
    constexpr static size_t array_size = traits< value_type, TAG_T >::simd_size;

    const container_type& ref_;
    typename traits< value_type, TAG_T >::simd_type cmp_;
};

//any container smart_step
template< class Cont_T, typename TAG_T >
class index_cache
{
public:
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using const_iterator = typename container_type::const_iterator;

    index_cache( const container_type& ref )
        : ref_( ref ){}

    void build_index()
    {
        size_t step = ref_.size() / (array_size + 1);

        value_type* pCmp = reinterpret_cast< value_type* >( &cmp_ );
        const_iterator it = ref_.begin();
        ranges_[ 0 ] = it;
        for( size_t i = 1; i <= array_size; ++i )
        {
            std::advance( it, step );
            ranges_[ i ] = it;
            pCmp[i-1] = *it;
        }
        ranges_[ array_size+1 ] = std::prev(ref_.end());
    }

    const_iterator find( const value_type& key ) const
    {
        size_t i = greater_than_index< value_type, TAG_T >( key, cmp_ );
        auto end = std::next( ranges_[ i + 1 ] );
        auto first = std::lower_bound( ranges_[ i ], end, key );
        return (first!=end && !(key<*first)) ? first : ref_.end();
    }

private:
    constexpr static size_t array_size = traits< value_type, TAG_T >::simd_size;

    const container_type& ref_;
    std::array< const_iterator, array_size + 2 > ranges_;
    typename traits< value_type, TAG_T >::simd_type cmp_;
};

template <class ForwardIterator, class T, typename TAG_T >
ForwardIterator lower_bound( ForwardIterator beg, ForwardIterator end, const T& key )
{
    using value_type     = typename std::iterator_traits< ForwardIterator >::value_type;
    using const_iterator = ForwardIterator;
    using simd_type      = typename traits< value_type, TAG_T >::simd_type;

    constexpr static size_t array_size = traits< value_type, TAG_T >::simd_size;

    size_t size = std::distance( beg, end );
    size_t step = size / (array_size + 1);

    // Create SIMD search key
    simd_type cmp;
    const_iterator it = beg;
    value_type* pCmp = reinterpret_cast< value_type* >( &cmp );
    for( size_t i = 0; i < array_size; ++i )
    {
        std::advance( it, step );
        pCmp[i] = *it;
    }

    // N-Way search
    size_t i = greater_than_index< value_type, TAG_T >( key, cmp );

    // Recalculate iterators
    it = beg;
    std::advance( it, i * step );
    const_iterator itEnd;
    if( i == array_size )
    {
        itEnd = end;
    }
    else
    {
        itEnd = it;
        std::advance( itEnd, step + 1 );
    }

    // Standard lower_bound on 1/(n+1) of container size
    return std::lower_bound( it, itEnd, key );
}

}} // namespace simd_algoriths::binary_search

#endif // SIMD_ALGORITHMS_BINARY_SEARCH_H
