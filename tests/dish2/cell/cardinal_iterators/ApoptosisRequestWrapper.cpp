#define CATCH_CONFIG_MAIN

#include "Catch/single_include/catch2/catch.hpp"

#include "dish2/cell/cardinal_iterators/ApoptosisRequestWrapper.hpp"
#include "dish2/spec/Spec.hpp"

using Spec = dish2::Spec;

TEST_CASE("Test ApoptosisRequestWrapper") {
  dish2::ApoptosisRequestWrapper<Spec>{};
}
