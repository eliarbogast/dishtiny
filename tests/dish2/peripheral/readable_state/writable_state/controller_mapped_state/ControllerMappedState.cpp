#define CATCH_CONFIG_MAIN

#include "Catch/single_include/catch2/catch.hpp"

#include "dish2/peripheral/readable_state/writable_state/controller_mapped_state/ControllerMappedState.hpp"

TEST_CASE("Test ControllerMappedState") {
  dish2::ControllerMappedState{};
}