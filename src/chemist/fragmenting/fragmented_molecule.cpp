#include <chemist/fragmenting/fragmented_molecule.hpp>

namespace chemist::fragmenting {
namespace detail_ {

template<typename MoleculeType>
class FragmentedMoleculePIMPL {
public:
    /// The type *this implements
    using parent_type = FragmentedMolecule<MoleculeType>;
};

} // namespace detail_

#define TPARAMS template<typename MoleculeType>
#define FRAGMENTED_MOLECULE FragmentedMolecule<MoleculeType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, dtor
// -----------------------------------------------------------------------------

TPARAMS
FRAGMENTED_MOLECULE::FragmentedMolecule() noexcept = default;

TPARAMS
FRAGMENTED_MOLECULE::~FragmentedMolecule() noexcept = default;

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool FRAGMENTED_MOLECULE::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

#undef FRAGMENTED_MOLECULE
#undef TPARAMS

template class FragmentedMolecule<Molecule>;
template class FragmentedMolecule<const Molecule>;

} // namespace chemist::fragmenting
