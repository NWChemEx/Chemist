#pragma once
#include "libchemist/orbital_space/derived_space.hpp"
#include <type_traits>

namespace libchemist::orbital_space {

/** @brief Determines if @p T is or derives from IndDerivedSpace.
 *
 * Independent spaces are paired with dependent spaces. This trait can be used
 * to determine if @p T is the type of an orbital space upon which another
 * orbital space may depend. If @p T is an independent space this global
 * variable will be set to true, otherwise it is false.
 *
 *  @tparam T The type we are inspecting.
 */
template<typename T>
constexpr auto is_independent_v = std::is_base_of_v<IndDerivedSpace, T>;

} // namespace libchemist::orbital_space
