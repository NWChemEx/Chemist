#pragma once
#include <optional>
#include <type_traits>
namespace libchemist::basis_set::detail_ {
/// Struct with a static member `value` whose value is true if @p U is a pointer
template<typename U>
using is_pointer = typename std::is_pointer<std::decay_t<U>>::type;

/// Type to use for holding a member in the basis set class hierarchy
template<typename U>
using holder = std::conditional_t<is_pointer<U>::value, U, std::optional<U>>;

} // namespace libchemist::basis_set::detail_
