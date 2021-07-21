#pragma once
#include "libchemist/wavefunction/wavefunction_fwd.hpp"

namespace libchemist::wavefunction {

namespace detail_ {

template<typename T>
struct IsLocal : std::false_type {};

template<>
struct IsLocal<LocalReference> : std::true_type {};

template<>
struct IsLocal<CanonicalLocalReference> : std::true_type {};

} // namespace detail_

template<typename T>
constexpr bool is_local_v = detail_::IsLocal<T>::value;

} // namespace libchemist::wavefunction
