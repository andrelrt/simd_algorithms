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

#ifndef SIMD_ALGORITHMS_NWAY_TREE_H
#define SIMD_ALGORITHMS_NWAY_TREE_H

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

std::ostream& operator<<( std::ostream& out, __m256i val )
{
    uint32_t* pval = reinterpret_cast<uint32_t*>( &val );
    out << std::hex << "("
       << std::setw(8) << std::setfill('0') << pval[0] << ","
       << std::setw(8) << std::setfill('0') << pval[1] << ","
       << std::setw(8) << std::setfill('0') << pval[2] << ","
       << std::setw(8) << std::setfill('0') << pval[3] << ","
       << std::setw(8) << std::setfill('0') << pval[4] << ","
       << std::setw(8) << std::setfill('0') << pval[5] << ","
       << std::setw(8) << std::setfill('0') << pval[6] << ","
       << std::setw(8) << std::setfill('0') << pval[7] << ")";
    return out;
}


namespace simd_algorithms{
namespace nway_tree{

template< class Cont_T, typename TAG_T >
class index
{
public:
	using container_type = Cont_T;
    using value_type     = typename container_type::value_type;
    using const_iterator = typename container_type::const_iterator;

    index( const container_type& ref )
        : ref_( ref ){}

    void build_index()
    {
        build_index( ref_ );
//        std::cout << "- tree size: " << std::dec << tree_.size() << std::endl;
//        size_t total = 0;
//        for( size_t i = 0; i < tree_.size(); ++i )
//        {
//            std::cout << "  level " << i << " size: " << tree_[i].keys_.size() << std::endl;
//            total += tree_[i].keys_.size();
//        }
//        std::cout << "  total: 0x" << std::hex
//                  << std::setw(8) << std::setfill('0') << total << std::endl;
    }

    const_iterator find( const value_type& key ) const
    {
        size_t idx = 0;
        for( auto&& level : tree_ )
        {
            uint32_t li = greater_than_index< value_type, TAG_T >( key, *level.get_simd( idx ) );
            idx = idx * array_size + li;
        }

        const simd_type* cmp = reinterpret_cast< const simd_type* >( &ref_[ idx * array_size ] );
        uint32_t mask = equal_mask< value_type, TAG_T >( key, *cmp );

        if( mask == 0 )
        {
            return ref_.end();
        }
        uint32_t off = mask_to_index< value_type, TAG_T >( mask ) - 1;
        auto it = ref_.begin();
        std::advance( it, idx * array_size + off );
        return it;
    }

private:
    constexpr static size_t array_size = traits< value_type, TAG_T >::simd_size;
    using simd_type = typename traits< value_type, TAG_T >::simd_type;

    struct tree_level
    {
        aligned_vector< value_type > keys_;

        const simd_type* get_simd( size_t idx ) const
        {
            return reinterpret_cast< const simd_type* >( &keys_[ idx * array_size ] );
        }

        void adjust()
        {
            size_t size = keys_.size() / array_size;
            if( keys_.size() > size * array_size )
                ++size;
            size *= array_size;
            keys_.resize( size, std::numeric_limits< value_type >::max() );
        }
    };

    aligned_vector< tree_level > tree_;
    const container_type& ref_;

    void build_index( const container_type& cont )
    {
        if( cont.size() <= array_size )
            return;

        tree_level level;
        for( size_t i = array_size-1; i < cont.size(); i += array_size )
        {
            level.keys_.push_back( cont[ i ] );
        }

        build_index( level.keys_ );

        level.adjust();
        tree_.emplace_back( std::move( level ) );
        return;
    }
};

}} // namespace simd_algoriths::nway_tree

#endif //SIMD_ALGORITHMS_NWAY_TREE_H
