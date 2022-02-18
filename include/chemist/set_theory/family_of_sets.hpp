#pragma once
#include "chemist/detail_/hashing.hpp"
#include "chemist/set_theory/subset.hpp"
#include "chemist/set_theory/traits/traits.hpp"
#include <vector>

/// TODO: Roll our own reference wrapper with these operations defined
namespace std {

template<typename T0>
bool operator==(const reference_wrapper<T0>& lhs, const std::decay_t<T0>& rhs) {
    return lhs.get() == rhs;
}

} // namespace std

namespace chemist::set_theory {

/** @brief Holds a series of Subset objects.
 *
 *  Mathematically a "family of sets" @f$\mathbb{F}@f$ is a collections of
 *  subsets of a set @f$\mathbb{S}@f$. A family of sets is a generalization of a
 *  partitioning of a set (a partitioning of a set is a family of sets where
 *  every element of the set appears in one and only one subset).
 *
 *  The FamilyOfSets class is a container which holds the subsets of a parent
 *  set. The FamilyOfSets, and its subsets, share ownership of the parent set.
 *  This means the subsets remain valid even if the originating FamilyOfSets
 *  instance goes out of scope. The subsets are lightweight views of the parent
 *  set which avoid copying elements of the parent set while still providing
 *  object-oriented access semantics to the elements.
 *
 *  @tparam SetType The type of the parent set. @p SetType must be copy and/or
 *                  move constructable. Additionally `SetTraits<SetType>` must
 *                  be valid either via the primary template definition or by
 *                  specialization.
 */
template<typename SetType>
class FamilyOfSets {
public:
    using traits_type = FamilyOfSetsTraits<SetType>;

    /// Type of the set we are dividing up
    using superset_type = SetType;

    /// Type of a read-only reference to the superset
    using const_superset_reference =
      typename traits_type::const_parent_reference;

    /// Type of the subsets in this family of sets
    using value_type = typename traits_type::value_type;

    /// Type of a read/write-reference to a subset
    using reference_type = typename traits_type::reference_type;

    /// Type of a read-only reference to a subset
    using const_reference = typename traits_type::const_reference;

    /// Type of the pointer holding the superset
    using ptr_type = typename traits_type::ptr_type;

private:
    /// Type of the container holding the subsets
    using subset_container = boost::container::flat_set<value_type>;

public:
    /// Type used for indexing and offsets
    using size_type = typename subset_container::size_type;

    /// Type of the initializer list expected by the subset ctor
    using subset_il_type = typename traits_type::subset_il_type;

    /** @brief Type of an initializer list which can be used for initialization
     *         by offset.
     */
    using il_type = typename traits_type::il_type;

    /** @brief Creates an empty FoS w/o a superset.
     *
     *  This ctor creates an empty FoS. The FoS also has no superset, and thus
     *  is not usable. To make the defaulted FoS instance usable you can assign
     *  a non-defaulted FoS instance to it.
     */
    FamilyOfSets() = default;

    /** @brief Creates a new FamilyOfSets instance using @p obj as the parent
     *         set.
     *
     *  @param[in] obj The parent set from which all of the subsets will be
     *                 taken.
     *  @param[in] il An initializer list containing the subsets to populate the
     *                instance with. Default value is an empty list, which will
     *                create an empty FamilyOfSets.
     */
    explicit FamilyOfSets(SetType obj, il_type il = {});

    /** @brief Used to determine if the current family is empty.
     *
     *  This family of sets is empty if it contains zero sets. This function is
     *  therefore a convenience function for checking if `size() == 0`.
     *
     *  @return True if this family of sets is empty and false otherwise.
     *
     *  @throw None no throw guarantee.
     */
    bool empty() const noexcept { return m_subsets_.empty(); }

    /** @brief Returns the number of subsets in this family of sets.
     *
     *  This function is used to count the number of subsets in this family of
     *  sets.
     *
     *  @return The number of subsets in this family.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_subsets_.size(); }

    /** @brief Returns the parent set.
     *
     *  This function returns the set from which all of the subsets are taken.
     *
     *  @return The parent set.
     *
     *  @throw None No throw guarantee.
     */
    const_superset_reference object() const noexcept;

