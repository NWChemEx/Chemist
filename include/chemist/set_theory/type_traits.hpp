#pragma once
#include <type_traits>

namespace chemist::set_theory {

// Forward declare FamilyOfSets for TMP purposes
template<typename T>
class FamilyOfSets;

namespace detail_ {

/** @brief Primary template for determining if type @p T is a FamilyOfSets.
 *
 *  The primary template is chosen whenever @p T is not an instantiation of
 *  `FamilyOfSets`. When instantiated the primary template of IsFamilyOfSets
 *  contains a static constexpr member `value` set to false.
 *
 *  @tparam T The type for this trait to determinine whether or not it is an
 *            instance of `type::FamilyOfSets`.
 *
 */
template<typename T>
struct IsFamilyOfSets : std::false_type {};

/** @brief Specialization of IsFamilyOfSets for when type @p T is a
 *         FamilyOfSets.
 *
 *  This specialization is chosen whenever @p T is an instantiation of
 *  `type::FamilyOfSets`. When instantiated this specialization of
 *  IsFamilyOfSets contains a static constexpr member `value` set to true.
 *
 *  @tparam T The type for this trait to determinine whether or not it is an
 *            instance of `type::FamilyOfSets`.
 */
template<typename T>
struct IsFamilyOfSets<set_theory::FamilyOfSets<T>> : std::true_type {};

} // namespace detail_

/** @brief Public API for determining if a type @p T is the type of a
 *         FamilyOfSets.
 *
 *  This type trait can be used to determine if @p T is an instantiation of
 *  FamilyOfSets. It will be set to true if @p T is an instantiation of
 *  FamilyOfSets and false otherwise.
 *
 *  @tparam T This trait will determine if @p T is a FamilyOfSets.
 */
template<typename T>
static constexpr bool is_family_of_sets_v = detail_::IsFamilyOfSets<T>::value;

} // namespace chemist::set_theory
