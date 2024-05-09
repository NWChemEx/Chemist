#pragma once
#include <chemist/chemical_system/chemical_system/chemical_system_view/chemical_system_view.hpp>

namespace chemist::detail_ {

template<typename ChemicalSystemType>
class ChemicalSystemViewPIMPL {
private:
    /// Type of *this
    using my_type = ChemicalSystemViewPIMPL<ChemicalSystemType>;

public:
    /// Type *this aliases
    using parent_type = ChemicalSystemView<ChemicalSystemType>;

    /// Pull in parent's types
    ///@{
    using pimpl_pointer      = typename parent_type::pimpl_pointer;
    using molecule_reference = typename parent_type::molecule_reference;
    using const_molecule_reference =
      typename parent_type::const_molecule_reference;
    ///@}

    explicit ChemicalSystemViewPIMPL(molecule_reference molecule) :
      m_molecule_(std::move(molecule)) {}

    /** @brief Initializes *this to a copy of @p rhs such that *this willl
     *         alias the same ChemicalSystem
     *
     *  @param[in] rhs The object to copy.
     *
     *  @throw std::bad_alloc if there is a problem copying @p rhs. Strong
     *                        throw guarantee.
     *
     */
    ChemicalSystemViewPIMPL(const ChemicalSystemViewPIMPL& rhs) = default;

    /// Will be implemented generically by the main class
    ///@{
    ChemicalSystemViewPIMPL(ChemicalSystemViewPIMPL&&) noexcept        = delete;
    ChemicalSystemViewPIMPL& operator=(const ChemicalSystemViewPIMPL&) = delete;
    ChemicalSystemViewPIMPL& operator=(ChemicalSystemViewPIMPL&&)      = delete;
    ///@}

    molecule_reference molecule() { return m_molecule_; }

    const_molecule_reference molecule() const { return m_molecule_; }

    pimpl_pointer clone() const { return std::make_unique<my_type>(*this); }

    bool operator==(const ChemicalSystemViewPIMPL& rhs) const noexcept {
        return m_molecule_ == rhs.m_molecule_;
    }

private:
    /// An alias of the Molecule piece of *this
    molecule_reference m_molecule_;
};

} // namespace chemist::detail_
