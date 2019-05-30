#pragma once
#include <type_traits>

namespace libchemist::basis_set::detail_ {

template<typename T>
using holder = std::conditional_t<std::is_pointer_v<T>, T, std::optional<T>>;

} // namespace libchemist::basis_set::detail_
