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

#include "chemist/molecule/molecule.hpp"
#include "chemist/set_theory/traits/set_traits.hpp"
#include <catch2/catch.hpp>

using namespace chemist::set_theory;

TEST_CASE("SetTraits<Molecule>") {
    using obj_type = chemist::Molecule;
    using traits   = SetTraits<obj_type>;

    SECTION("size_type") {
        using corr = typename obj_type::size_type;
        using T    = typename traits::size_type;
        STATIC_REQUIRE(std::is_same_v<corr, T>);
    }

    SECTION("value_type") {
        using corr = typename obj_type::value_type;
        using T    = typename traits::value_type;
        STATIC_REQUIRE(std::is_same_v<corr, T>);
    }

    SECTION("reference_type") {
        using corr = typename obj_type::reference;
        using T    = typename traits::reference_type;
        STATIC_REQUIRE(std::is_same_v<corr, T>);
    }

    SECTION("const_reference") {
        using corr = typename obj_type::const_reference;
        using T    = typename traits::const_reference;
        STATIC_REQUIRE(std::is_same_v<corr, T>);
    }

    SECTION("subset_type") {
        using corr = Subset<obj_type>;
        using T    = typename traits::subset_type;
        STATIC_REQUIRE(std::is_same_v<corr, T>);
    }
    // Linear water
    using coord_type = typename chemist::Atom::coord_type;
    chemist::Atom H0("H", 1ul, 0.0, 0.0, 0.0, 1.0);
    chemist::Atom H1("H", 1ul, 0.0, 0.0, 0.0, -1.0);
    chemist::Atom O("O", 8ul, 0.0, 0.0, 0.0, 0.0);
    chemist::Molecule mol{O, H0, H1};

    SECTION("size") { REQUIRE(traits::size(mol) == 3); }

    SECTION("get_elem_i") {
        SECTION("i in bounds") {
            REQUIRE(traits::get_elem_i(mol, 0) == O.nucleus());
            REQUIRE(traits::get_elem_i(mol, 1) == H0.nucleus());
            REQUIRE(traits::get_elem_i(mol, 2) == H1.nucleus());
        }
        SECTION("i not in bounds") {
            REQUIRE_THROWS_AS(traits::get_elem_i(mol, 3), std::out_of_range);
        }
    }

    SECTION("get_index") {
        SECTION("elem in molecule") {
            REQUIRE(traits::get_index(mol, O.nucleus()) == 0);
            REQUIRE(traits::get_index(mol, H0.nucleus()) == 1);
            REQUIRE(traits::get_index(mol, H1.nucleus()) == 2);
        }
        SECTION("elem not in molecule") {
            chemist::Atom U("U", 92ul, 0.0, 0.0, 0.0, 0.0);
            REQUIRE_THROWS_AS(traits::get_index(mol, U.nucleus()),
                              std::out_of_range);
        }
    }
}
