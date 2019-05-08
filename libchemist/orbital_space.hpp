#pragma once
#include <libchemist/ao_basis_set.hpp>
#include <libchemist/molecule.hpp>
#include <sde/detail_/memoization.hpp>
#include <tamm/tamm.hpp>

namespace libchemist {

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

    void hash(sde::Hasher& h) const { h(basis, *S, density, C); }

};

    // Hack until tamm::Tensor == exists
    template<typename element_type>
    bool operator==(const OrbitalSpace<element_type>& lhs, const OrbitalSpace<element_type>& rhs) noexcept {
        return false;
    }

    // Can use once tensors have ==
//    bool operator==(const OrbitalSpace<element_type>& lhs, const OrbitalSpace<element_type>& rhs) noexcept {
//        return (lhs.S == rhs.S &&
//                lhs.basis == rhs.basis &&
//                std::tie(lhs.density, lhs.Cdagger, lhs.C) ==
//                std::tie(rhs.density, rhs.Cdagger, rhs.C));
//    }

} // end namespace libchemist
