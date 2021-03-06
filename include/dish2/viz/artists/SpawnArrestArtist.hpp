#pragma once
#ifndef DISH2_VIZ_ARTISTS_SPAWNARRESTARTIST_HPP_INCLUDE
#define DISH2_VIZ_ARTISTS_SPAWNARRESTARTIST_HPP_INCLUDE

#include <string>

#include "../../spec/Spec.hpp"

#include "../border_colormaps/KinGroupIDBorderColorMap.hpp"
#include "../fill_colormaps/BooleanColorMap.hpp"
#include "../fill_colormaps/IsAliveColorMap.hpp"
#include "../getters/IsAliveGetter.hpp"
#include "../getters/KinGroupIDGetter.hpp"
#include "../getters/SpawnArrestGetter.hpp"
#include "../renderers/CardinalFillRenderer.hpp"
#include "../renderers/CellBorderRenderer.hpp"
#include "../renderers/CellFillRenderer.hpp"

#include "Artist.hpp"

namespace dish2 {

namespace internal::spawn_arrest_artist {

  template<
    typename SpawnArrestGetter,
    typename IsAliveGetter,
    typename KinGroupIDGetter
  >
  using parent_t = dish2::Artist<
    dish2::CardinalFillRenderer<
      dish2::BooleanColorMap,
      SpawnArrestGetter
    >,
    dish2::CellFillRenderer<
      dish2::IsAliveColorMap,
      IsAliveGetter
    >,
    dish2::CellBorderRenderer<
      dish2::KinGroupIDBorderColorMap,
      KinGroupIDGetter
    >
  >;

} // namespace internal::spawn_arrest_artist

template<
  typename SpawnArrestGetter
    =dish2::SpawnArrestGetter<dish2::Spec>,
  typename IsAliveGetter=dish2::IsAliveGetter<dish2::Spec>,
  typename KinGroupIDGetter=dish2::KinGroupIDGetter<dish2::Spec>
>
class SpawnArrestArtist
: public internal::spawn_arrest_artist::parent_t<
  SpawnArrestGetter,
  IsAliveGetter,
  KinGroupIDGetter
> {

  using parent_t = internal::spawn_arrest_artist::parent_t<
    SpawnArrestGetter,
    IsAliveGetter,
    KinGroupIDGetter
  >;

public:

  // inherit constructors
  using parent_t::parent_t;

  static std::string GetName() { return "Spawn Arrest"; }

};

} // namespace dish2

#endif // #ifndef DISH2_VIZ_ARTISTS_SPAWNARRESTARTIST_HPP_INCLUDE
