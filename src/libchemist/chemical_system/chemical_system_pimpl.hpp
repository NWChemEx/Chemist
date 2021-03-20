#pragma once
#include "libchemist/chemical_system/chemical_system.hpp"

namespace libchemist::detail_ {

class ChemicalSystemPIMPL {
public:
    using chem_sys_t  = ChemicalSystem;
    using molecule_t  = typename chem_sys_t::molecule_t;
    using pimpl_ptr_t = std::unique_ptr<ChemicalSystemPIMPL>;

    ChemicalSystemPIMPL() = default;
    ChemicalSystemPIMPL(molecule_t mol) : m_mol_(std::move(mol)) {}
    virtual ~ChemicalSystemPIMPL() noexcept    = default;
    ChemicalSystemPIMPL(ChemicalSystemPIMPL&&) = delete;
    ChemicalSystemPIMPL& operator=(ChemicalSystemPIMPL&&) = delete;

    pimpl_ptr_t clone() const { return clone_(); }

    auto& molecule() { return m_mol_; }
    const auto& molecule() const { return m_mol_; }

protected:
    virtual pimpl_ptr_t clone_() const;
    ChemicalSystemPIMPL(const ChemicalSystemPIMPL&) = default;
    ChemicalSystemPIMPL& operator=(const ChemicalSystemPIMPL&) = default;

private:
    molecule_t m_mol_;
};

// Out-of-line inline definitions
inline typename ChemicalSystemPIMPL::pimpl_ptr_t ChemicalSystemPIMPL::clone_()
  const {
    auto pnew = new ChemicalSystemPIMPL(*this);
    return std::unique_ptr<ChemicalSystemPIMPL>(pnew);
}

} // namespace libchemist::detail_