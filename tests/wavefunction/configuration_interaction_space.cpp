/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/operators/kinetic.hpp"
#include "chemist/operators/multipole.hpp"
#include "chemist/wavefunction/configuration_interaction_space.hpp"
#include "test_wavefunction.hpp"
#include "../test_chemist.hpp"

using namespace chemist::wavefunction;
using namespace chemist::orbital_space;

using tuple_type =
  std::tuple<DerivedConfigurationInteractionSpace>;

/* Testing strategy:
 *
 * We assume that the orbital space classes are thoroughly tested and work as
 * expected. Ditto for the FockOperator class. Thus with respect to the values
 * of the members the only thing that matters to this class is we can get and
 * set them and that they are taken into account in comparisons.
 */

TEMPLATE_LIST_TEST_CASE("ConfigurationInteractionSpace", "", tuple_type) {
    using space_t         = TestType;
    using inact_space_t   = typename TestType::inactive_orbital_type;
    using act_space_t     = typename TestType::active_orbital_type;
    using virt_space_t    = typename TestType::virtual_orbital_type;
    using core_space_t    = typename TestType::core_orbital_type;
    using ordm_t          = typename TestType::active_1rdm_type;
    using trdm_t          = typename TestType::active_2rdm_type;
    using inact_tensor_t  = typename inact_space_t::transform_type;
    using act_tensor_t    = typename act_space_t::transform_type;
    using virt_tensor_t   = typename virt_space_t::transform_type;
    using core_tensor_t   = typename core_space_t::transform_type;

    auto& world = TA::get_default_world();

    // A default ConfigurationInteractionSpace
    space_t defaulted;

    // Makes a non-default ConfigurationInteractionSpace
    int nalpha = 1, nbeta = 3;
    auto inact  = testing::make_space<inact_space_t>(1.0);
    auto act    = testing::make_space<act_space_t>(1.2);
    auto virt   = testing::make_space<virt_space_t>(2.0);
    auto core   = testing::make_space<core_space_t>(3.5);
    auto ordm_tensor = testing::generate_tensor(2);
    auto trdm_tensor = testing::generate_tensor(4);
    ordm_t a1rdm(ordm_tensor,virt);
    trdm_t a2rdm(trdm_tensor,virt);
    space_t nondefault(nalpha, nbeta, inact, act, virt, core, a1rdm, a2rdm);

    SECTION("CTors") {
        SECTION("value") {
            REQUIRE(nondefault.n_active_alpha_electrons() == nalpha);
            REQUIRE(nondefault.n_active_beta_electrons() == nbeta);
            REQUIRE(nondefault.inactive_orbitals() == inact);
            REQUIRE(nondefault.active_orbitals() == act);
            REQUIRE(nondefault.virtual_orbitals() == virt);
            REQUIRE(nondefault.core_orbitals() == core);
            REQUIRE(nondefault.active_1rdm() == a1rdm);
            REQUIRE(nondefault.active_2rdm() == a2rdm);
        }

        SECTION("copy") {
            space_t copy(nondefault);
            REQUIRE(copy.n_active_alpha_electrons() == nalpha);
            REQUIRE(copy.n_active_beta_electrons() == nbeta);
            REQUIRE(copy.inactive_orbitals() == inact);
            REQUIRE(copy.active_orbitals() == act);
            REQUIRE(copy.virtual_orbitals() == virt);
            REQUIRE(copy.core_orbitals() == core);
            REQUIRE(copy.active_1rdm() == a1rdm);
            REQUIRE(copy.active_2rdm() == a2rdm);
        }

        SECTION("move") {
            space_t moved(std::move(nondefault));
            REQUIRE(moved.n_active_alpha_electrons() == nalpha);
            REQUIRE(moved.n_active_beta_electrons() == nbeta);
            REQUIRE(moved.inactive_orbitals() == inact);
            REQUIRE(moved.active_orbitals() == act);
            REQUIRE(moved.virtual_orbitals() == virt);
            REQUIRE(moved.core_orbitals() == core);
            REQUIRE(moved.active_1rdm() == a1rdm);
            REQUIRE(moved.active_2rdm() == a2rdm);
        }

        SECTION("copy assignment") {
            space_t copy;
            auto pcopy = &(copy = nondefault);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.n_active_alpha_electrons() == nalpha);
            REQUIRE(copy.n_active_beta_electrons() == nbeta);
            REQUIRE(copy.inactive_orbitals() == inact);
            REQUIRE(copy.active_orbitals() == act);
            REQUIRE(copy.virtual_orbitals() == virt);
            REQUIRE(copy.core_orbitals() == core);
            REQUIRE(copy.active_1rdm() == a1rdm);
            REQUIRE(copy.active_2rdm() == a2rdm);
        }

        SECTION("move assignment") {
            space_t moved;
            auto pmoved = &(moved = nondefault);
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.n_active_alpha_electrons() == nalpha);
            REQUIRE(moved.n_active_beta_electrons() == nbeta);
            REQUIRE(moved.inactive_orbitals() == inact);
            REQUIRE(moved.active_orbitals() == act);
            REQUIRE(moved.virtual_orbitals() == virt);
            REQUIRE(moved.core_orbitals() == core);
            REQUIRE(moved.active_1rdm() == a1rdm);
            REQUIRE(moved.active_2rdm() == a2rdm);
        }
    }

    SECTION("n_active_alpha_electrons") {
        REQUIRE(defaulted.n_active_alpha_electrons() == 0);
        REQUIRE(nondefault.n_active_alpha_electrons() == nalpha);
    }

    SECTION("n_active_beta_electrons") {
        REQUIRE(defaulted.n_active_beta_electrons() == 0);
        REQUIRE(nondefault.n_active_beta_electrons() == nbeta);
    }

    SECTION("inactive_orbitals") {
        REQUIRE_THROWS_AS(defaulted.inactive_orbitals(), std::runtime_error);
        REQUIRE(nondefault.inactive_orbitals() == inact);
    }

    SECTION("active_orbitals") {
        REQUIRE_THROWS_AS(defaulted.active_orbitals(), std::runtime_error);
        REQUIRE(nondefault.active_orbitals() == act);
    }

    SECTION("virtual_orbitals") {
        REQUIRE_THROWS_AS(defaulted.virtual_orbitals(), std::runtime_error);
        REQUIRE(nondefault.virtual_orbitals() == virt);
    }

    SECTION("core_orbitals") {
        REQUIRE_THROWS_AS(defaulted.core_orbitals(), std::runtime_error);
        REQUIRE(nondefault.core_orbitals() == core);
    }

    SECTION("active_1rdm") {
        REQUIRE_THROWS_AS(defaulted.active_1rdm(), std::runtime_error);
        REQUIRE(nondefault.active_1rdm() == a1rdm);
    }

    SECTION("active_2rdm") {
        REQUIRE_THROWS_AS(defaulted.active_2rdm(), std::runtime_error);
        REQUIRE(nondefault.active_2rdm() == a2rdm);
    }


    SECTION("comparisons") {
        SECTION("LHS is default") {
            SECTION("Same value") {
                REQUIRE(defaulted == space_t{});
                REQUIRE_FALSE(defaulted != space_t{});
            }
            SECTION("Different alpha electrons") {
                space_t rhs(nalpha+10, nbeta, inact, act_space_t{},  virt_space_t{}, core_space_t{}, ordm_t{}, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different beta electrons") {
                space_t rhs(nalpha, nbeta+10, inact, act_space_t{},  virt_space_t{}, core_space_t{}, ordm_t{}, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different inactive") {
                space_t rhs(nalpha, nbeta, inact, act_space_t{},  virt_space_t{}, core_space_t{}, ordm_t{}, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different active") {
                space_t rhs(nalpha, nbeta, inact_space_t{}, act,  virt_space_t{}, core_space_t{}, ordm_t{}, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different virtual") {
                space_t rhs(nalpha, nbeta, inact_space_t{}, act_space_t{}, virt, core_space_t{}, ordm_t{}, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different core") {
                space_t rhs(nalpha, nbeta, inact_space_t{}, act_space_t{}, virt_space_t{}, core, ordm_t{}, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different active 1RDM") {
                space_t rhs(nalpha, nbeta, inact_space_t{}, act_space_t{}, virt_space_t{}, core_space_t{}, a1rdm, trdm_t{});
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
            SECTION("Different active 2RDM") {
                space_t rhs(nalpha, nbeta, inact_space_t{}, act_space_t{}, virt_space_t{}, core_space_t{}, ordm_t{}, a2rdm);
                REQUIRE(defaulted != rhs);
                REQUIRE_FALSE(defaulted == rhs);
            }
        }
    }
}

