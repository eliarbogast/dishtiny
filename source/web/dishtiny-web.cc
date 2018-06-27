#include <string>

#include "web/web.h"
#include "web/Animate.h"
#include "base/vector.h"
#include "tools/Random.h"

#include "../TinyWorld.h"
#include "../GridAnimator.h"
#include "../GridSpec.h"
#include "../DishtinyConfig.h"
#include "../CustomConfig.h"
namespace UI = emp::web;

struct Interface {

  UI::Document viewer;        //< Div that contains the canvas viewer.
  UI::Document button_dash;  //< Div that contains the button dashboard.
  UI::Canvas canvas;

  emp::vector<std::function<void()>> cbs_beforedraw;
  emp::vector<std::function<void()>> cbs_afterdraw;

  emp::vector<emp::Ptr<GridAnimator<int>>> gas_channelmanager;
  emp::vector<emp::Ptr<GridAnimator<double>>> gas_resourcemanager;
  emp::vector<emp::Ptr<GridAnimator<int>>> gas_signalmanager;

  DishtinyConfig dconfig;
  GridSpec spec;
  CustomConfig cconfig;
  emp::Random rand;

  TinyWorld world;

  UI::Animate fforward;

  size_t update;

  Interface()
    : viewer("emp_viewer")
    , button_dash("emp_button_dash")
    , canvas(500,500)
    , cbs_beforedraw()
    , cbs_afterdraw()
    , gas_channelmanager()
    , dconfig()
    , spec(dconfig)
    , cconfig(dconfig, spec)
    , rand(dconfig.SEED())
    , world(1000, rand, dconfig, cconfig)
    , fforward([this](){
        // used to fast forward without drawing
        // must be placed inside Animate to be interruptable
        world.Update(25);
        for (size_t i = 0; i < cbs_afterdraw.size(); ++i) {
          cbs_afterdraw[i]();
        }
      })
    , update(0)
  {

    // setup page skeleton
    viewer << canvas;
    viewer << "<br />";
    auto ud_text = viewer.AddText("ud_text");
    ud_text << "Update: " << UI::Live(update);
    std::cout << typeid(ud_text).name();

    // register callbacks for animators
    cbs_beforedraw.push_back([this](){ world.Update(); });

    cbs_afterdraw.push_back([this](){
      update = world.GetUpdate();
      viewer.Text("ud_text").Redraw();
    });

    world.MakeChannelAnimators(
      gas_channelmanager,
      canvas,
      cbs_beforedraw,
      cbs_afterdraw
    );

    world.MakeResourceAnimators(
      gas_resourcemanager,
      canvas,
      cbs_beforedraw,
      cbs_afterdraw
    );

    world.MakeSignalAnimators(
      gas_signalmanager,
      canvas,
      cbs_beforedraw,
      cbs_afterdraw
    );

    button_dash << fforward.GetToggleButton("FastForward");
    button_dash << "&nbsp;&nbsp;&nbsp;";
    button_dash << "Run without Animation (Fast Forward)" << "<br />";

    MakeControl(gas_channelmanager, "Channel_IDs");
    MakeControl(gas_resourcemanager, "Resources");
    MakeControl(gas_signalmanager, "Signals");

  }

private:
  /*
   * Place buttons to run and step vector of animators onto button_dash.
   */
  template<typename STATE_TYPE>
  void MakeControl(
      emp::vector<emp::Ptr<GridAnimator<STATE_TYPE>>>& gas,
      std::string name
    ) {

    for (size_t lev = 0; lev < gas.size(); ++lev) {
      button_dash << gas[lev]->GetToggleButton(
          "toggle"
          +name
          +std::to_string(lev)
        );
      button_dash << "&nbsp;&nbsp;&nbsp;";
      button_dash << gas[lev]->GetStepButton(
          "step"
          +name
          +std::to_string(lev)
        );
      button_dash << "&nbsp;&nbsp;&nbsp;";
      button_dash << name + " " + std::to_string(lev + 1) << "<br />";

    }

  }


};

Interface interface;

int main()
{
  return 0;
}
