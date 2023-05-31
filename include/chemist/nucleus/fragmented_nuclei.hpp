#pragma once
#include <chemist/nucleus/nuclei.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

/** @brief Class describing super-/sub-set relationship among sets of `Nucleus`
 *         objects.
 *
 *  See
 * https://nwchemex-project.github.io/Chemist/developer/design/chemistry/fragmented_nuclei.html
 *  for design details.
 *
 */
class FragmentedNuclei
  : public utilities::IndexableContainerBase<FragmentedNuclei> {
public:
    /// *this holds fragments from an object of supersystem_type
    using supersystem_type = Nuclei;

    /// Type of a read-only reference to the supersystem
    using const_supersystem_reference = const supersystem_type&;

private:
    /// Type *this inherits from
    using base_type = utilities::IndexableContainerBase<FragmentedNuclei>;

    /// This is the type of the object used to implement *this
    using impl_type = set_theory::FamilyOfSets<supersystem_type>;

public:
    /// Type of a reference to a fragment returned by non-const *this, will be
    /// read-only.
    using reference = typename impl_type::const_reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename impl_type::const_reference;

    /// Type used for referring to nuclei indices, unsigned integral type
    using index_type = impl_type::size_type;

    /// Type used for referring to fragments by index, unsigned integral type
    using size_type = base_type::size_type;

    FragmentedNuclei() = default;

    explicit FragmentedNuclei(nuclei_type supersystem) :
      m_frags_(std::move(supersystem)) {}

    void add_fragment(std::initializer_list<size_type> il) {
        add_fragment(il.begin(), il.end());
    }

    template<typename Args...>
    void add_fragment(size_type index0, Args... other_indices) {
        constexpr auto n = sizeof(Args) + 1;
        std::array<size_type, n> indices{index0, other_indices....};
        add_fragment(indices.begin(), indices.end());
    }

    template<typename BeginItr, typename EndItr>
    void add_fragment(BeginItr&& b, EndItr&& e) {
        auto s = m_frags_.new_subset();
        for(; b != e; ++b) s.insert(*b);
        m_frags_.emplace(std::move(s));
    }

    bool operator==(const FragmentedNuclei& rhs) const noexcept {
        return m_frags_ == rhs.m_frags_;
    }

    bool operator!=(const FragmentedNuclei& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    friend base_type;

    reference at_(size_type i) noexcept {return m_}

    const_reference at_(size_type i) const noexcept {
        return m_frags_[i];
    }

    size_type size_() const noexcept { return m_frags_.size(); }

    /// Object actually implementing the set theory
    impl_type m_frags_;
};

} // namespace chemist
