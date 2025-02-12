/*
 * Copyright 2024 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file test_qm.hpp
 *
 *  Contains a series of functions meant to facilitate the testing of the QM
 *  component of Chemist.
 */

#pragma once
#include "../catch.hpp"
#include <chemist/basis_set/ao_basis_set.hpp>
#include <chemist/density/density.hpp>
#include <chemist/electron/electron.hpp>
#include <chemist/nucleus/nucleus.hpp>
#include <chemist/quantum_mechanics/wavefunction/wavefunction.hpp>
#include <cmath>

namespace test_chemist {

// The coordinates (a.u.) of a hydrogen molecule
inline auto h2_coords() {
    return std::array<double, 6>{0.0, 0.0, 0.0, 0.0, 0.0, 1.3983972315};
}

// Creates an AOBasisSet object representing the STO-3G basis for the hydrogen
// molecule.
inline auto h2_basis() {
    using ao_basis_type            = chemist::basis_set::AOBasisSetD;
    using atomic_basis_type        = typename ao_basis_type::value_type;
    using shell_type               = typename atomic_basis_type::value_type;
    using contracted_gaussian_type = typename shell_type::cg_type;
    using center_type = typename atomic_basis_type::shell_traits::center_type;

    auto coords = h2_coords();
    center_type h0_coords(coords[0], coords[1], coords[2]);
    center_type h1_coords(coords[3], coords[4], coords[5]);
    std::vector<double> h_coefs{0.1543289673, 0.5353281423, 0.4446345422};
    std::vector<double> h_exps{3.425250914, 0.6239137298, 0.1688554040};

    contracted_gaussian_type h0_cg(h_coefs.begin(), h_coefs.end(),
                                   h_exps.begin(), h_exps.end(), h0_coords);
    contracted_gaussian_type h1_cg(h_coefs.begin(), h_coefs.end(),
                                   h_exps.begin(), h_exps.end(), h1_coords);

    auto cartesian = shell_type::pure_type::cartesian;
    shell_type::angular_momentum_type l0{0};
    atomic_basis_type h0("STO-3G", 1, h0_coords);
    h0.add_shell(cartesian, l0, h0_cg);
    atomic_basis_type h1("STO-3G", 1, h1_coords);
    h1.add_shell(cartesian, l0, h0_cg);

    ao_basis_type rv;
    rv.add_center(h0);
    rv.add_center(h1);

    return rv;
}

/// AOs for H2 with STO-3G approximating the 1s orbitals
inline auto h2_aos() { return chemist::wavefunction::AOs(h2_basis()); }

/// MOs for H2 that are a 50-50 mix of the two 1s orbitals
inline auto h2_mos() {
    using mos_type    = chemist::wavefunction::MOs;
    using tensor_type = typename mos_type::transform_type;
    tensor_type c({{1.0 / std::sqrt(2.0), 0.0}, {0.0, 1.0 / std::sqrt(2.0)}});
    return mos_type(h2_aos(), std::move(c));
}

/// Creates a defaulted instance of each particle type we want to test
inline auto defaulted_particles() {
    return std::make_tuple(chemist::Electron{}, chemist::ManyElectrons{},
                           chemist::Nucleus{}, chemist::Nuclei{},
                           chemist::Density<chemist::Electron>{},
                           chemist::DecomposableDensity<chemist::Electron>{});
}

/// Creates a non-default instance of each particle type we want to test
inline auto non_defaulted_particles() {
    auto r = h2_coords();
    chemist::Nucleus h0("H", 1ul, 1837.15264648179, r[0], r[1], r[2]);
    chemist::Nucleus h1("H", 1ul, 1837.15264648179, r[3], r[4], r[5]);
    using density_type = chemist::Density<chemist::Electron>;
    typename density_type::value_type rho{{1.0, 0.0}, {0.0, 1.0}};
    density_type rho_e(rho, h2_aos());
    chemist::DecomposableDensity<chemist::Electron> rho_e2(rho, h2_mos());
    return std::make_tuple(chemist::Electron{}, chemist::ManyElectrons{3}, h0,
                           chemist::Nuclei{h0, h1}, rho_e, rho_e2);
}

} // namespace test_chemist
