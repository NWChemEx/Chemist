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

FragmentedMolecule::const_reference FragmentedMolecule::at_(size_type i) const {
    molecule_type::nuclei_type nukes;
    for(const auto& x : m_nuclei_[i])
        nukes.push_back(m_nuclei_.supersystem()[x].as_nucleus());
    if(m_caps_.count(i)) {
        for(const auto& cap_i : m_caps_.at(i)) {
            for(size_type j = 0; j < cap_i.size(); ++j)
                nukes.push_back(cap_i.cap_atom(j).nucleus());
        }
    }
    return molecule_type(m_charges_.at(i), m_multiplicities_[i], nukes);
}

} // namespace chemist
