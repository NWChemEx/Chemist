#pragma once
#include <type_traits>

namespace libchemist::orbital_space {

/// Forward declaration for TMP purposes
class DependentSpace;

/** @brief Determines if @p T is or derives from DependentSpace.
 *
 *  Dependent spaces have different orbitals for different independent indices.
 *  This trait can be used to determine if @p T is the type of an orbital space
 *  which depends on another orbital space. If @p T is a dependent space this
 *  global variable will be set to true, otherwise it is false.
 *
 *  @tparam T The type we are inspecting.
 */
template<typename T>
constexpr auto is_dependent_v = std::is_base_of_v<DependentSpace, T>;

} // namespace libchemist::orbital_space
