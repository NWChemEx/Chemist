#pragma once
#include <type_traits> // for std::true_type, std::false_type

namespace chemist::orbital_space {

/// Forward declare for TMP purposes
template<typename SpaceType>
class IndependentSpace;

namespace detail_ {

/** @brief Primary template for determining if a type is an IndependentSpace
 *
 *  @tparam T The type whose IndependentSpace-ness is in question.
 *
 */
template<typename T>
struct IsIndependentSpace : std::false_type {};

template<typename T>
struct IsIndependentSpace<IndependentSpace<T>> : std::true_type {};

} // namespace detail_

template<typename T>
static constexpr auto is_independent_space_v =
  detail_::IsIndependentSpace<T>::value;

} // namespace chemist::orbital_space
