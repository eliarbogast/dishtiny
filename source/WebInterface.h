#pragma once

#include <algorithm>
#include <experimental/optional>
#include <string>

#include "tools/math.h"

#include "Config.h"
#include "DishWorld.h"
#include "WebArtist.h"

namespace UI = emp::web;

class WebInterface : public UI::Animate {

  Config cfg;

  UI::Document button_dash;  //< Div that contains the button dashboard.

  DishWorld w;

  UI::Document channel_viewer;
  UI::Document wave_viewer;
  UI::Document stockpile_viewer;
  WebArtist<ChannelPack> channel;
  emp::vector<WebArtist<int>> wave;
  WebArtist<double> stockpile;

  bool render;

public:

  WebInterface()
    : button_dash("emp_button_dash")
    , w(cfg)
    , channel_viewer("channel_viewer")
    , wave_viewer("wave_viewer")
    , stockpile_viewer("stockpile_viewer")
    , channel(
      channel_viewer,
      [this](size_t i){
        return w.IsOccupied(i) ? std::experimental::make_optional(w.man->Channel(i).GetIDs()) : std::experimental::nullopt;
      },
      [](std::experimental::optional<ChannelPack> cp) {
        return cp ?
        emp::ColorHSV(emp::Mod((*cp)[1],360.0),emp::Mod((*cp)[0],0.6)+0.4,1.0)
        : emp::ColorRGB(0.0,0.0,0.0);
      },
      cfg
    ), stockpile(
      stockpile_viewer,
      [this](size_t i){
        return w.IsOccupied(i) ? std::experimental::make_optional(w.man->Stockpile(i).QueryResource()) : std::experimental::nullopt;
      },
      [this](std::experimental::optional<double> amt) {
        if (amt) {
          if (*amt > cfg.REP_THRESH()) return emp::ColorRGB(
              std::min(255.0,(*amt - cfg.REP_THRESH())*25),
              255,
              0
            );
          else if (*amt > 0) return emp::ColorRGB(0,0,255);
          else if (*amt == 0) return emp::ColorRGB(255,255,255);
          else return emp::ColorRGB(
              255,
              255-std::min(255.0,(*amt)*255.0/cfg.KILL_THRESH()),
              255-std::min(255.0,(*amt)*255.0/cfg.KILL_THRESH())
            );
        } else return emp::ColorRGB(0,0,0);
      },
      cfg
    ), render(true) {

    for (size_t l = 0; l < cfg.NLEV(); ++l) {
      wave.emplace_back(
          wave_viewer,
          [this, l](size_t i){
            return w.IsOccupied(i) ? std::experimental::make_optional(w.man->Wave(i,l).GetState()) : std::experimental::nullopt;
          },
          [this, l](std::experimental::optional<int> cp) {
            if (cp) {
              if (*cp > 0 && *cp < cfg.Lev(l).EVENT_RADIUS()) {
                return emp::ColorRGB(0,255,0);
              } else if (*cp > 0) {
                return emp::ColorRGB(255,0,0);
              } else if (*cp < 0) {
                return emp::ColorRGB(0,0,255);
              } else {
                return emp::ColorRGB(255,255,255);
              }
            } else {
              return emp::ColorRGB(0,0,0);
            }
          },
          cfg
      );
    }

    auto ud_text = button_dash.AddText("ud_text");
    ud_text << "Update: " << UI::Live(w.update);

    button_dash << UI::Button(
      [this](){ DoFrame(); },
      "Step"
    );
    button_dash << GetToggleButton("Animate");
    button_dash << UI::Button(
        [this](){ render = !render; },
        "Toggle Render"
      );

  }

  void DoFrame() {
    w.Update();
    Redraw();
  }

  void Redraw() {
    button_dash.Text("ud_text").Redraw();
    if (render) {
      channel.Redraw();
      stockpile.Redraw();
      for(auto &w : wave) w.Redraw();
    }
  }

};
