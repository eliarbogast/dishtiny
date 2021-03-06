#pragma once
#ifndef DISH2_CELL_CARDINAL_ITERATORS_SPAWNARRESTWRAPPER_HPP_INCLUDE
#define DISH2_CELL_CARDINAL_ITERATORS_SPAWNARRESTWRAPPER_HPP_INCLUDE

#include "../../../../third-party/Empirical/include/emp/base/vector.hpp"

#include "../../peripheral/readable_state/ReadableState.hpp"

#include "../Cardinal.hpp"

namespace dish2 {

template<typename Spec>
class SpawnArrestWrapper
: public emp::vector<Cardinal<Spec>>::iterator {

  using parent_t = typename emp::vector<Cardinal<Spec>>::iterator;

public:

  SpawnArrestWrapper() = default;

  SpawnArrestWrapper(const parent_t& in) : parent_t(in) {}

  using value_type = dish2::SpawnArrest;
  using pointer = value_type*;
  using reference = value_type&;

  value_type& operator*() {
    return parent_t::operator*().peripheral.readable_state.template Get<
      dish2::SpawnArrest
    >();
  }

  value_type* operator->() { return &operator*(); }

  SpawnArrestWrapper& operator++() {
    parent_t::operator++();
    return *this;
  }

  SpawnArrestWrapper operator++(int) {
    const auto res = *this;
    operator++();
    return res;
  }

  SpawnArrestWrapper& operator--() {
    parent_t::operator--();
    return *this;
  }

  SpawnArrestWrapper operator--(int) {
    SpawnArrestWrapper res{ *this };
    res -= 1;
    return res;
  }

  SpawnArrestWrapper operator+(const size_t rhs) {
    SpawnArrestWrapper res{ *this };
    res += rhs;
    return res;
  }

  SpawnArrestWrapper operator-(const size_t rhs) {
    SpawnArrestWrapper res{ *this };
    res -= rhs;
    return res;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_CELL_CARDINAL_ITERATORS_SPAWNARRESTWRAPPER_HPP_INCLUDE
