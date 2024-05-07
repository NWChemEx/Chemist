#include "detail_/molecule_view_pimpl.hpp"

namespace chemist {

#define TPARAMS template<typename MoleculeType>
#define MOLECULE_VIEW MoleculeView<MoleculeType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
MOLECULE_VIEW::MoleculeView() noexcept = default;

TPARAMS
MOLECULE_VIEW::MoleculeView(molecule_reference mol) :
  MoleculeView(mol.nuclei(), mol.charge_data(), mol.multiplicity_data()) {}

TPARAMS
MOLECULE_VIEW::MoleculeView(nuclei_reference nuclei, charge_pointer pcharge,
                            multiplicity_pointer pmultiplicity) :
  m_pimpl_(
    std::make_unique<pimpl_type>(std::move(nuclei), pcharge, pmultiplicity)) {}

TPARAMS
MOLECULE_VIEW::MoleculeView(const MoleculeView& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS MOLECULE_VIEW::~MoleculeView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

TPARAMS
typename MOLECULE_VIEW::nuclei_reference MOLECULE_VIEW::nuclei() {
    return has_pimpl_() ? m_pimpl_->nuclei() : nuclei_reference{};
}

TPARAMS
typename MOLECULE_VIEW::const_nuclei_reference MOLECULE_VIEW::nuclei() const {
    return has_pimpl_() ? std::as_const(*m_pimpl_).nuclei() :
                          const_nuclei_reference{};
}

TPARAMS
typename MOLECULE_VIEW::charge_type MOLECULE_VIEW::charge() const noexcept {
    return has_pimpl_() ? std::as_const(*m_pimpl_).charge() : 0;
}

TPARAMS
typename MOLECULE_VIEW::multiplicity_type MOLECULE_VIEW::multiplicity()
  const noexcept {
    return has_pimpl_() ? std::as_const(*m_pimpl_).multiplicity() : 1;
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
typename MOLECULE_VIEW::const_charge_pointer MOLECULE_VIEW::charge_data()
  const noexcept {
    return has_pimpl_() ? &(std::as_const(*m_pimpl_).charge()) : nullptr;
}

TPARAMS
typename MOLECULE_VIEW::const_multiplicity_pointer
MOLECULE_VIEW::multiplicity_data() const noexcept {
    return has_pimpl_() ? &(std::as_const(*m_pimpl_).multiplicity()) : nullptr;
}

TPARAMS
void MOLECULE_VIEW::swap(MoleculeView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool MOLECULE_VIEW::operator==(const MoleculeView& rhs) const noexcept {
    if(this->size() != rhs.size()) return false;
    if(this->size() == 0) return true; // Both empty
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool MOLECULE_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

#undef MOLECULE_VIEW
#undef TPARAMS

template class MoleculeView<Molecule>;
template class MoleculeView<const Molecule>;

} // namespace chemist