    /** @brief Returns a shared pointer to the superset.
     *
     *  The superset is stored in a shared pointer, a copy of which lives in
     *  this FamilyOfSets and in each Subset object. This function allows you
     *  to retrieve the shared_ptr, primarily for creating new Subsets.
     *
     *  @return A shared pointer to the superset.
     *
     *  @throw None no throw guarantee.
     */
    ptr_type data() const noexcept { return m_obj_; }

    /** @brief Convenience function for creating a new subset.
     *
     *  This function will create a new subset of the family of sets and return
     *  it. The user can then fill in the subset. The subset which is returned
     *  has **NOT** been added to the family of sets yet; adding it to family
     *  of sets can be accomplised by calling insert.
     *
     *  @return An empty subset, which the user can fill in.
     *
     *  @throw None No throw guarantee. (TODO: Check this.)
     */
    auto new_subset() const { return traits_type::new_subset(data()); }

    /** @brief Returns the i-th subset in the family.
     *
     *  This function can be used to retrieve a subset by offset. No bounds
     *  check is performed so this function should be used when you know that
     *  the index is in bounds. If you are not sure if @p i is in the range
     *  [0, size()) use `at`.
     *
     *  @param[in] i the offset of the requested subset. Must be in the range
     *               [0, size()). Providing an @p i not in range is undefined
     *               behavior.
     *
     *  @return A read-only reference to the requested subset.
     *
     *  @throw None No throw guarantee.
     */
    const_reference operator[](size_type i) const noexcept;

    /** @brief Convience function for creating an empty subset.
     *
     *  This function wraps the construction of a new subset. If this instance
     *  already contains the subset this is a no-op.
     *
     *  @tparam ElemType The type of the elements in the initializer list.
     *                   Expected to be implicitly convertible to either
     *                   size_type or the type of the elements in the parent
     *                   set.
     *
     *  @param[in] il An initializer list of either the offsets for the subset's
     *                values or the values themselves.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory for
     *                        the new set. Strong throw guarantee.
     */
    void emplace(subset_il_type il);

    /** @brief Returns the i-th subset in the family.
     *
     *  This function can be used to retrieve a subset by offset. A bounds
     *  check is performed to ensure that @p i is in the range [0, size()). If
     *  you know that @p i is in range use `operator[]`.
     *
     *  @param[in] i the offset of the requested subset. Must be in the range
     *               [0, size()).
     *
     *  @return A read-only reference to the requested subset.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *                           throw guarantee.
     */
    const_reference at(size_type i) const;

    /** @brief Returns an iterator to the first subset.
     *
     *  @return An iterator pointing to the first subset in the family.
     *
     *  @throw None no throw guarantee.
     */
    auto begin() const noexcept { return m_subsets_.begin(); }

    /** @brief Returns an iterator to just past the last subset in the family.
     *
     *  @return An iterator pointing to just past the last subset in the family.
     *
     *  @throw None No throw guarantee.
     */
    auto end() const noexcept { return m_subsets_.end(); }

    /** @brief Inserts a new subset into the family of subsets.
     *
     *  This function is used to insert a new subset into the family of sets. If
     *  the family already contains @p elem nothing happens.
     *
     *  @param[in] elem The subset to insert.
     *
     *  @throw std::runtime_error if the Subset is not defined in terms of the
     *                            same superset. Strong throw guarantee.
     */
    void insert(value_type elem);

    /** @brief Determines if all of the subsets are disjoint.
     *
     *  Two sets are disjoint if their intersection is empty. This function will
     *  determine if all of the sets in this family are disjoint with one
     *  another.
     *
     *  @return True if all pair-wise intersections are empty and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool disjoint() const noexcept;

    /** @brief Hashes the FamilyOfSets.
     *
     *  @param[in,out] h The Hasher instance to use for hashing the
     *                   FamilyOfSets. After this call the internal hash of @p h
     *                   will be updated to include the hash of this instance.
     */
    void hash(chemist::detail_::Hasher& h) const;

private:
    /// Checks that @p i is a valid offset
    void bounds_check_(size_type i) const;

    /// The actual subsets
    subset_container m_subsets_;

