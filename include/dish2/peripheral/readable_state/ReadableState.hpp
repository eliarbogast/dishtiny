#pragma once
#ifndef DISH2_PERIPHERAL_READABLE_STATE_READABLESTATE_HPP_INCLUDE
#define DISH2_PERIPHERAL_READABLE_STATE_READABLESTATE_HPP_INCLUDE

#include "../../../../third-party/conduit/include/uitsl/datastructs/PodInternalNode.hpp"
#include "../../../../third-party/conduit/include/uitsl/datastructs/PodLeafNode.hpp"
#include "../../../../third-party/Empirical/source/base/macros.h"
#include "../../../../third-party/signalgp-lite/include/sgpl/utility/ByteEnumeration.hpp"

#include "introspective_state/IntrospectiveState.hpp"
#include "writable_state/WritableState.hpp"

namespace dish2 {

namespace internal {

  using readable_state_parent_t = uitsl::PodInternalNode<
    dish2::IntrospectiveState,
    dish2::WritableState
  >;

}

struct ReadableState : public internal::readable_state_parent_t {

  // https://stackoverflow.com/a/63046442
  template<size_t Templateify=0>
  double Read(const size_t idx) const {

    #define DISH2_READABLE_STATE_CASE_PAYLOAD(N) \
      case N: \
        if constexpr ( N < internal::readable_state_parent_t::GetSize() ) { \
          return this->Get<N + Templateify>(); \
        } \
      break;

    emp_assert( idx < internal::readable_state_parent_t::GetSize() );
    static_assert( internal::readable_state_parent_t::GetSize() < 256 );

    switch ( idx ) {

      EMP_WRAP_EACH( DISH2_READABLE_STATE_CASE_PAYLOAD, SGPL_BYTE_ENUMERATION )

    }

    emp_assert( false );
    return 0;

  }

};

} // namespace dish2

#endif // #ifndef DISH2_PERIPHERAL_READABLE_STATE_READABLESTATE_HPP_INCLUDE