#pragma once
#include "chemist/set_theory/traits/family_of_sets.hpp"

namespace chemist::set_theory {

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
    /// Tuple containing the types each container uses for offsets and
    /// indices
    template<typename T>
    using subset_size_type = typename SetTraits<T>::size_type;

    /// The number of parent sets in the FamilyOfSets
    static constexpr std::size_t N = sizeof...(SetType);

    /// The initializer list for a subset of a superset of type T
    template<typename T>
    using offset_il = std::initializer_list<subset_size_type<T>>;

public:
    /// Type of the supersets
    using parent_set_type = std::tuple<SetType...>;

    /// Type of a read-only reference to the supersets
    using const_parent_reference =
      std::tuple<std::reference_wrapper<const SetType>...>;

    /// Type of the subsets
    using value_type = std::tuple<Subset<SetType>...>;

    /// Type of a read/write reference to a subset
    using reference_type = value_type&;

    /// Type of a read-only reference to a subset
    using const_reference = const value_type&;

    /// Type of an initializer list for a subset
    using subset_il_type = std::tuple<offset_il<SetType>...>;

    /// Type of an initializer list for the FamilyOfSets
    using il_type = std::initializer_list<subset_il_type>;

    /// Type of a pointer to the superset
    using ptr_type = std::tuple<std::shared_ptr<const SetType>...>;

    /** @brief Wraps the logic for making the shared_ptr to the parent set.
     *
     *  @param[in] pset The superset we are wrapping in a pointr.
     *
     *  @return An object of type ptr_type containing the superset
     */
    static auto make_pointer(parent_set_type pset) {
        return make_pointer_(std::move(pset), std::make_index_sequence<N>());
    }

    /** @brief Creates a new subset for the provided superset.
     *
     *  @param[in] parent The superset for the subset.
     *  @param[in] il The members to initialize the subset with.
     *
     *  @return The newly create subset.
     */
    static value_type new_subset(ptr_type parent, subset_il_type il = {}) {
        return new_subset_(parent, il, std::make_index_sequence<N>());
    }

    /** @brief Returns a reference to provided superset.
     *
     *  For a tuple we actually get back a tuple of references. This function
     *  wraps the process of making that tuple.
     *
     *  @param[in] parent The superset we are dereferencing.
     *
     *  @return An object of type parent_set_reference which is a reference to
     *          the provided superset.
     */
    static auto dereference_object(const ptr_type& parent) {
        return dereference_object_(parent, std::make_index_sequence<N>());
    }

    /** @brief Determines if the provided set is a subset of @p parent.
     *
     *  @param[in] elem The set we want to know the subset-ness of.
     *  @param[in] parent The set we are assuming might be a superset.
     *
     *  @return True if @p elem is a subset of @p parent and false otherwise.
     */
    static bool is_subset(value_type elem, ptr_type parent) {
        return is_subset_<0>(elem, parent);
    }

    /** @brief Determines if two subsets are disjoint.
     *
     *  @param[in] lhs The first of the two subsets.
     *  @param[in] rhs The second of the two subsets.
     *
     *  @return True if @p lhs and @p rhs are disjoint and false otherwise.
     */
    static bool disjoint(const value_type& lhs, const value_type& rhs) {
        return disjoint_<0>(lhs, rhs);
    }

    /** @brief Wraps the process of printing an element of the FoS.
     *
     *  @param[in] os The stream to print the element to.
     *  @param[in] elem The subset to print to @p os.
     *
     *  @return @p os after adding @p elem to it.
     */
    static std::ostream& print_elem(std::ostream& os, const value_type& elem) {
        return print_elem_<0>(os, elem);
    }

private:
    /** @brief Implements new_subset
     *
     *  @tparam Is A template non-type parameter pack containing the indices
     *             of the tuple.
     *
     *  @param[in] parent The superset we are making a new subset for.
     *  @param[in] il     An initializer list to populate the subset with.
     *  @param[in] <anonymous> An object used purely for its dependence on @p Is
     *
     *  @return The newly created subset.
     */
    template<std::size_t... Is>
    static auto new_subset_(ptr_type parent, subset_il_type il,
                            std::index_sequence<Is...>);

    /** @brief Implements make_pointer by expanding the provided
     * index_sequence
     *
     *  @tparam Is template non-type parameters to be used as indices for
     *             manipulating the tuple. @p Is should contain the values
     *             [0, N).
     *
     *  @param[in] pset The superset we are wrapping.
     *  @param[in] <anonymous> An object used purely for its dependence on @p Is
     *
     *  @return A pointer wrappinng @p pset.
     */
    template<std::size_t... Is>
    static auto make_pointer_(parent_set_type pset, std::index_sequence<Is...>);

    /** @brief Implements dereference_object
     *
     *  @tparam Is A template non-type parameter pack containing the indices of
     *             the tuple.
     *
     *  @param[in] parent The superset we are dereferencing.
     *  @param[in] <anonymous> An object used purely for its dependence on @p Is
     *
     *  @return A read-only reference to the superset.
     */
    template<std::size_t... Is>
    static auto dereference_object_(const ptr_type& parent,
                                    std::index_sequence<Is...>);

    /** @brief Implements is_subset.
     *
     *  @tparam I The tuple index to compare at this level of recursion.
     *
     *  @param[in] elem The set whose subset-ness is in question.
     *  @param[in] parent The set which may be the superset of @p elem.
     *
     *  @return True if all elements of @p elem with an index greater than equal
     *          to I are subsets of @p parent. False otherwise.
     */
    template<std::size_t I>
    static bool is_subset_(value_type elem, ptr_type parent);

    /** @brief Implements disjoint
     *
     *  @tparam I The tuple index to compare at this level of recursion.
     *
     *  @param[in] lhs The first subset to compare.
     *  @param[in] rhs The second subset to compare.
     *
     *  @return True if @p lhs and @p rhs are disjoint in all tuple elemenst J
     *          such that J >= I. False otherwise.
     */
    template<std::size_t I>
    static bool disjoint_(const value_type& lhs, const value_type& rhs);

    /** @brief Implements print_elem
     *
     *  @tparam I The index of the tuple being printed at this level of
     *            recursion.
     *
     *  @param[in] os The stream we are printing @p elem to.
     *  @param[in] elem The subset of the FoS being printed to @p os.
     *
     *  @return @p os after adding the [I, sizeof(SetType)) tuple elements to
     *                it.
     */
    template<std::size_t I>
    static std::ostream& print_elem_(std::ostream& os, const value_type& elem);
}; // namespace chemist::set_theory

//------------------------------- Implementations ------------------------------

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

template<typename... SetTypes>
template<std::size_t I>
bool FamilyOfSetsTraits<std::tuple<SetTypes...>>::disjoint_(
  const value_type& lhs, const value_type& rhs) {
    if constexpr(I == sizeof...(SetTypes)) {
        return true;
    } else {
        if(!(std::get<I>(lhs) ^ std::get<I>(rhs)).empty()) return false;
        return disjoint_<I + 1>(lhs, rhs);
    }
}

template<typename... SetTypes>
template<std::size_t I>
std::ostream& FamilyOfSetsTraits<std::tuple<SetTypes...>>::print_elem_(
  std::ostream& os, const value_type& elem) {
    if constexpr(I == 0) { os << "("; }
    if constexpr(I == sizeof...(SetTypes)) {
        return os << ")";
    } else {
        if constexpr(I != 0) { os << ","; }
        os << std::get<I>(elem);
        return print_elem_<I + 1>(os, elem);
    }
}

} // namespace chemist::set_theory