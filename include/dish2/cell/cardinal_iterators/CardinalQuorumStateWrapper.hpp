#pragma once
#ifndef DISH2_CELL_CARDINAL_ITERATORS_CARDINALQUORUMSTATEWRAPPER_HPP_INCLUDE
#define DISH2_CELL_CARDINAL_ITERATORS_CARDINALQUORUMSTATEWRAPPER_HPP_INCLUDE

#include "../../../../third-party/Empirical/source/base/vector.h"

#include "../../quorum/CardinalQuorumState.hpp"

#include "../Cardinal.hpp"

namespace dish2 {

template<typename Spec>
class CardinalQuorumStateWrapper
: public emp::vector<Cardinal<Spec>>::iterator {

  using parent_t = typename emp::vector<Cardinal<Spec>>::iterator;

public:

  // inherit constructors
  CardinalQuorumStateWrapper() = default;

  CardinalQuorumStateWrapper(const parent_t& in) : parent_t(in) {}

  using value_type = typename dish2::CardinalQuorumState<Spec>;
  using pointer = value_type*;
  using reference = value_type&;

  value_type& operator*() {
    return parent_t::operator*().cardinal_quorum_state;
  }

  value_type* operator->() { return &operator*(); }

  CardinalQuorumStateWrapper& operator++() {
    parent_t::operator++();
    return *this;
  }

  CardinalQuorumStateWrapper operator++(int) {
    const auto res = *this;
    operator++();
    return res;
  }

  CardinalQuorumStateWrapper& operator--() {
    parent_t::operator--();
    return *this;
  }

  CardinalQuorumStateWrapper operator--(int) {
    return *this - 1;
  }

  CardinalQuorumStateWrapper operator+(const size_t rhs) {
    CardinalQuorumStateWrapper res{ *this };
    res += rhs;
    return res;
  }

  CardinalQuorumStateWrapper operator-(const size_t rhs) {
    CardinalQuorumStateWrapper res{ *this };
    res -= rhs;
    return res;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_CELL_CARDINAL_ITERATORS_CARDINALQUORUMSTATEWRAPPER_HPP_INCLUDE
