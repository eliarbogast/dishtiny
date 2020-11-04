#pragma once
#ifndef DISH2_PERIPHERAL_READABLE_STATE_WRITABLE_STATE_WRITABLESTATE_HPP_INCLUDE
#define DISH2_PERIPHERAL_READABLE_STATE_WRITABLE_STATE_WRITABLESTATE_HPP_INCLUDE

#include <type_traits>

#include "../../../../../third-party/conduit/include/uitsl/datastructs/PodInternalNode.hpp"
#include "../../../../../third-party/conduit/include/uitsl/algorithm/clamp_cast.hpp"
#include "../../../../../third-party/conduit/include/uitsl/datastructs/PodLeafNode.hpp"
#include "../../../../../third-party/Empirical/source/base/macros.h"
#include "../../../../../third-party/signalgp-lite/include/sgpl/utility/ByteEnumeration.hpp"

#include "NopState.hpp"

#include "controller_mapped_state/ControllerMappedState.hpp"

namespace dish2 {

namespace internal {

  using writable_state_parent_t = uitsl::PodInternalNode<
    dish2::ControllerMappedState,
    dish2::NopState
  >;

}

struct WritableState : public internal::writable_state_parent_t {

  // https://stackoverflow.com/a/63046442
  template<size_t Templateify=0>
  void AddTo(const size_t idx, const float val) {

    #define DISH2_ADD_TO_STATE_CASE_PAYLOAD(N) \
      case N: \
        if constexpr ( N  < internal::writable_state_parent_t::GetSize() ) { \
          this->Get<N + Templateify>() += uitsl::clamp_cast< \
            std::decay_t< decltype( this->Get<N + Templateify>() ) > \
          >( val ); \
        } \
      break;

    emp_assert( idx < internal::writable_state_parent_t::GetSize() );
    static_assert( internal::writable_state_parent_t::GetSize() < 256 );

    switch ( idx ) {

      EMP_WRAP_EACH( DISH2_ADD_TO_STATE_CASE_PAYLOAD, SGPL_BYTE_ENUMERATION )

    }

  }

  // https://stackoverflow.com/a/63046442
  template<size_t Templateify=0>
  void Multiply(const size_t idx, const float val) {

    #define DISH2_MULTIPLY_STATE_CASE_PAYLOAD(N) \
      case N: \
        if constexpr ( N  < internal::writable_state_parent_t::GetSize() ) { \
          this->Get<N + Templateify>() *= uitsl::clamp_cast< \
            std::decay_t< decltype( this->Get<N + Templateify>() ) > \
          >( val ); \
        } \
      break;

    emp_assert( idx < internal::writable_state_parent_t::GetSize() );
    static_assert( internal::writable_state_parent_t::GetSize() < 256 );

    switch ( idx ) {

      EMP_WRAP_EACH( DISH2_MULTIPLY_STATE_CASE_PAYLOAD, SGPL_BYTE_ENUMERATION )

    }

  }

  // https://stackoverflow.com/a/63046442
  template<size_t Templateify=0>
  void Write(const size_t idx, const float val) {

    #define DISH2_WRITABLE_STATE_CASE_PAYLOAD(N) \
      case N: \
        if constexpr ( N  < internal::writable_state_parent_t::GetSize() ) { \
          this->Get<N + Templateify>() = uitsl::clamp_cast< \
            std::decay_t< decltype( this->Get<N + Templateify>() ) > \
          >( val ); \
        } \
      break;

    emp_assert( idx < internal::writable_state_parent_t::GetSize() );
    static_assert( internal::writable_state_parent_t::GetSize() < 256 );

    switch ( idx ) {

      EMP_WRAP_EACH( DISH2_WRITABLE_STATE_CASE_PAYLOAD, SGPL_BYTE_ENUMERATION )

    }

  }

};

} // namespace dish2

#endif // #ifndef DISH2_PERIPHERAL_READABLE_STATE_WRITABLE_STATE_WRITABLESTATE_HPP_INCLUDE
