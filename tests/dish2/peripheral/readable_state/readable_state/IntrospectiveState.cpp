#define CATCH_CONFIG_MAIN

#include "Catch/single_include/catch2/catch.hpp"

#include "dish2/peripheral/readable_state/IntrospectiveState.hpp"

TEST_CASE("Test IntrospectiveState") {
  dish2::IntrospectiveState{};
}
