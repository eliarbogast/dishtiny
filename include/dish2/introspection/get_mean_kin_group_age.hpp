#pragma once
#ifndef DISH2_INTROSPECTION_GET_MEAN_KIN_GROUP_AGE_HPP_INCLUDE
#define DISH2_INTROSPECTION_GET_MEAN_KIN_GROUP_AGE_HPP_INCLUDE

#include <algorithm>
#include <limits>

#include "../cell/cardinal_iterators/KinGroupAgeWrapper.hpp"
#include "../world/iterators/LiveCellIterator.hpp"
#include "../world/iterators/WorldIteratorAdapter.hpp"
#include "../world/ThreadWorld.hpp"

#include "count_live_cells.hpp"

namespace dish2 {

template< typename Spec >
double get_mean_kin_group_age(
  const dish2::ThreadWorld<Spec>& world, const size_t lev
) {

  const auto& population = world.population;

  using lcit_t = dish2::LiveCellIterator<Spec>;

  using iterator_t = dish2::WorldIteratorAdapter<
    lcit_t,
    dish2::KinGroupAgeWrapper<Spec>
  >;

  if ( dish2::count_live_cells<Spec>( world ) == 0 ) {
    return std::numeric_limits<double>::quiet_NaN();
  } else return std::accumulate(
    iterator_t::make_begin( lcit_t::make_begin( population ) ),
    iterator_t::make_end( lcit_t::make_end( population ) ),
    0.0,
    [lev]( const double accumulator, const auto& kin_group_age ) {
      return accumulator + kin_group_age.Get( lev );
    }
  ) / dish2::count_live_cells< Spec >( world );

}

} // namespace dish2

#endif // #ifndef DISH2_INTROSPECTION_GET_MEAN_KIN_GROUP_AGE_HPP_INCLUDE
