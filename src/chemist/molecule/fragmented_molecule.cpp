#include <chemist/molecule/fragmented_molecule.hpp>

namespace chemist {

FragmentedMolecule::FragmentedMolecule(fragmented_nuclei_type nuclei,
                                       charge_type charge,
                                       multiplicity_type multiplicity) :
  m_nuclei_(std::move(nuclei)),
  m_charge_(charge),
  m_multiplicity_(multiplicity),
  m_caps_(),
  m_charges_(m_nuclei_.size(), 0),
  m_multiplicities_(m_nuclei_.size(), 1) {}

} // namespace chemist
