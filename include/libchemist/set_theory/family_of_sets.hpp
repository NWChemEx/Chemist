#pragma once
#include "libchemist/set_theory/subset.hpp"
#include <vector>

namespace libchemist::set_theory {

/** @brief Holds a series of Subset objects.
 *
 *  Mathematically a "family of sets" @f$\mathbb{F}@f$ is a collections of
 *  subsets of a set @f$\mathbb{S}@f$. A family of sets is a generalization of a
 *  partitioning of a set (a partitioning of a set is a family of sets where
 *  every element of the set appears in one and only one subset).
 *
 *  The FamilyOfSets class is a container which holds the subsets of a parent
 *  set. The parent set is owned by the FamilyOfSets instance to ensure that
 *  references to the parent set's elements remain valid. The subsets are
 *  lightweight views of the parent set which avoid copying elements of the
 *  parent set while still providing object-oriented access semantics to the
 *  elements.
 *
 *  @tparam SetType The type of the parent set. @p SetType must be copy and/or
 *                  move constructable. Additionally `SetTraits<SetType>` must
 *                  be valid either via the primary template definition or by
 *                  specialization.
 */
template<typename SetType>
class FamilyOfSets {
public:
    /// Type of the subsets in this family of sets
    using value_type = Subset<SetType>;

    /// Type of a read/write-reference to a subset
    using reference_type = value_type&;

    /// Type of a read-only reference to a subset
    using const_reference = const value_type&;

    /** @brief Creates a new FamilyOfSets instance using @p obj as the parent
     *         set.
     *
     *  @param[in] obj The parent set from which all of the subsets will be
     *                 taken.
     */
    explicit FamilyOfSets(SetType obj);

    // bool disjoint() const noexcept;
    reference_type new_subset();

    const auto& object() const { return m_obj_; }

private:
    std::vector<value_type> m_subsets_;
    SetType m_obj_;
};

#define FAMILYOFSETS FamilyOfSets<SetType>

template<typename SetType>
FAMILYOFSETS::FamilyOfSets(SetType obj) : m_obj_(std::move(obj)) {}

template<typename SetType>
typename FAMILYOFSETS::reference_type FAMILYOFSETS::new_subset() {
    m_subsets_.push_back(value_type(*this));
    return m_subsets_.back();
}

#undef FAMILYOFSETS
} // namespace libchemist::set_theory