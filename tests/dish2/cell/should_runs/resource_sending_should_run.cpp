#define CATCH_CONFIG_MAIN

#include "Catch/single_include/catch2/catch.hpp"

#include "dish2/cell/should_runs/resource_sending_should_run.hpp"

TEST_CASE("Test resource_sending_should_run") {
  dish2::resource_sending_should_run(0, false);
}
