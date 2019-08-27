#pragma once

#include <limits>

#include "config/config.h"

EMP_BUILD_CONFIG(
  ConfigLevel,
  GROUP(CORE, "Core settings for a DISHTINY level"),
  VALUE(SIGNAL_RADIUS, int, 9/*std::numeric_limits<int>::max()*/, "How far can a signal propagate?"),
  VALUE(EVENT_RADIUS, int, 4, "How far from the seed are harvests beneficial?"),
  VALUE(HARVEST_VALUE, double, 5.0, "How much resource benefit does a successful harvest grant?"),
  VALUE(ACTIVATION_COST, double, -4.0, "How much resource is spent to attempt a harvest?")
)
