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

#include "../../catch.hpp"
#include "chemist/topology/connectivity_table/connectivity_table_pimpl.hpp"
#include <type_traits>

using namespace chemist::topology::detail_;

using size_type        = typename ConnectivityTablePIMPL::size_type;
using offset_pair      = typename ConnectivityTablePIMPL::offset_pair;
using offset_pair_list = typename ConnectivityTablePIMPL::offset_pair_list;

TEST_CASE("ConnectivityTablePIMPL") {
    ConnectivityTablePIMPL defaulted;

    SECTION("Typedefs") {
        using ct_t = chemist::topology::ConnectivityTable;
        // Make sure the PIMPL uses the same type as the main class
        STATIC_REQUIRE(std::is_same_v<size_type, typename ct_t::size_type>);
        STATIC_REQUIRE(std::is_same_v<offset_pair, typename ct_t::offset_pair>);
        using corr_bond_list = typename ct_t::offset_pair_list;
        STATIC_REQUIRE(std::is_same_v<offset_pair_list, corr_bond_list>);
    }

    SECTION("Default CTor") {
        REQUIRE(defaulted.natoms() == 0);
        REQUIRE(defaulted.nbonds() == 0);
    }

    SECTION("Allocating CTor") {
        for(size_type i = 0; i < 10; ++i) {
            SECTION(std::to_string(i) + " atoms") {
                ConnectivityTablePIMPL p(i);
                REQUIRE(p.natoms() == i);
                REQUIRE(p.nbonds() == 0);
            }
        }
    }

    ConnectivityTablePIMPL p0;
    ConnectivityTablePIMPL p1(1);
    ConnectivityTablePIMPL p2(2);
    ConnectivityTablePIMPL p3(3);

    SECTION("Copy CTor") {
        ConnectivityTablePIMPL CopyOfP3(p3);
        REQUIRE(CopyOfP3.natoms() == 3);
        REQUIRE(CopyOfP3.nbonds() == 0);
    }

    SECTION("set_natoms/natoms") {
        SECTION("Change from defaulted") {
            p0.set_natoms(1);
            REQUIRE(p0.natoms() == 1);
        }
        SECTION("Change from non-zero") {
            p1.set_natoms(4);
            REQUIRE(p1.natoms() == 4);
        }
        SECTION("Changing to the same number doesn't reallocate") {
            p3.add_bond(0, 1);
            p3.set_natoms(3);
            REQUIRE(p3.natoms() == 3);
            REQUIRE(p3.nbonds() == 1);
            REQUIRE(p3.are_bonded(0, 1));
        }
    }

    SECTION("add_bond/nbondes/are_bonded") {
        SECTION("Throws if atom is out of bounds") {
            REQUIRE_THROWS_AS(p0.add_bond(0, 0), std::out_of_range);
            REQUIRE_THROWS_AS(p1.add_bond(0, 1), std::out_of_range);
            REQUIRE_THROWS_AS(p2.add_bond(3, 1), std::out_of_range);
            REQUIRE_THROWS_AS(p3.add_bond(4, 4), std::out_of_range);
        }

        SECTION("Can't add self bond") {
            REQUIRE_THROWS_AS(p1.add_bond(0, 0), std::out_of_range);
        }

        SECTION("Add bond to two-atom system") {
            REQUIRE(p2.nbonds() == 0);
            REQUIRE_FALSE(p2.are_bonded(0, 1));

            p2.add_bond(0, 1);

            REQUIRE(p2.nbonds() == 1);
            REQUIRE(p2.are_bonded(0, 1));
        }

        SECTION("Add bonds to three-atom system") {
            REQUIRE(p3.nbonds() == 0);
            REQUIRE_FALSE(p3.are_bonded(0, 2));
            REQUIRE_FALSE(p3.are_bonded(1, 2));

            p3.add_bond(0, 2);

            REQUIRE(p3.nbonds() == 1);
            REQUIRE(p3.are_bonded(0, 2));
            REQUIRE_FALSE(p3.are_bonded(1, 2));

            p3.add_bond(1, 2);
            REQUIRE(p3.nbonds() == 2);
            REQUIRE(p3.are_bonded(0, 2));
            REQUIRE(p3.are_bonded(1, 2));
        }

        SECTION("Adding same bond twice is okay and doesn't change state") {
            REQUIRE(p2.nbonds() == 0);
            REQUIRE_FALSE(p2.are_bonded(0, 1));

            p2.add_bond(0, 1);

            REQUIRE(p2.nbonds() == 1);
            REQUIRE(p2.are_bonded(0, 1));

            p2.add_bond(0, 1);
            REQUIRE(p2.nbonds() == 1);
            REQUIRE(p2.are_bonded(0, 1));
        }
    }

    SECTION("bonds") {
        SECTION("No bonds") { REQUIRE(p0.bonds() == offset_pair_list{}); }

        SECTION("One bond") {
            p3.add_bond(0, 1);
            offset_pair_list corr{offset_pair{0, 1}};
            REQUIRE(p3.bonds() == corr);
        }

        SECTION("Two bonds") {
            p3.add_bond(0, 1);
            p3.add_bond(0, 2);
            offset_pair_list corr{offset_pair{0, 1}, offset_pair{0, 2}};
            REQUIRE(p3.bonds() == corr);
        }

        SECTION("Three bonds") {
            p3.add_bond(0, 1);
            p3.add_bond(0, 2);
            p3.add_bond(1, 2);

            offset_pair pair0{0, 1};
            offset_pair pair1{0, 2};
            offset_pair pair2{1, 2};
            offset_pair_list corr{pair0, pair1, pair2};

            REQUIRE(p3.bonds() == corr);
        }
    }
}
