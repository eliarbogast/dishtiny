#pragma once
#ifndef DISH2_INTROSPECTION_GET_FRACTION_SPAWN_REQUEST_HPP_INCLUDE
#define DISH2_INTROSPECTION_GET_FRACTION_SPAWN_REQUEST_HPP_INCLUDE

#include <algorithm>
#include <limits>

#include "../../../third-party/conduit/include/uitsl/polyfill/identity.hpp"

#include "../cell/cardinal_iterators/SpawnRequestWrapper.hpp"
#include "../world/iterators/LiveCellIterator.hpp"
#include "../world/iterators/WorldIteratorAdapter.hpp"
#include "../world/ThreadWorld.hpp"

#include "count_live_cells.hpp"

namespace dish2 {

template< typename Spec >
double get_fraction_spawn_request( const dish2::ThreadWorld<Spec>& world ) {

  const auto& population = world.population;

  using lcit_t = dish2::LiveCellIterator<Spec>;

  using iterator_t = dish2::WorldIteratorAdapter<
    lcit_t,
    dish2::SpawnRequestWrapper<Spec>
  >;

  if ( dish2::count_live_cells<Spec>( world ) == 0 ) {
    return std::numeric_limits<double>::quiet_NaN();
  } else return std::count_if(
    iterator_t::make_begin( lcit_t::make_begin( population ) ),
    iterator_t::make_end( lcit_t::make_end( population ) ),
    std::identity
  ) / static_cast< double >( dish2::count_live_cells<Spec>( world ) );

}

} // namespace dish2

#endif // #ifndef DISH2_INTROSPECTION_GET_FRACTION_SPAWN_REQUEST_HPP_INCLUDE
