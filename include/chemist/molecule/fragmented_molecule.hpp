#pragma once
#include <chemist/capping/cap_set.hpp>
#include <chemist/molecule/molecule.hpp>
#include <chemist/nucleus/fragmented_nuclei.hpp>
#include <map>

namespace chemist {

class FragmentedMolecule {
public:
    /// Type of the object we are fragmenting
    using molecule_type = Molecule;

    /** @brief Used to represent the charge on each fragment.
     *
     *  This type is ultimately determined by the Molecule class. It will be
     *  a signed integral type.
     */
    using charge_type = Molecule::charge_type;

    using multiplicity_type = Molecule::size_type;

    using fragmented_nuclei_type = FragmentedNuclei;

    using cap_set_type = CapSet;

    /// Type this class uses for indexing and offsets. Unsigned integral type.
    using size_type = fragmented_nuclei_type::size_type;

    /// Type mapping a fragment to its charge
    using charge_map_type = std::vector<charge_type>;

    using charge_reference = typename charge_map_type::reference;

    using const_charge_reference = typename charge_map_type::const_reference;

    /// Type mapping a fragment to its multiplicity
    using multiplicity_map_type = std::vector<multiplicity_type>;

    using multiplicity_reference = typename multiplicity_map_type::reference;

    using const_multiplicity_reference =
      typename multiplicity_map_type::const_reference;

    explicit FragmentedMolecule(fragmented_nuclei_type nuclei,
                                charge_type charge             = 0,
                                multiplicity_type multiplicity = 1);

    charge_reference charge(size_type i) { return m_charges_[i]; }

    const_charge_reference charge(size_type i) const { return m_charges_[i]; }

    multiplicity_reference multiplicity(size_type i) {
        return m_multiplicities_[i];
    }

    const_multiplicity_reference multiplicity(size_type i) const {
        return m_multiplicities_[i];
    }

private:
    /// The fragments
    fragmented_nuclei_type m_nuclei_;

    /// The overall charge of the supersystem
    charge_type m_charge_;

    /// The overall multiplicity of the supersystem
    multiplicity_type m_multiplicity_;

    /// Element i contains the caps for fragment i
    std::map<size_type, cap_set_type> m_caps_;

    /// Element i is the charge of fragment i
    charge_map_type m_charges_;

    /// Element i is the multiplicity of fragment i
    multiplicity_map_type m_multiplicities_;
};

} // namespace chemist