    /// The parent set, i.e. the superset
    ptr_type m_obj_;
};

/** @brief Compares two FamilyOfSet instances for equality.
 *
 *  @relates FamilyOfSets
 *
 *  Two family of sets are equal if they have the same superset, contain the
 *  same number of subsets, and `lhs[i] == rhs[i]` for all i.
 *
 *  @param[in] lhs The FamilyOfSets on the left of the equality.
 *  @param[in] rhs The FamilyOfSets on the right of the equality.
 *
 *  @return True if the families are equal and false otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSSetType, typename RHSSetType>
bool operator==(const FamilyOfSets<LHSSetType>& lhs,
                const FamilyOfSets<RHSSetType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>) {
        return false;
    } else {
        if(lhs.object() != rhs.object()) return false;
        if(lhs.size() != rhs.size()) return false;
        using size_type = typename FamilyOfSets<LHSSetType>::size_type;
        for(size_type i = 0; i < lhs.size(); ++i)
            if(lhs[i] != rhs[i]) return false;
        return true;
    }
}

/** @brief Determines if two FamilyOfSet instances are different
 *
 *  Two family of sets are equal if they have the same superset, contain the
 *  same number of subsets, and `lhs[i] == rhs[i]` for all i.
 *
 *  @param[in] lhs The FamilyOfSets on the left of the not equal operator.
 *  @param[in] rhs The FamilyOfSets on the right of the not equal operator.
 *
 *  @return False if the families are equal and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSSetType, typename RHSSetType>
bool operator!=(const FamilyOfSets<LHSSetType>& lhs,
                const FamilyOfSets<RHSSetType>& rhs) {
    return !(lhs == rhs);
}

template<typename SetType>
std::ostream& operator<<(std::ostream& os, const FamilyOfSets<SetType>& fos) {
    os << "[";
    bool first = true;
    for(const auto& set_i : fos) {
        if(!first) os << ",";
        FamilyOfSetsTraits<SetType>::print_elem(os, set_i);
        first = false;
    }
    return os << "]";
}

// -------------------------- Implementations ---------------------------------
#define FAMILYOFSETS FamilyOfSets<SetType>

template<typename SetType>
FAMILYOFSETS::FamilyOfSets(SetType obj, il_type il) :
  m_obj_(traits_type::make_pointer(std::move(obj))) {
    for(auto x : il) emplace(x);
}

template<typename SetType>
void FAMILYOFSETS::emplace(subset_il_type il) {
    insert(traits_type::new_subset(data(), il));
}

template<typename SetType>
typename FAMILYOFSETS::const_reference FAMILYOFSETS::operator[](
  size_type i) const noexcept {
    return *(m_subsets_.begin() + i);
}

template<typename SetType>
typename FAMILYOFSETS::const_reference FAMILYOFSETS::at(size_type i) const {
    bounds_check_(i);
    return (*this)[i];
}

template<typename SetType>
typename FAMILYOFSETS::const_superset_reference FAMILYOFSETS::object()
  const noexcept {
    return traits_type::dereference_object(m_obj_);
}

template<typename SetType>
void FAMILYOFSETS::insert(value_type elem) {
    if(!traits_type::is_subset(elem, data()))
        throw std::runtime_error("Subset is not part of this family");

    m_subsets_.emplace(std::move(elem));
}

template<typename SetType>
bool FAMILYOFSETS::disjoint() const noexcept {
    auto e = end();
    for(auto lhs = begin(); lhs != e; ++lhs) {
        auto rhs = lhs;
        ++rhs;
        for(; rhs != e; ++rhs) {
            if(!traits_type::disjoint(*lhs, *rhs)) return false;
        }
    }
    return true;
}

template<typename SetType>
void FAMILYOFSETS::hash(chemist::detail_::Hasher& h) const {
    for(const auto& x : m_subsets_) h(x);
    h(m_obj_);
}

template<typename SetType>
void FAMILYOFSETS::bounds_check_(size_type i) const {
    const auto n = size();
    if(i < n) return;
    throw std::out_of_range("Index " + std::to_string(i) +
                            " is not in the "
                            "range [0, " +
                            std::to_string(n) + ").");
}

#undef FAMILYOFSETS
} // namespace chemist::set_theory
