#pragma once
#ifndef DISH2_QUORUM_QUORUMMESSAGE_HPP_INCLUDE
#define DISH2_QUORUM_QUORUMMESSAGE_HPP_INCLUDE

#include <algorithm>
#include <iostream>

#include "../../../third-party/conduit/include/uitsl/algorithm/unset_mask.hpp"
#include "../../../third-party/Empirical/include/emp/bits/BitSet.hpp"
#include "../../../third-party/signalgp-lite/include/sgpl/utility/ThreadLocalRandom.hpp"

#include "../config/cfg.hpp"

namespace dish2 {

template< typename Spec >
class QuorumMessage {

  // TODO swtich back to emp::array after https://github.com/devosoft/Empirical/issues/408 is resolved
  std::array< emp::BitSet<64>, Spec::NLEV > data{};

public:

  QuorumMessage() = default;

  QuorumMessage(std::in_place_t) {
    auto& rand = sgpl::tlrand.Get();
    for ( size_t lev{}; lev < Spec::NLEV; ++lev ) {
      if ( rand.P( dish2::cfg.P_SET_QUORUM_BIT()[lev] ) ) {
        data[lev].Set( rand.GetUInt( data[lev].GetSize() ) );
      }
    }
  }

  bool operator==(const QuorumMessage& other) const {
    return data == other.data;
  }

  void UnsetMask( const QuorumMessage& mask ) {
    std::transform(
      std::begin( data ),
      std::end( data ),
      std::begin( mask.data ),
      std::begin( data ),
      [](const auto data_bits, const auto mask_bits){
        return uitsl::unset_mask( data_bits, mask_bits );
      }
    );
  }

  QuorumMessage operator~() const {
    QuorumMessage res;
    std::transform(
      std::begin( data ),
      std::end( data ),
      std::begin( res.data ),
      []( const auto& this_bits ){ return ~this_bits; }
    );
    return res;
  }

  QuorumMessage operator|(const QuorumMessage& other) const {
    QuorumMessage res;
    std::transform(
      std::begin( data ),
      std::end( data ),
      std::begin( other.data ),
      std::begin( res.data ),
      []( const auto& this_bits, const auto& other_bits ){
        return this_bits | other_bits;
      }
    );
    return res;
  }

  QuorumMessage operator&(const QuorumMessage& other) const {
    QuorumMessage res;
    std::transform(
      std::begin( data ),
      std::end( data ),
      std::begin( other.data ),
      std::begin( res.data ),
      []( const auto& this_bits, const auto& other_bits ){
        return this_bits & other_bits;
      }
    );
    return res;
  }

  const QuorumMessage& operator|=(const QuorumMessage& other) {
    std::transform(
      std::begin( data ),
      std::end( data ),
      std::begin( other.data ),
      std::begin( data ),
      []( const auto& this_bits, const auto& other_bits ){
        return this_bits | other_bits;
      }
    );
    return *this;
  }

  void Clear() { for (auto& bs : data) bs.Clear(); }

  void ClearLev( const size_t lev ) { data[ lev ].Clear(); }

  size_t GetNumBits(const size_t lev) const { return data[ lev ].CountOnes(); }

  size_t GetNumBits() const {
    return std::accumulate(
      std::begin( data ),
      std::end( data ),
      0,
      [](const auto& cumulator, const auto& bitset){
        return cumulator + bitset.CountOnes();
      }
    );
  }

  friend std::ostream& operator<<(std::ostream &out, const QuorumMessage& qm){
    for ( const auto& bs : qm.data ) out << bs;
    return out;
  }


};

} // namespace dish2

#endif // #ifndef DISH2_QUORUM_QUORUMMESSAGE_HPP_INCLUDE
