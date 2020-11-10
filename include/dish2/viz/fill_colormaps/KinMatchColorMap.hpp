#pragma once
#ifndef DISH2_VIZ_FILL_COLORMAPS_KINMATCHCOLORMAP_HPP_INCLUDE
#define DISH2_VIZ_FILL_COLORMAPS_KINMATCHCOLORMAP_HPP_INCLUDE

#include "../../../../third-party/Empirical/source/tools/math.h"
#include "../../../../third-party/Empirical/source/web/color_map.h"

namespace dish2 {

struct KinMatchColorMap {

  template<typename... Args>
  KinMatchColorMap( Args&&... ){}

  template<typename ValueType>
  std::string Paint(const ValueType kin_match_array) const {
    if ( kin_match_array[0] ) return "green";
    else return "white";
  }

};

} // namespace dish2

#endif // #ifndef DISH2_VIZ_FILL_COLORMAPS_KINMATCHCOLORMAP_HPP_INCLUDE