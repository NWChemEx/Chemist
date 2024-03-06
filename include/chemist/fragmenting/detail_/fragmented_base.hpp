#pragma once
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::fragmenting::detail_ {

/** @brief Code factorization for FragmentedNuclei and the like.
 *
 *  @tparam T The type of the derived class. Assumed to be FragmentedNuclei,
 *            FragmentedMolecule, or FragmentedChemicalSystem. Assumed to have
 *            a typedef value_type which is the type of the supersystem
 *
 *  See https://tinyurl.com/mwr6ysvs for design discussions. Note that this
 *  class is essentially an abstract class (virtual functions stemming from
 *  IndexableContainerBase are actually implemented in the derived class, this
 *  class uses downcasting to get to them) and thus can not be instantiated
 *  directly. This class is meant to be used as code factorization.
 */
template<typename T>
class FragmentedBase : public utilities::IndexableContainerBase<T> {
private:
    /// Type of *this, used for convenience
    using my_type = FragmentedBase<T>;

    /// Type of the class we are implementing
    using derived_type = T;

    /// Type of the base class, used for convenience
    using base_type = utilities::IndexableContainerBase<T>;

public:
    /// Type of the object which has been fragmented.
    using supersystem_type = typename T::value_type;

    /// A read-only reference to the object which as been fragmented
    using const_supersystem_reference = const supersystem_type&;

    /// Type used for indexing and offsets
    using size_type = typename base_type::size_type;

    FragmentedBase() = default;

    explicit FragmentedBase(supersystem_type super_system) :
      m_supersystem_(std::move(supersystem)) {}

    /** @brief Returns the supersystem (if it was set).
     *
     *  It should be noted that the supersystem can NOT be modified once set as
     *  this may invalidate the fragments.
     *
     *  @return A read-only reference to the supersystem (if it was set).
     *
     *  @throw std::bad_optional_access if *this does not have a supersystem.
     *          Strong throw guarantee.
     */
    const_supersystem_reference supersystem() const {
        return m_supersystem_.value();
    }

    bool is_null() const noexcept { return !m_supersystem_.has_value(); }

    bool operator==(const my_type& rhs) const noexcept {
        return m_supersystem_ == rhs.m_supersystem_;
    }

    bool operator!=(const my_type& rhs) const noexcept {
        return !(*this == rhs);
    }

    void swap(my_type& other) noexcept {
        m_supersystem_.swap(other.m_supersystem);
    }

private:
    /// Allows the base class to get at_ and size_
    friend base_type;

    /// Calls derived class's at_ member
    auto at_(size_type i) { return static_cast<T*>(this)->at_(i); }

    /// Calls derived class's at_ const member
    auto at_(size_type i) const { return static_cast<const T*>(this)->at_(i); }

    /// Calls derived class's size_ member
    size_typ size_() const noexcept { return static_cast<T*>(this)->size_(); }

    /// The actual supersystem (assuming it's been set)
    std::optional<supersystem_type> m_supersystem_;
};

} // namespace chemist::fragmenting::detail_
