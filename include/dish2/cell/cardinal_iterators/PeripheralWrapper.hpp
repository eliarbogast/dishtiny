#pragma once
#ifndef DISH2_CELL_CARDINAL_ITERATORS_PERIPHERALWRAPPER_HPP_INCLUDE
#define DISH2_CELL_CARDINAL_ITERATORS_PERIPHERALWRAPPER_HPP_INCLUDE

#include "../../../../third-party/Empirical/source/base/vector.h"

#include "../Cardinal.hpp"

namespace dish2 {

template<typename Spec>
class PeripheralWrapper
: public emp::vector<Cardinal<Spec>>::iterator {

  using parent_t = typename emp::vector<Cardinal<Spec>>::iterator;

public:

  PeripheralWrapper() = default;

  PeripheralWrapper(const parent_t& in) : parent_t(in) {}

  using value_type = typename Cardinal<Spec>::peripheral_t;
  using pointer = value_type*;
  using reference = value_type&;

  value_type& operator*() { return parent_t::operator*().cpu; }

  value_type* operator->() { return &operator*(); }

  PeripheralWrapper operator+(const size_t rhs) {
    PeripheralWrapper res{ *this };
    res += rhs;
    return res;
  }

  PeripheralWrapper operator-(const size_t rhs) {
    PeripheralWrapper res{ *this };
    res -= rhs;
    return res;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_CELL_CARDINAL_ITERATORS_PERIPHERALWRAPPER_HPP_INCLUDE
