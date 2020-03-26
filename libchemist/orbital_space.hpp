#pragma once
#include <libchemist/basis_set/ao_basis_set/ao_basis_set.hpp>
#include <libchemist/molecule.hpp>
#include <libchemist/types.hpp>
#include <sde/detail_/memoization.hpp>

namespace libchemist {

/**
 * @brief Class for holding the details related to a particular set of orbitals.
 *
 * We assume that the orbitals within this class can be expanded as a linear
 * combination of atomic-orbitals
 *
 * @tparam element_type
 * @tparam tensor_type
 */
template<typename element_type = double,
         typename tensor_type  = type::tensor<element_type>>
struct OrbitalSpace {
    using basis_type = AOBasisSet<element_type>;

    // AO basis set
    std::shared_ptr<const basis_type> basis;

    // Overlap of AO basis
    std::shared_ptr<const tensor_type> S;

    // Density matrix in the AO basis set
    tensor_type density;

    // Conversion from AO basis to current occupied orbital basis
    tensor_type C_occ;

    // Conversion from AO basis to current virtual orbital basis
    tensor_type C_virt;

    void hash(sde::Hasher& h) const { h(basis, *S, density, C_occ, C_virt); }
};

// Hack until tensor_type == exists
template<typename E1, typename T1, typename E2, typename T2>
bool operator==(const OrbitalSpace<E1, T1>& lhs,
                const OrbitalSpace<E2, T2>& rhs) noexcept {
    return false;
}

// Can use once tensors have ==
//    bool operator==(const OrbitalSpace<T>& lhs, const OrbitalSpace<U>& rhs)
//    noexcept {
//        return (lhs.S == rhs.S &&
//                lhs.basis == rhs.basis &&
//                std::tie(lhs.density, lhs.C_occ, lhs.C_virt) ==
//                std::tie(rhs.density, rhs.C_occ, rhs.C_virt));
//    }

} // end namespace libchemist
