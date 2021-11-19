#pragma once
#include "chemist/set_theory/traits/set_traits.hpp"
#include <memory>
#include <tuple>

namespace chemist::set_theory {
template<typename SetType>
class Subset;

// TODO: The functions should probably become PIMPls

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
    /// Type of the supersets
    using parent_type = SetType;

    /// Type of a read-only reference to the superset
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

    /** @brief Wraps the process of dereferencing a pointer to the superset.
     *
     *  @param[in] parent An object of ptr_type holding the superset for the
     *                    FamilyOfSets.
     *  @return A read-only reference to the superset.
     */
    static const auto& dereference_object(ptr_type parent) { return *parent; }

    /** @brief Used to check if @p x is a subset of @p parent.
     *
     *  @param[in] x The set we are comparing to @p parent.
     *  @param[in] parent The set serving in the superset role.
     *
     *  @return True if @p x is a subst of @p parent and false otherwise.
     */
    static bool is_subset(const_reference x, ptr_type parent) {
        return x.object() == *parent;
    }

    /** @brief Determines if two sets in the family of sets are disjoint.
     *
     *  @param[in] lhs The set on the left of operator^.
     *  @param[in] rhs The set on the right of operator^.
     *
     *  @return True if @p lhs and @p rhs are disjoint. False otherwise.
     */
    static bool disjoint(const value_type& lhs, const value_type& rhs) {
        return (lhs ^ rhs).empty();
    }

    /** @brief Wraps the process of printing an element of the FoS.
     *
     *  @param[in] os The stream to print the element to.
     *  @param[in] elem The subset to print to @p os.
     *
     *  @return @p os after adding @p elem to it.
     */
    static std::ostream& print_elem(std::ostream& os, const value_type& elem) {
        return os << elem;
    }
};

} // namespace chemist::set_theory