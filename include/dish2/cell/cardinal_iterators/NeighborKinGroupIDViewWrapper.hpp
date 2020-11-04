#pragma once
#ifndef DISH2_CELL_CARDINAL_ITERATORS_NEIGHBORKINGROUPIDVIEWWRAPPER_HPP_INCLUDE
#define DISH2_CELL_CARDINAL_ITERATORS_NEIGHBORKINGROUPIDVIEWWRAPPER_HPP_INCLUDE

#include "../../../../third-party/Empirical/source/base/vector.h"

#include "../../peripheral/readable_state/ReadableState.hpp"

#include "../Cardinal.hpp"

namespace dish2 {

template<typename Spec>
class NeighborKinGroupIDViewWrapper
: public emp::vector<Cardinal<Spec>>::iterator {

  using parent_t = typename emp::vector<Cardinal<Spec>>::iterator;

public:

  NeighborKinGroupIDViewWrapper() = default;

  NeighborKinGroupIDViewWrapper(const parent_t& in) : parent_t(in) {}

  using value_type = dish2::KinGroupIDView< Spec >;
  using pointer = value_type*;
  using reference = value_type&;

  value_type& operator*() {
    return parent_t::operator*().state_node_input.Get().template Get<
      dish2::KinGroupIDView< Spec >
    >();
  }

  value_type* operator->() { return &operator*(); }

  NeighborKinGroupIDViewWrapper& operator++() {
    parent_t::operator++();
    return *this;
  }

  NeighborKinGroupIDViewWrapper operator++(int) {
    const auto res = *this;
    operator++();
    return res;
  }

  NeighborKinGroupIDViewWrapper& operator--() {
    parent_t::operator--();
    return *this;
  }

  NeighborKinGroupIDViewWrapper operator--(int) {
    NeighborKinGroupIDViewWrapper res{ *this };
    res -= 1;
    return res;
  }

  NeighborKinGroupIDViewWrapper operator+(const size_t rhs) {
    NeighborKinGroupIDViewWrapper res{ *this };
    res += rhs;
    return res;
  }

  NeighborKinGroupIDViewWrapper operator-(const size_t rhs) {
    NeighborKinGroupIDViewWrapper res{ *this };
    res -= rhs;
    return res;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_CELL_CARDINAL_ITERATORS_NEIGHBORKINGROUPIDVIEWWRAPPER_HPP_INCLUDE
