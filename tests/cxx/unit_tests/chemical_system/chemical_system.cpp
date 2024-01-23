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
#include <chemist/chemical_system/chemical_system_class.hpp>
#include <utility>

using namespace chemist;

TEST_CASE("ChemicalSystem") {
    SECTION("Typedefs") {
        SECTION("pimpl_t") {
            using pimpl_t = typename ChemicalSystem::pimpl_t;
            using corr_t  = detail_::ChemicalSystemPIMPL;
            STATIC_REQUIRE(std::is_same_v<pimpl_t, corr_t>);
        }

        SECTION("pimpl_ptr_t") {
            using pimpl_ptr_t = typename ChemicalSystem::pimpl_ptr_t;
            using corr_t      = std::unique_ptr<detail_::ChemicalSystemPIMPL>;
            STATIC_REQUIRE(std::is_same_v<pimpl_ptr_t, corr_t>);
        }

        SECTION("molecule_t") {
            using molecule_t = typename ChemicalSystem::molecule_t;
            using corr_t     = Molecule;
            STATIC_REQUIRE(std::is_same_v<molecule_t, corr_t>);
        }

        SECTION("mol_ref_t") {
            using mol_ref_t = typename ChemicalSystem::mol_ref_t;
            using corr_t    = Molecule&;
            STATIC_REQUIRE(std::is_same_v<mol_ref_t, corr_t>);
        }

        SECTION("const_mol_ref_t") {
            using const_mol_ref_t = typename ChemicalSystem::const_mol_ref_t;
            using corr_t          = const Molecule&;
            STATIC_REQUIRE(std::is_same_v<const_mol_ref_t, corr_t>);
        }

        SECTION("size_type") {
            using t    = typename ChemicalSystem::size_type;
            using corr = std::size_t;
            STATIC_REQUIRE(std::is_same_v<t, corr>);
        }

        SECTION("charge_type") {
            using corr_t      = int;
            using charge_type = typename ChemicalSystem::charge_type;
        }
    }

    chemist::Atom atom("H", 1ul, 0.0, 0.0, 0.0, 0.0); // Mass-less hydrogen
    chemist::Molecule default_mol, h{atom};

    SECTION("Default ctor") {
        ChemicalSystem sys;
        REQUIRE(sys.molecule() == default_mol);
    }

    SECTION("Copy ctor") {
        ChemicalSystem sys(h);
        ChemicalSystem copy(sys);
        REQUIRE(sys == copy);
        REQUIRE(copy.molecule() == h);
    }

    SECTION("Move ctor") {
        ChemicalSystem sys(h);
        ChemicalSystem moved(std::move(sys));
        REQUIRE(moved.molecule() == h);
    }

    SECTION("value ctors") {
        SECTION("Default n_electrons and potential") {
            ChemicalSystem sys(h);
            REQUIRE(sys.molecule() == h);
        }
    }

    SECTION("copy assignment") {
        ChemicalSystem sys(h);
        ChemicalSystem copy;
        auto pcopy = &(copy = sys);
        REQUIRE(sys == copy);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.molecule() == h);
    }

    SECTION("move assignment") {
        ChemicalSystem sys(h);
        ChemicalSystem moved;
        auto pmoved = &(moved = std::move(sys));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.molecule() == h);
    }

    SECTION("molecule()") {
        ChemicalSystem sys(h);

        SECTION("value") { REQUIRE(sys.molecule() == h); }

        SECTION("is read/write") {
            sys.molecule() = Molecule{};
            REQUIRE(sys.molecule() == Molecule{});
        }

        SECTION("Allocates new PIMPL if no PIMPL") {
            ChemicalSystem buffer(std::move(sys));
            REQUIRE(sys.molecule() == Molecule{});
        }
    }

    SECTION("molecule() const") {
        ChemicalSystem sys(h);

        SECTION("value") { REQUIRE(std::as_const(sys).molecule() == h); }

        SECTION("Throws if no PIMPL") {
            ChemicalSystem buffer(std::move(sys));
            const auto& csys = sys;
            REQUIRE_THROWS_AS(csys.molecule(), std::runtime_error);
        }
    }

    SECTION("operator==") {
        SECTION("LHS is default") {
            ChemicalSystem lhs;

            SECTION("RHS is default") {
                ChemicalSystem rhs;
                REQUIRE(lhs == rhs);
            }

            SECTION("RHS has a different molecule") {
                ChemicalSystem rhs(h);
                REQUIRE_FALSE(lhs == rhs);
            }
        }
    }
}
