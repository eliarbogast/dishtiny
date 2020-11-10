#pragma once
#ifndef DISH2_CELL_CARDINAL_ITERATORS_EPOCHWRAPPER_HPP_INCLUDE
#define DISH2_CELL_CARDINAL_ITERATORS_EPOCHWRAPPER_HPP_INCLUDE

#include "../../../../third-party/Empirical/source/base/vector.h"

#include "../../peripheral/readable_state/ReadableState.hpp"

#include "../Cardinal.hpp"

namespace dish2 {

template<typename Spec>
class EpochWrapper
: public emp::vector<Cardinal<Spec>>::iterator {

  using parent_t = typename emp::vector<Cardinal<Spec>>::iterator;

public:

  EpochWrapper() = default;

  EpochWrapper(const parent_t& in) : parent_t(in) {}

  using value_type = dish2::Epoch;
  using pointer = value_type*;
  using reference = value_type&;

  value_type& operator*() {
    return parent_t::operator*().peripheral.readable_state.template Get<
      dish2::Epoch
    >();
  }

  value_type* operator->() { return &operator*(); }

  EpochWrapper& operator++() {
    parent_t::operator++();
    return *this;
  }

  EpochWrapper operator++(int) {
    const auto res = *this;
    operator++();
    return res;
  }

  EpochWrapper& operator--() {
    parent_t::operator--();
    return *this;
  }

  EpochWrapper operator--(int) {
    EpochWrapper res{ *this };
    res -= 1;
    return res;
  }

  EpochWrapper operator+(const size_t rhs) {
    EpochWrapper res{ *this };
    res += rhs;
    return res;
  }

  EpochWrapper operator-(const size_t rhs) {
    EpochWrapper res{ *this };
    res -= rhs;
    return res;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_CELL_CARDINAL_ITERATORS_EPOCHWRAPPER_HPP_INCLUDE