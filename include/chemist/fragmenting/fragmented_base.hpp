#pragma once
#include <chemist/traits/fragmenting_traits.hpp>
#include <optional>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::fragmenting {

template<typename DerivedClass>
class FragmentedBase : public utilities::IndexableContainerBase<DerivedClass> {
private:
    // Type of the base class
    using base_type = utilities::IndexableContainerBase<DerivedClass>;

public:
    /// Type defining the traits for the derived class
    using traits_type = ChemistClassTraits<DerivedClass>;

    /// Type the derived class fragments
    using supersystem_type = typename traits_type::supersystem_type;

    /// Type defining the traits for the class being fragmented
    using supersystem_traits = typename traits_type::supersystem_traits;

    /// Type of a mutable reference to the supersystem
    using supersystem_reference = typename supersystem_traits::view_type;

    /// Type of a read-only reference to the supersystem
    using const_supersystem_reference =
      typename supersystem_traits::const_view_type;

    FragmentedBase() = default;

    FragmentedBase(supersystem_type supersystem) :
      m_supersystem_(std::move(supersystem)) {}

    supersystem_reference supersystem() { return m_supersystem_.value(); }

    const_supersystem_reference supersystem() const {
        return m_supersystem_.value();
    }

    bool is_null() const noexcept { return !m_supersystem_.has_value(); }

private:
    /// The supersystem being fragmented
    std::optional<supersystem_type> m_supersystem_;
};

} // namespace chemist::fragmenting
