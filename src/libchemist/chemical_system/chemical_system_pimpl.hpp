#pragma once
#include "libchemist/chemical_system/chemical_system.hpp"

namespace libchemist::detail_ {

class ChemicalSystemPIMPL {
public:
    using chem_sys_t = ChemicalSystem;
    using molecule_t = typename chem_sys_t::molecule_t;

    ChemicalSystemPIMPL() = default;
    ChemicalSystemPIMPL(molecule_t mol) : m_mol_(std::move(mol)) {}
    ChemicalSystemPIMPL(ChemicalSystemPIMPL&&) = delete;
    ChemicalSystemPIMPL& operator=(ChemicalSystemPIMPL&&) = delete;

    std::unique_ptr<ChemicalSystemPIMPL> clone();

    auto& molecule() { return m_mol_; }
    const auto& molecule() const { return m_mol_; }

protected:
    ChemicalSystemPIMPL(const ChemicalSystemPIMPL&) = default;
    ChemicalSystemPIMPL& operator=(const ChemicalSystemPIMPL&) = default;

private:
    molecule_t m_mol_;
};

// Out-of-line inline definitions
inline std::unique_ptr<ChemicalSystemPIMPL> ChemicalSystemPIMPL::clone() const {
    return std::make_unique<ChemicalSystemPIMPL>(*this);
}

} // namespace libchemist::detail_