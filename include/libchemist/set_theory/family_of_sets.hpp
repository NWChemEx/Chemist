#pragma once
#include "libchemist/set_theory/subset.hpp"

namespace libchemist::set_theory {

/** @brief Holds a series of Subset objects.
 *
 *  Mathematically a "family of sets" @f$\mathbb{F}@f$ is a collections of
 *  subsets of a set @f$\mathbb{S}@f$. A family of sets is a generalization of a
 *  partitioning of a set (a partitioning of a set is a family of sets where
 *  every element of the set appears in one and only one subset).
 *
 *  @tparam SetType The type of the parent set. @p SetType must be copy and/or
 *                  move constructable. Additionally `SetTraits<SetType>` must
 *                  be valid either via the primary template definition or by
 *                  specialization.
 */
template<typename SetType>
class FamilyOfSets {
public:
    /// Type of the subsets in this FamilyOfSets
    using value_type = Subset<SetType>;

    using reference_type  = value_type&;
    using const_reference = const value_type&;

    explicit FamilyOfSets(SetType obj);

    bool disjoint() const noexcept;
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