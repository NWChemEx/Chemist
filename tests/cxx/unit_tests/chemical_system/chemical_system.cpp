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

#include "../catch.hpp"
#include <chemist/chemical_system/chemical_system_class.hpp>
#include <utility>

using namespace chemist;

TEST_CASE("ChemicalSystem") {
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
        ChemicalSystem defaulted;
        ChemicalSystem sys(h);

        REQUIRE(defaulted.molecule() == Molecule{});
        REQUIRE(sys.molecule() == h);

        SECTION("is read/write") {
            sys.molecule()[0].x() = 1.0;
            h[0].x()              = 1.0;
            REQUIRE(sys.molecule() == h);
        }
    }

    SECTION("molecule() const") {
        ChemicalSystem defaulted;
        ChemicalSystem sys(h);

        REQUIRE(std::as_const(defaulted).molecule() == Molecule{});
        REQUIRE(std::as_const(sys).molecule() == h);
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
