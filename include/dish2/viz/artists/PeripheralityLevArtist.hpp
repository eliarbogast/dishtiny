#pragma once
#ifndef DISH2_VIZ_ARTISTS_PERIPHERALITYLEVARTIST_HPP_INCLUDE
#define DISH2_VIZ_ARTISTS_PERIPHERALITYLEVARTIST_HPP_INCLUDE

#include <string>

#include "../../spec/Spec.hpp"

#include "../border_colormaps/MatchBorderColorMap.hpp"
#include "../fill_colormaps/BooleanColorMap.hpp"
#include "../fill_colormaps/IsAliveColorMap.hpp"
#include "../getters/PeripheralityLevGetter.hpp"
#include "../renderers/CellBorderRenderer.hpp"
#include "../renderers/CellFillRenderer.hpp"

#include "Artist.hpp"

namespace dish2 {

namespace internal::peripherality_lev_artist {

  template<
    typename IsAliveGetter,
    typename PeripheralityLevGetter
  >
  using parent_t = dish2::Artist<
    dish2::CellFillRenderer<
      dish2::BooleanColorMap,
      PeripheralityLevGetter
    >,
    dish2::CellFillRenderer<
      dish2::IsAliveColorMap,
      IsAliveGetter
    >,
    dish2::CellBorderRenderer<
      dish2::MatchBorderColorMap,
      PeripheralityLevGetter
    >
  >;

} // namespace internal::peripherality_lev_artist

template<
  typename IsAliveGetter=dish2::IsAliveGetter<dish2::Spec>,
  typename PeripheralityLevGetter=dish2::PeripheralityLevGetter<dish2::Spec>
>
class PeripheralityLevArtist
: public internal::peripherality_lev_artist::parent_t<
  IsAliveGetter,
  PeripheralityLevGetter
> {

  using parent_t = internal::peripherality_lev_artist::parent_t<
    IsAliveGetter,
    PeripheralityLevGetter
  >;

public:

  // inherit constructors
  using parent_t::parent_t;

  static std::string GetName() { return "Peripherality by Lev"; }

  template< typename Spec >
  static size_t GetSeriesLength(const dish2::ThreadWorld<Spec>& thread_world) {
    return Spec::NLEV;
  }

};

} // namespace dish2

#endif // #ifndef DISH2_VIZ_ARTISTS_PERIPHERALITYLEVARTIST_HPP_INCLUDE
