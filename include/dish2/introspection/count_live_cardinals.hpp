#pragma once
#ifndef DISH2_INTROSPECTION_COUNT_LIVE_CARDINALS_HPP_INCLUDE
#define DISH2_INTROSPECTION_COUNT_LIVE_CARDINALS_HPP_INCLUDE

#include <algorithm>

#include "../../../third-party/signalgp-lite/include/sgpl/introspection/count_modules.hpp"

#include "../cell/Cell.hpp"
#include "../world/iterators/LiveCellIterator.hpp"
#include "../world/ThreadWorld.hpp"

namespace dish2 {

template< typename Spec >
size_t count_live_cardinals( const dish2::ThreadWorld<Spec>& world ) {

  const auto& population = world.population;

  return std::accumulate(
    dish2::LiveCellIterator<Spec>::make_begin( population ),
    dish2::LiveCellIterator<Spec>::make_end( population ),
    0,
    []( const auto accumulator, const auto& cell ) {
      return accumulator + cell.GetNumCardinals();
    }
  );

}

} // namespace dish2

#endif // #ifndef DISH2_INTROSPECTION_COUNT_LIVE_CARDINALS_HPP_INCLUDE
