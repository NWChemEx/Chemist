#pragma once
#include "LibChemist/AOBasisSet.hpp"
#include "LibChemist/Molecule.hpp"
#include <SDE/detail_/Memoization.hpp>
#include <SDE/PropertyType.hpp>
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <tamm/tamm.hpp>

namespace LibChemist {

/**
 * @brief Class for holding the details related to a particular set of orbitals.
 *
 * We assume that the orbitals within this class can be expanded as a linear
 * combination of atomic-orbitals
 *
 * Further considerations:
 * - Rely on TAMM for spin or hold alpha and beta versions?
 * - Need identity matrix class (should be such that TAMM skips evaluations
 *   involving it) to allow for the resulting orbitals to be non-orthogonal and
 *   to ensure the AO space can be treated as a special case of this class
 *   (Cdagger == C == 1)
 *
 *
 * @tparam element_type
 */
template<typename element_type = double>
struct OrbitalSpace {
    using tensor_type = tamm::Tensor<element_type>;
    using basis_type  = AOBasisSet;

    // AO basis set
    std::shared_ptr<const basis_type> basis;

    // Overlap of AO basis
    std::shared_ptr<const tensor_type> S;

    // Density matrix in the AO basis set
    tensor_type density;

    // Conversion from current orbital basis to AO basis
    tensor_type Cdagger;

    // Conversion from AO basis to current basis
    tensor_type C;

    void hash(SDE::Hasher& h) const { h(basis); }
};

/**
 * @brief The base class for modules that build tensors filled with AO
 * integrals.
 *
 * This class is envisioned as wrapping builds of full AO integral tensors.
 *
 * @tparam NBases The total number of AO basis sets involved in the integral.
 * @tparam element_type The type of the element in the tensor
 */
template<std::size_t NBases, typename element_type = double>
struct AOIntegral : SDE::PropertyType<AOIntegral<element_type>> {
    using tensor_type      = tamm::Tensor<element_type>;
    using molecule_type    = Molecule;
    using basis_set_type   = AOBasisSet;
    using basis_array_type = std::array<basis_set_type, NBases>;
    using size_type        = std::size_t;

    // Multiple tensors for things like dipoles?
    //virtual tensor_type run(const molecule_type&, const basis_array_type&,
    //                      size_type deriv) = 0;

    auto inputs_() {
        auto rv = declare_input()
                    .add_field<const molecule_type&>("Molecule")
                    .add_field<const basis_array_type&>("Basis sets")
                    .add_field<size_type>("Derivative");
        rv["Molecule"].set_description("The molecule for which the AO integrals are computed");
        rv["Basis sets"].set_description("The array of basis sets used for the AO integral computation");
        rv["Derivative"].set_description("The derivative order of AO integrals to be computed");
        return rv;
    }

    auto results_() {
        auto rv = declare_result().add_field<tensor_type>("AO Integrals");
        rv["AO Integrals"].set_description("The tensor with computed AO integrals");
        return rv;
    }

};

/**
 * @brief The base class for modules that build the core Hamiltonian in the AO
 * basis set.
 *
 */
template<typename element_type = double>
struct CoreHamiltonian : SDE::PropertyType<CoreHamiltonian<element_type>> {
    using tensor_type    = tamm::Tensor<element_type>;
    using molecule_type  = Molecule;
    using basis_set_type = AOBasisSet;
    using size_type      = std::size_t;

    auto inputs_() {
        auto rv = declare_input()
                    .add_field<const molecule_type&>("Molecule")
                    .add_field<const basis_set_type&>("Bra")
                    .add_field<const basis_set_type&>("Ket")
                    .add_field<size_type>("Derivative");
        rv["Molecule"].set_description("The molecule for which the Core Hamiltonian matrix is build");
        rv["Bra"].set_description("The basis set used for the bra of the Core Hamiltonian matrix");
        rv["Ket"].set_description("The basis set used for the ket of the Core Hamiltonian matrix");
        rv["Derivative"].set_description("The derivative order of the Core Hamiltonian matrix");
        return rv;
    }

