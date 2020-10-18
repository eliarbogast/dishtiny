#pragma once
#ifndef DISH2_PERIPHERAL_READABLE_STATE_WRITABLE_STATE_NOPSTATE_HPP_INCLUDE
#define DISH2_PERIPHERAL_READABLE_STATE_WRITABLE_STATE_NOPSTATE_HPP_INCLUDE

#include "../../../../../third-party/conduit/include/uitsl/datastructs/PodLeafNode.hpp"

namespace dish2 {

struct NopState : public uitsl::PodLeafNode<float> {
  // inherit constructors
  using parent_t = uitsl::PodLeafNode<float>;
  using parent_t::parent_t;
};

} // namespace dish2

#endif // #ifndef DISH2_PERIPHERAL_READABLE_STATE_WRITABLE_STATE_NOPSTATE_HPP_INCLUDE
