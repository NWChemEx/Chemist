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

#include <catch2/catch.hpp>
#include <chemist/chemical_system/chemical_system/chemical_system_pimpl.hpp>

TEST_CASE("ChemicalSystemPIMPL") {
    using chem_sys_t       = chemist::ChemicalSystem;
    using chem_sys_pimpl_t = chemist::detail_::ChemicalSystemPIMPL;

    chemist::Atom atom("H", 1ul, 0.0, 0.0, 0.0, 0.0); // Mass-less hydrogen
    chemist::Molecule default_mol, h{atom};

    // For all of these typedefs we are just ensuring that the ChemicalSystem
    // and the ChemicalSystemPIMPL class are synched
    SECTION("Typedefs") {
        SECTION("chem_sys_t") {
            using t = typename chem_sys_pimpl_t::chem_sys_t;
            STATIC_REQUIRE(std::is_same_v<t, chem_sys_t>);
        }

        SECTION("pimpl_ptr_t") {
            using t    = typename chem_sys_pimpl_t::pimpl_ptr_t;
            using corr = typename chem_sys_t::pimpl_ptr_t;
        }

        SECTION("molecule_t") {
            using t    = typename chem_sys_pimpl_t::molecule_t;
            using corr = typename chem_sys_t::molecule_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("mol_ref_t") {
            using t    = typename chem_sys_pimpl_t::mol_ref_t;
            using corr = typename chem_sys_t::mol_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("const_mol_ref_t") {
            using t    = typename chem_sys_pimpl_t::const_mol_ref_t;
            using corr = typename chem_sys_t::const_mol_ref_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("size_type") {
            using t    = typename chem_sys_pimpl_t::size_type;
            using corr = typename chem_sys_t::size_type;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }
    }

    SECTION("Default ctor") {
        chem_sys_pimpl_t pimpl;
        REQUIRE(pimpl.molecule() == default_mol);
    }

    SECTION("Value ctor") {
        chem_sys_pimpl_t pimpl(h);
        REQUIRE(pimpl.molecule() == h);
    }

    SECTION("clone") {
        SECTION("Default") {
            chem_sys_pimpl_t pimpl;
            auto copy = pimpl.clone();
            REQUIRE(*copy == pimpl);
        }

        SECTION("Non-default") {
            chem_sys_pimpl_t pimpl(h);
            auto copy = pimpl.clone();
            REQUIRE(*copy == pimpl);
        }
    }

    SECTION("molecule()") {
        SECTION("Default") {
            chem_sys_pimpl_t pimpl;
            REQUIRE(pimpl.molecule() == default_mol);
        }

        SECTION("Has value") {
            chem_sys_pimpl_t pimpl(h);
            REQUIRE(pimpl.molecule() == h);
        }

        SECTION("Is writeable") {
            chem_sys_pimpl_t pimpl;
            pimpl.molecule() = h;
            REQUIRE(pimpl.molecule() == h);
        }
    }

    SECTION("molecule() const") {
        SECTION("Default") {
            const chem_sys_pimpl_t pimpl;
            REQUIRE(pimpl.molecule() == default_mol);
        }

        SECTION("Has value") {
            const chem_sys_pimpl_t pimpl(h);
            REQUIRE(pimpl.molecule() == h);
        }
    }

    SECTION("operator==") {
        SECTION("LHS is default") {
            chem_sys_pimpl_t lhs;

            SECTION("RHS is default") {
                chem_sys_pimpl_t rhs;
                REQUIRE(lhs == rhs);
            }

            SECTION("RHS has a different molecule") {
                chem_sys_pimpl_t rhs(h);
                REQUIRE_FALSE(lhs == rhs);
            }
        }
    }
}
