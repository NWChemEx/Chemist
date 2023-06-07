#pragma once
#include <chemist/capping/cap_set.hpp>
#include <chemist/molecule/molecule.hpp>
#include <chemist/nucleus/fragmented_nuclei.hpp>
#include <map>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

/** @brief Represents a molecule which has been decomposed into fragments.
 *
 *  The FragmentedNuclei class details how the nuclear framework of a molecule
 *  has been decomposed into fragments. The FragmentedMolecule class extends
 *  on this by also detailing how the electrons are fragmented.
 *
 *  For design details see:
 *  https://nwchemex-project.github.io/Chemist/developer/design/chemistry/fragmented_molecule.html
 */
class FragmentedMolecule
  : public utilities::IndexableContainerBase<FragmentedMolecule> {
private:
    /// Type implementing the container-like API
    using base_type = utilities::IndexableContainerBase<FragmentedMolecule>;

public:
    /// Type of the object we are fragmenting
    using molecule_type = Molecule;

    /// Conceptually this container is filled with Molecule instances
    using value_type = molecule_type;

    // TODO: This should be a view of a molecule
    using reference = Molecule;

    // TODO: This should be a view of a molecule
    using const_reference = Molecule;

    /** @brief Used to represent the charge on each fragment.
     *
     *  This type is ultimately determined by the Molecule class. It will be
     *  a signed integral type.
     */
    using charge_type = Molecule::charge_type;

    /** @brief Used to represent the multiplicity of each fragment
     *
     *  This type is ultimately determined by the Molecule class. It will be
     *  an unsigned integral type.
     */
    using multiplicity_type = Molecule::size_type;

    /// Type of the object storing how the nuclei are fragmented
    using fragmented_nuclei_type = FragmentedNuclei;

    /// Type of the object holding the caps fro a fragment
    using cap_set_type = CapSet;

    /// Type this class uses for indexing and offsets. Unsigned integral type.
    using size_type = fragmented_nuclei_type::size_type;

    /// Type mapping a fragment to its charge
    using charge_map_type = std::vector<charge_type>;

    /// Type of a mutable reference to a fragment's charge
    using charge_reference = typename charge_map_type::reference;

    /// Type of a read-only reference to a fragment's charge
    using const_charge_reference = typename charge_map_type::const_reference;

    /// Type mapping a fragment to its multiplicity
    using multiplicity_map_type = std::vector<multiplicity_type>;

    /// Type of a mutable reference to a fragment's multiplicity
    using multiplicity_reference = typename multiplicity_map_type::reference;

    /// Type of a read-only reference to a fragment's multiplicity
    using const_multiplicity_reference =
      typename multiplicity_map_type::const_reference;

    /** @brief Creates an empty object.
     *
     *  Default constructed FragmentedMolecule objects have no supersystem
     *  associated with them and thus contain no fragments.
     *
     *  @throw None No throw guarantee.
     */
    FragmentedMolecule() = default;

    explicit FragmentedMolecule(fragmented_nuclei_type nuclei,
                                charge_type charge             = 0,
                                multiplicity_type multiplicity = 1);

    molecule_type supersystem() const {
        return molecule_type(m_charge_, m_multiplicity_,
                             m_nuclei_.supersystem());
    }

    charge_reference charge(size_type i) { return m_charges_.at(i); }

    const_charge_reference charge(size_type i) const {
        return m_charges_.at(i);
    }

    multiplicity_reference multiplicity(size_type i) {
        return m_multiplicities_.at(i);
    }

    const_multiplicity_reference multiplicity(size_type i) const {
        return m_multiplicities_.at(i);
    }

    void set_caps(size_type i, cap_set_type caps) {
        m_caps_.emplace(i, std::move(caps));
    }

private:
    /// Allows base class to implement container API via CRTP
    friend base_type;

    /// Implements operator[]/at() via CRTP
    reference at_(size_type i) { return std::as_const(*this).at_(i); }

    /// Implements read-only version of operator[]/at() via CRTP
    const_reference at_(size_type i) const;

    /// Implements size() via CRTP
    size_type size_() const noexcept { return m_nuclei_.size(); }

    /// The fragments
    fragmented_nuclei_type m_nuclei_;

    /// The overall charge of the supersystem
    charge_type m_charge_ = 0;

    /// The overall multiplicity of the supersystem
    multiplicity_type m_multiplicity_ = 1;

    /// Element i contains the caps for fragment i
    std::map<size_type, cap_set_type> m_caps_;

    /// Element i is the charge of fragment i
    charge_map_type m_charges_;

    /// Element i is the multiplicity of fragment i
    multiplicity_map_type m_multiplicities_;
};

} // namespace chemist
