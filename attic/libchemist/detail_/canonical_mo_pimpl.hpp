#pragma once
#include "chemist/canonical_mos.hpp"
#include "chemist/detail_/orbital_space_pimpl.hpp"
#include <chemist/basis_set/ao_basis_set/ao_basis_set.hpp>

namespace chemist::detail_ {

// Class that implements CanonicalMO
template<typename basis_type, typename tensor_type>
class CanonicalMOsPIMPL : public OrbitalSpacePIMPL<basis_type, tensor_type> {
    using base_pimpl = OrbitalSpacePIMPL<basis_type, tensor_type>;
    tensor_type mo_energies_; // The molecular orbital energies

public:
    /** @brief Creates a new CanonicalMOsPIMPL instance containing empty
     * members. */
    CanonicalMOsPIMPL() : base_pimpl(){};

    /** @brief Creates a new CanonicalMOsPIMPL instance with the provided
     * values.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param mo_energies The molecular orbital energies
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     * @param D The denisty of the MOs
     */
    CanonicalMOsPIMPL(basis_type basis_set, tensor_type S,
                      tensor_type mo_energies, tensor_type C,
                      tensor_type Cdagger, tensor_type D) :
      base_pimpl(basis_set, S, C, Cdagger, D), mo_energies_(mo_energies){};

    /** @brief Creates a new CanonicalMOsPIMPL instance with the provided
     * values.
     *
     * The density of calculated from C and Cdagger
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param mo_energies The molecular orbital energies
     * @param C The coefficients going from the AO space to the MO space
     * @param Cdagger The coefficients going from the MO space to the AO space
     */
    CanonicalMOsPIMPL(basis_type basis_set, tensor_type S,
                      tensor_type mo_energies, tensor_type C,
                      tensor_type Cdagger) :
      base_pimpl(basis_set, S, C, Cdagger), mo_energies_(mo_energies){};

    /** @brief Creates a new CanonicalMOsPIMPL instance with the provided
     * values.
     *
     * Cdagger is assumed to be the transpose of C, and the density calculated
     * as such.
     *
     * @param bs The basis set of the space
     * @param S The overlap matrix of the basis set
     * @param mo_energies The molecular orbital energies
     * @param C The coefficients going from the AO space to the MO space
     */
    CanonicalMOsPIMPL(basis_type basis_set, tensor_type S,
                      tensor_type mo_energies, tensor_type C) :
      base_pimpl(basis_set, S, C), mo_energies_(mo_energies){};

    /** @brief Creates a clone of the CanonicalMOsPIMPL.
     *
     * @return A clone of the current CanonicalMOsPIMPL
     */
    std::unique_ptr<base_pimpl> clone() const override {
        return std::make_unique<CanonicalMOsPIMPL>(
          this->basis_set(), this->S(), mo_energies(), this->C(),
          this->Cdagger(), this->density());
    }

    /** @brief Return a reference to the orbital energies
     *
     * @return A reference to the orbital energies
     */
    const tensor_type& mo_energies() const { return mo_energies_; }

}; // class CanonicalMOsPIMPL

} // namespace chemist::detail_