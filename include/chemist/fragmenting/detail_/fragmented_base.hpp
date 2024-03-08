#pragma once
#include <chemical_system/chemial_system_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::fragmenting::detail_ {

/** @brief Code factorization for FragmentedNuclei and the like.
 *
 *  @tparam DerivedType The type of the derived class. Assumed to be
 *          FragmentedNuclei,FragmentedMolecule, or FragmentedChemicalSystem.
 *          Assumed to have a typedef value_type which is the type of the
 *          supersystem.
 *
 *  See https://tinyurl.com/mwr6ysvs for design discussions. Note that this
 *  class is essentially an abstract class (virtual functions stemming from
 *  IndexableContainerBase are actually implemented in the derived class, this
 *  class uses downcasting to get to them) and thus can not be instantiated
 *  directly. This class is meant to be used as code factorization.
 */
template<typename DerivedType, typename ValueType>
class FragmentedBase : public utilities::IndexableContainerBase<DerivedType> {
private:
    /// Type of the class we are implementing
    using derived_type = DerivedType;

    /// Type of *this, used for convenience
    using my_type = FragmentedBase<derived_type, ValueType>;

    /// Type of the traits class
    using traits_type = ChemicalSystemClassTraits<ValueType>;

protected:
    /// Type of the base class, used for convenience
    using icb_type = utilities::IndexableContainerBase<derived_type>;

public:
    /// Type of the supersystem and the fragments
    using value_type = typename traits_type::value_type;

    using reference = typename traits_type::reference;

    using const_reference = typename traits_type::const_reference;

    using pointer = std::shared_ptr<value_type>;

    using const_pointer = std::shared_ptr<const value_type>;

    /// Type used for indexing and offsets
    using size_type = typename icb_type::size_type;

    FragmentedBase() = default;

    explicit FragmentedBase(value_type supersystem) :
      m_psupersystem_(std::make_shared<value_type>(std::move(supersystem))),
      m_supersystem_(*m_psupersystem_),
      m_const_supersystem_(*m_psupersystem_) {}

    FragmentedBase(const FragmentedBase& other) :
      m_psupersystem_(other.has_supersystem() ?
                        std::make_shared<value_type>(other.supersystem()) :
                        nullptr),
      m_supersystem_(*m_psupersystem_),
      m_const_supersystem_(*m_psupersystem_) {}

    my_type& operator=(const FragmentedBase& rhs) {
        if(this != &rhs) FragmentedBase(rhs).swap(*this);
        return *this;
    }

    FragmentedBase(FragmentedBase&&) = default;

    my_type& operator=(FragmentedBase&&) = default;

    reference supersystem() { return m_supersystem_; }

    const_reference supersystem() const { return m_const_supersystem_; }

    /** @brief Returns the supersystem (if it was set).
     *
     *  It should be noted that the supersystem can NOT be modified once set as
     *  this may invalidate the fragments.
     *
     *  @return A read-only reference to the supersystem (if it was set).
     *
     *  @throw std::runtime_error if *this does not have a supersystem.
     *          Strong throw guarantee.
     */
    // const_reference supersystem() const {
    //     if(has_supersystem()) return *m_supersystem_;
    //     throw std::runtime_error("Fragmented object is null");
    // }

    pointer supersystem_data() noexcept { return m_psupersystem_; }

    const_pointer supersystem_data() const noexcept { return m_psupersystem_; }

    bool is_null() const noexcept { return static_cast<bool>(m_psupersystem_); }

    /// Does *this have a supersystem?
    bool has_supersystem() const noexcept { return !is_null(); }

    bool operator==(const my_type& rhs) const noexcept {
        if(is_null() != rhs.is_null()) return false;
        if(is_null()) return true;
        return *m_psupersystem_ == *rhs.m_psupersystem_;
    }

    bool operator!=(const my_type& rhs) const noexcept {
        return !(*this == rhs);
    }

    void swap(my_type& other) noexcept {
        m_psupersystem_.swap(other.m_psupersystem_);
        m_supersystem_.swap(other.m_supersystem_);
    }

private:
    /// The actual supersystem (assuming it's been set)
    pointer m_psupersystem_;

    /// A mutable view of the supersystem
    reference m_supersystem_;

    /// A read-only view of the supersystem
    const_reference m_const_supersystem_;
};

} // namespace chemist::fragmenting::detail_