    auto results_() {
        auto rv = declare_result().add_field<tensor_type>("Core Hamiltonian");
        rv["Core Hamiltonian"].set_description("The matrix of the computed Core Hamiltonian");
        return rv;
    }

};

/**
 * @brief The base class for modules that build the J and K matrices in the AO
 * basis set.
 *
 * In order to take advantage of integral re-use J and K are typically built
 * together.  This class is designed to build a series of Js and Ks given a
 * series of densities.
 *
 */
template<typename element_type = double>
struct JKMatrices : SDE::PropertyType<JKMatrices<element_type>> {
    using tensor_type    = tamm::Tensor<element_type>;
    using molecule_type  = Molecule;
    using orbital_type   = OrbitalSpace<element_type>;
    using basis_set_type = AOBasisSet;
    using tensor_map     = Utilities::CaseInsensitiveMap<tensor_type>;
    using size_type      = std::size_t;

    auto inputs_() {
        auto rv = declare_input()
                    .add_field<const molecule_type&>("Molecule")
                    .add_field<const orbital_type&>("Molecular Orbitals")
                    .add_field<const basis_set_type&>("Bra")
                    .add_field<const basis_set_type&>("Ket")
                    .add_field<size_type>("Derivative");
        rv["Molecule"].set_description("The molecule for which J and K matrices are build in AO basis");
        rv["Molecular Orbitals"].set_description("The molecular orbitals used to build the J and K matrices");
        rv["Bra"].set_description("The basis set used for the bra of the matrices and integrals");
        rv["Ket"].set_description("The basis set used for the ket of the matrices and integrals");
        rv["Derivative"].set_description("The derivative order of the J and K matrices");
        return rv;
    }

    auto results_() {
        auto rv = declare_result()
                    .add_field<tensor_map>("J Matrix");
                    .add_field<tensor_map>("K Matrix");
        rv["J Matrix"].set_description("The computed J Matrix");
        rv["K Matrix"].set_description("The computed K Matrix");
        return rv;
    }

};

/**
 * @brief The base class for modules that build Fock matrices in the AO
 * basis set.
 *
 */

template<typename element_type = double>
struct FockBuilder : public SDE::PropertyType<FockBuilder<element_type>> {
    using tensor_type    = tamm::Tensor<element_type>;
    using molecule_type  = Molecule;
    using orbital_type   = OrbitalSpace<element_type>;
    using basis_set_type = AOBasisSet;
    using tensor_map     = Utilities::CaseInsensitiveMap<tensor_type>;
    using size_type      = std::size_t;

    auto inputs_() {
        auto rv = declare_input()
                    .add_field<const molecule_type&>("Molecule")
                    .add_field<const orbital_type&>("Molecular Orbitals")
                    .add_field<const basis_set_type&>("Bra")
                    .add_field<const basis_set_type&>("Ket")
                    .add_field<size_type>("Derivative");
        rv["Molecule"].set_description("The molecule for which the Fock Matrix is build");
        rv["Molecular Orbitals"].set_description("The molecular orbitals used to build the Fock Matrix");
        rv["Bra"].set_description("The basis set used for the bra of the matrices and integrals");
        rv["Ket"].set_description("The basis set used for the ket of the matrices and integrals");
        rv["Derivative"].set_description("The derivative order of the Fock Matrix");
        return rv;
    }

    auto results_() {
        auto rv = declare_result().add_field<tensor_map>("Fock Matrix");
        rv["Fock Matrix"].set_description("The computed Fock Matrix");
        return rv;
    }

};

/**
 * @brief The base class for modules that build energy and energy derivatives
 *
 */

template<typename element_type = double>
struct Energy : public SDE::PropertyType<Energy<element_type>> {
    using molecule_type = Molecule;
    using tensor_type   = tamm::Tensor<element_type>;
    using size_type     = std::size_t;

    auto inputs_() {
        auto rv = declare_input()
                    .add_field<const molecule_type&>("Molecule")
                    .add_field<size_type>("Derivative");
        rv["Molecule"].set_description("The molecule for which the energy is calculated");
        rv["Derivative"].set_description("The derivative order of the energy");
        return rv;
    }

    auto results_() {
        auto rv = declare_result().add_field<tensor_type>("Energy");
        rv["Energy"].set_description("The computed energy or derivatives");
        return rv;
    }
};

};

} // namespace LibChemist
