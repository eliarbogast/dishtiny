#pragma once
#ifndef DISH2_RECORD_DRAWINGS_GRIDDRAWER_HPP_INCLUDE
#define DISH2_RECORD_DRAWINGS_GRIDDRAWER_HPP_INCLUDE

#include <algorithm>
#include <chrono>
#include <ratio>
#include <string>

#include "../../../../third-party/Empirical/include/emp/tools/keyname_utils.hpp"
#include "../../../../third-party/Empirical/include/emp/tools/string_utils.hpp"
#include "../../../../third-party/Empirical/include/emp/web/Canvas.hpp"

#include "../../world/ThreadWorld.hpp"

#include "../make_filename/make_drawing_filename.hpp"

namespace dish2 {

template<typename Artist>
class GridDrawer {

  Artist artist;

  emp::web::Canvas canvas{ 2000, 2000 };

  const dish2::ThreadWorld<dish2::Spec>& thread_world;

  size_t thread_idx;

  size_t series_idx;

public:

  GridDrawer(
    const dish2::ThreadWorld<dish2::Spec>& thread_world_,
    const size_t thread_idx_,
    const size_t series_idx_=0
  )
  : artist( thread_world_, series_idx_ )
  , thread_world( thread_world_ )
  , thread_idx( thread_idx_ )
  , series_idx( series_idx_ )
  { }

  void SaveToFile() {

    const std::string filename = dish2::make_drawing_filename(
      series_idx,
      thread_idx,
      thread_world.GetUpdate(),
      emp::slugify( Artist::GetName() )
    );

    artist.Draw( canvas );

    canvas.SavePNG( filename );

  }


};

} // namespace dish2

#endif // #ifndef DISH2_RECORD_DRAWINGS_GRIDDRAWER_HPP_INCLUDE