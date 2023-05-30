#pragma once
#include <chemist/nucleus/nuclei.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

class FragmentedNuclei
  : public utilities::IndexableContainerBase<FragmentedNuclei> {
public:
    using supersystem_type = Nuclei;

private:
    using base_type = utilities::IndexableContainerBase<FragmentedNuclei>;

    /// This is the type of the object used to implement *this
    using impl_type = set_theory::FamilyOfSets<supersystem_type>;

public:
    using reference = typename impl_type::reference_type;

    using const_reference = typename impl_type::const_reference;

    using size_type = base_type::size_type;

    FragmentedNuclei() = default;

    explicit FragmentedNuclei(nuclei_type supersystem) :
      m_frags_(std::move(supersystem)) {}

private:
    friend base_type;

    reference at_(size_type i) const noexcept { return m_frags_[i]; }

    size_type size_() const noexcept { return m_frags_.size(); }

    impl_type m_frags_;
};

} // namespace chemist
