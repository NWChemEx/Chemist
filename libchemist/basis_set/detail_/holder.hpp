#pragma once
#include <optional>
#include <type_traits>
namespace libchemist::basis_set::detail_ {

template<typename U>
using is_pointer = typename std::is_pointer<std::decay_t<U>>::type;

template<typename U>
using holder = std::conditional_t<is_pointer<U>::value, U, std::optional<U>>;

} // namespace libchemist::basis_set::detail_
