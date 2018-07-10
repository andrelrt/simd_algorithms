#ifndef SIMD_ALGORITHMS_NWAY_TREE_H
#define SIMD_ALGORITHMS_NWAY_TREE_H

#include <iostream>
#include <iomanip>
#include <vector>
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
    }

    const_iterator find( const value_type& key ) const
    {
        size_t idx = 0;
        uint32_t li;
        for( int i = 0; i < tree_.size(); ++i )
        {
            li = greater_than_index< value_type, TAG_T >( key, *tree_[i].get_simd( idx ) );
            auto debugmask = greater_than_mask< value_type, TAG_T >( key, *tree_[i].get_simd( idx ) );
            std::cout << i << " - " << idx
                      << "," << std::setw(8) << std::setfill('0') << key 
                      << "," << *tree_[i].get_simd( idx )
                      << "," << li
                      << ",0x" << std::setw(8) << std::setfill('0') << debugmask
                      << std::endl;
            idx = idx * (array_size+1) + li;
        }

        const simd_type* cmp = reinterpret_cast< const simd_type* >( &ref_[ idx * array_size ] );
        std::cout << "equal - " << idx << "," << key << "," << *cmp << std::endl;
        uint32_t mask = equal_mask< value_type, TAG_T >( key, *cmp );

        std::cout << "mask: 0x" << std::setw(8) << std::setfill('0') << mask << std::endl;
        if( mask == 0 )
        {
            return ref_.end();
        }
        uint32_t off = mask_to_index< value_type, TAG_T >( mask );
        auto it = ref_.begin();
        std::advance( it, idx * array_size + off );
        return it;
    }

private:
    constexpr static size_t array_size = traits< value_type, TAG_T >::simd_size;
    using simd_type = typename traits< value_type, TAG_T >::simd_type;

    struct tree_level
    {
        std::vector< value_type > keys_;

        const simd_type* get_simd( size_t idx ) const
        {
            return reinterpret_cast< const simd_type* >( &keys_[ idx * array_size ] );
        }

        void adjust()
        {
            size_t size = keys_.size() / array_size;
//            if( keys_.size() > size * array_size )
//            {
//                size_t cnt = array_size - (keys_.size() - size * array_size);
//                auto it = keys_.begin();
//                std::advance( it, size * array_size );
//
//                for( size_t i = 0; i < cnt; ++i )
//                {
//                    keys_.insert( it, std::numeric_limits< value_type >::min() );
//                }
//            }
            if( keys_.size() > size * array_size )
                ++size;
            size *= array_size;
            keys_.resize( size, 0x7fffffff );
            for( size_t i = 0; i < keys_.size(); i += array_size )
            {
                for( size_t j = 0; j < array_size / 2; ++j )
                {
                    std::swap( keys_[i + j], keys_[i + array_size - j - 1] );
                }
            }
        }
    };

    std::vector< tree_level > tree_;
    const container_type& ref_;

    void build_index( const container_type& cont )
    {
        if( cont.size() < array_size )
            return;

        tree_level level;
        for( int i = array_size; i < cont.size(); i += array_size )
        {
            level.keys_.push_back( cont[ i ] );
        }

        build_index( level.keys_ );

        level.adjust();
//        std::cout << tree_.size() << " - ";
//        for( auto&& i : level.keys_ )
//        {
//            std::cout << i << ",";
//        }
//        std::cout << std::endl;

        tree_.emplace_back( std::move( level ) );
        return;
    }
};


}} // namespace simd_algoriths::nway_tree

#endif //SIMD_ALGORITHMS_NWAY_TREE_H
