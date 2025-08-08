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

#include "../../../test_helpers.hpp"
#include "../../test_qm.hpp"
#include <chemist/quantum_mechanics/wavefunction/determinant.hpp>
#include <chemist/quantum_mechanics/wavefunction/mos.hpp>

/* Testing Strategy:
 *
 * WavefunctionImpl requires a derived class in order to work. We therefore
 * make an object of the derived class, but test the methods implemented in
 * the WavefunctionImpl part of the class. It is assumed that the derived class
 * is tested elsewhere.
 *
 * We also test Wavefunction::are_different here so that we don't have to create
 * a new unit test to test one method.
 */
TEST_CASE("WavefunctionImpl<Determinant<MOs>>") {
    using orbitals_type    = chemist::wavefunction::MOs;
    using determinant_type = chemist::wavefunction::Determinant<orbitals_type>;
    using index_set_type   = typename determinant_type::orbital_index_set_type;

    orbitals_type defaulted_mos;
    auto mos = test_chemist::h2_mos();

    index_set_type i;
    index_set_type i0{0};

    determinant_type defaulted;
    determinant_type value(i0, mos);

    SECTION("clone") {
        REQUIRE(defaulted.clone()->are_equal(defaulted));
        REQUIRE(value.clone()->are_equal(value));
    }

    SECTION("are_equal") {
        REQUIRE(defaulted.are_equal(determinant_type{}));
        REQUIRE_FALSE(defaulted.are_equal(value));
    }

    SECTION("are_different") {
        REQUIRE_FALSE(defaulted.are_different(determinant_type{}));
        REQUIRE(defaulted.are_different(value));
    }
}
