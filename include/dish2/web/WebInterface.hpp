#pragma once
#ifndef DISH2_WEB_WEBINTERFACE_HPP_INCLUDE
#define DISH2_WEB_WEBINTERFACE_HPP_INCLUDE

#include "../spec/Spec.hpp"
#include "../world/ProcWorld.hpp"
#include "../world/ThreadWorld.hpp"

#include "ControlPanel.hpp"
#include "ViewerPanel.hpp"

namespace dish2 {

class WebInterface {

  dish2::ThreadWorld<dish2::Spec> thread_world{
    dish2::ProcWorld<dish2::Spec>{}.MakeThreadWorld(0)
  };

  dish2::ControlPanel control_panel{
    [this](const bool render_toggle, const size_t render_freq){
      thread_world.Update();
      Redraw( render_toggle, render_freq );
      return thread_world.GetUpdate();
    }
  };
  dish2::ViewerPanel view_panel{ thread_world };

public:


  void Redraw( const bool render_toggle=true, const size_t render_freq=1 ) {

    emp_assert( render_freq );
    if (
      render_toggle && thread_world.GetUpdate() % render_freq == 0
    ) {
      view_panel.Redraw();
      // systematics_dash.Redraw();
      // dominant_viewer.Text("dom_text").Redraw();
    };

  }

};

} // namespace dish2

#endif // #ifndef DISH2_WEB_WEBINTERFACE_HPP_INCLUDE
