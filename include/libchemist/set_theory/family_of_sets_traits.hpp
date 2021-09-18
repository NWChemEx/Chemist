#pragma once
#include "libchemist/set_theory/set_traits.hpp"
#include <memory>
#include <tuple>

namespace libchemist::set_theory {
template<typename SetType>
class Subset;

/** @brief Given a set type determines the types for a FamilyOfSets which wraps
 *         the set.
 *
 *  Given a set type @p SetType this class will contain a series of types and
 *  static factory functions which the FamilyOfSets will use to implement
 *  itself. By specializing this class it should be possible to get FamilyOfSets
 *  to work with almost any container. The primary template should work for
 *  most containers. All specializations must minimally have the same public
 *  API.
 *
 *  @tparam SetType The type of the container the FamilyOfSets will wrap.
 *
 */
template<typename SetType>
struct FamilyOfSetsTraits {
private:
    /// Type that the subset uses for offsets and indices
    using subset_size_type = typename SetTraits<SetType>::size_type;

public:
    using parent_type = SetType;

    using const_parent_reference = const parent_type&;

    /// Type of the elements in a FamilyOfSets
    using value_type = Subset<SetType>;

    /// Type of a modifiable reference
    using reference_type = value_type&;

    /// Type of a read-only reference
    using const_reference = const value_type&;

    /// The inner type of the initializer list, will map to a subset
    using subset_il_type = std::initializer_list<subset_size_type>;

    /** @brief Type of an initializer list which uses offsets to elements
     *
     *  You can think of family of sets as a list of lists so the structure of
     *  the initializer list should be akin to:
     *
     *  @code
     *  il_type il{{0, 1, 2}, {1, 2, 3}};
     *  @endcode
     *
     *  This example initializer list declares two sets. The first set contains
     *  elements 0, 1, and 2. The second set contains elements 1, 2, and 3.
     */
    using il_type = std::initializer_list<subset_il_type>;

    /// Type of the pointer which contains the parent set
    using ptr_type = std::shared_ptr<const SetType>;

    /** @brief Turns a parent set instance into a pointer to a parent set.
     *
     *  For most containers turning a parent set into a pointer is
     *  straightforward; however, for some types, such as tuples, there are a
     *  couple of container-specific steps. This function can be used to hide
     *  those steps.
     *
     *  @param[in] set The set the FamilyOfSets will wrap.
     *
     *  @return An instance of `ptr_type` containing @p set.
     */
    static ptr_type make_pointer(SetType set) {
        return std::make_shared<SetType>(std::move(set));
    }

    /** @brief Creates a new subset of the provided parent set.
     *
     *  This function wraps the process of creating a new subset.
     *
     *  @param[in] parent A pointer to the superset of the subset.
     *  @param[in] il The values to initialize the subset with.
     *
     *  @return The newly created subset.
     */
    static value_type new_subset(ptr_type parent, subset_il_type il = {}) {
        return value_type(parent, il);
    }

    static const auto& dereference_object(ptr_type parent) { return *parent; }

    static bool is_subset(const_reference x, ptr_type parent) {
        return x.object() == *parent;
    }
};

/** @brief Specializes FamilyOfSets to a tuple of sets.
 *
 *  Having tuples of parent sets is convenient when one wants to associate
 *  subsets with each other. For example, using a tuple of an AO basis set and
 *  a molecule allows us to have subsets which contain an AO basis set piece and
 *  a molecule piece. This template specialization enables FamilyOfSets to work
 *  with tuples of containers.
 *
 *  @tparam SetType A template parameter pack containg the types of the parent
 *                  set containers.
 */
template<typename... SetType>
struct FamilyOfSetsTraits<std::tuple<SetType...>> {
private:
    /// Tuple containing the types each container uses for offsets and indices
    template<typename T>
    using subset_size_type = typename SetTraits<T>::size_type;

    /// The number of parent sets in the FamilyOfSets
    static constexpr std::size_t N = sizeof...(SetType);

    template<typename T>
    using offset_il = std::initializer_list<subset_size_type<T>>;

public:
    using parent_set_type = std::tuple<SetType...>;

    using const_parent_reference =
      std::tuple<std::reference_wrapper<const SetType>...>;

    using value_type = std::tuple<Subset<SetType>...>;

    using reference_type = value_type&;

    using const_reference = const value_type&;

    using subset_il_type = std::tuple<offset_il<SetType>...>;

    using il_type = std::initializer_list<subset_il_type>;

    using ptr_type = std::tuple<std::shared_ptr<const SetType>...>;

    /** @brief Wraps the logic for making the shared_ptr to the parent set.
     *
     */
    static auto make_pointer(parent_set_type pset) {
        return make_pointer_(std::move(pset), std::make_index_sequence<N>());
    }

    static value_type new_subset(ptr_type parent, subset_il_type il) {
        return new_subset_(parent, il, std::make_index_sequence<N>());
    }

    static auto dereference_object(const ptr_type& parent) {
        return dereference_object_(parent, std::make_index_sequence<N>());
    }

    static bool is_subset(value_type elem, ptr_type parent) {
        return is_subset_<0>(elem, parent);
    }

private:
    template<std::size_t... Is>
    static auto new_subset_(ptr_type parent, subset_il_type il,
                            std::index_sequence<Is...>);

    /** @brief Implements make_pointer by expanding the provided index_sequence
     *
     *  @tparam Is template non-type parameters to be used as indices for
     *             manipulating the tuple. @p Is should contain the values
     *             [0, N).
     */
    template<std::size_t... Is>
    static auto make_pointer_(parent_set_type pset, std::index_sequence<Is...>);

    template<std::size_t... Is>
    static auto dereference_object_(const ptr_type& parent,
                                    std::index_sequence<Is...>);

    template<std::size_t I>
    static bool is_subset_(value_type elem, ptr_type parent);
}; // namespace libchemist::set_theory

template<typename... SetTypes>
template<std::size_t... Is>
auto FamilyOfSetsTraits<std::tuple<SetTypes...>>::new_subset_(
  ptr_type ptr, subset_il_type il, std::index_sequence<Is...>) {
    return value_type{Subset<std::tuple_element_t<Is, parent_set_type>>(
      std::get<Is>(ptr), std::get<Is>(il))...};
}

template<typename... SetTypes>
template<std::size_t... Is>
auto FamilyOfSetsTraits<std::tuple<SetTypes...>>::make_pointer_(
  parent_set_type pset, std::index_sequence<Is...>) {
    return ptr_type{std::make_shared<std::tuple_element_t<Is, parent_set_type>>(
      std::move(std::get<Is>(pset)))...};
}

template<typename... SetTypes>
template<std::size_t... Is>
auto FamilyOfSetsTraits<std::tuple<SetTypes...>>::dereference_object_(
  const ptr_type& ptr, std::index_sequence<Is...>) {
    return std::make_tuple(std::cref(*(std::get<Is>(ptr).get()))...);
}

template<typename... SetTypes>
template<std::size_t I>
bool FamilyOfSetsTraits<std::tuple<SetTypes...>>::is_subset_(value_type elem,
                                                             ptr_type parent) {
    if constexpr(sizeof...(SetTypes) == I) {
        return true;
    } else {
        if(std::get<I>(elem).object() != *std::get<I>(parent)) return false;
        return is_subset_<I + 1>(elem, parent);
    }
}

} // namespace libchemist::set_theory