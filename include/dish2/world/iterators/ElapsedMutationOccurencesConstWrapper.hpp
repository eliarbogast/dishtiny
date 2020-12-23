#pragma once
#ifndef DISH2_WORLD_ITERATORS_ELAPSEDMUTATIONOCCURENCESCONSTWRAPPER_HPP_INCLUDE
#define DISH2_WORLD_ITERATORS_ELAPSEDMUTATIONOCCURENCESCONSTWRAPPER_HPP_INCLUDE

#include "../../../../third-party/Empirical/include/emp/base/vector.hpp"

#include "../../cell/Cell.hpp"
#include "../../genome/RootID.hpp"

namespace dish2 {

template<typename Spec, typename ParentIterator>
class ElapsedMutationOccurencesConstWrapper : public ParentIterator {

  using parent_t = ParentIterator;

public:

  ElapsedMutationOccurencesConstWrapper() = default;

  ElapsedMutationOccurencesConstWrapper(const parent_t& in) : parent_t(in) {}

  using value_type = const size_t;
  using pointer = value_type*;
  using reference = value_type&;

  value_type& operator*() {
    return parent_t::operator*(
      ).genome->mutation_counter.mutation_occurrence_counter;
  }

  ElapsedMutationOccurencesConstWrapper& operator++() {
    parent_t::operator++();
    return *this;
  }

  ElapsedMutationOccurencesConstWrapper operator++(int) {
    const auto res = *this;
    operator++();
    return res;
  }

  ElapsedMutationOccurencesConstWrapper& operator--() {
    parent_t::operator--();
    return *this;
  }

  ElapsedMutationOccurencesConstWrapper operator--(int) {
    ElapsedMutationOccurencesConstWrapper res{ *this };
    res -= 1;
    return res;
  }

  ElapsedMutationOccurencesConstWrapper operator+(const size_t rhs) {
    ElapsedMutationOccurencesConstWrapper res{ *this };
    res += rhs;
    return res;
  }

  ElapsedMutationOccurencesConstWrapper operator-(const size_t rhs) {
    ElapsedMutationOccurencesConstWrapper res{ *this };
    res -= rhs;
    return res;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_WORLD_ITERATORS_ELAPSEDMUTATIONOCCURENCESCONSTWRAPPER_HPP_INCLUDE
