#define CATCH_CONFIG_MAIN

#include "Catch/single_include/catch2/catch.hpp"

#include "dish2/peripheral/readable_state/writable_state/NopState.hpp"
#include "dish2/spec/Spec.hpp"

TEST_CASE("Test NopState") {
  dish2::NopState<dish2::Spec>{};
}
