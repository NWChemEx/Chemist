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

#include "chemist/topology/connectivity_table.hpp"
#include <catch2/catch.hpp>
#include <sstream>

using namespace chemist::topology;

using pimpl_type       = typename ConnectivityTable::pimpl_type;
using pimpl_ptr        = typename ConnectivityTable::pimpl_ptr;
using size_type        = typename ConnectivityTable::size_type;
using offset_pair      = typename ConnectivityTable::offset_pair;
using offset_pair_list = typename ConnectivityTable::offset_pair_list;

TEST_CASE("ConnectivityTable") {
    SECTION("Typedefs") {
        using corr_pimpl = detail_::ConnectivityTablePIMPL;
        STATIC_REQUIRE(std::is_same_v<pimpl_type, corr_pimpl>);

        using corr_pimpl_ptr = std::unique_ptr<corr_pimpl>;
        STATIC_REQUIRE(std::is_same_v<pimpl_ptr, corr_pimpl_ptr>);

        using corr_size_type = std::size_t;
        STATIC_REQUIRE(std::is_same_v<size_type, corr_size_type>);

        using corr_offset_pair = std::array<corr_size_type, 2>;
        STATIC_REQUIRE(std::is_same_v<offset_pair, corr_offset_pair>);

        using corr_bond_list = std::vector<corr_offset_pair>;
        STATIC_REQUIRE(std::is_same_v<offset_pair_list, corr_bond_list>);
    }

    ConnectivityTable t;

    SECTION("Default Ctor") {
        REQUIRE(t.natoms() == 0);
        REQUIRE(t.nbonds() == 0);
        REQUIRE(t.bonds() == offset_pair_list{});
    }

    ConnectivityTable t0(0);
    ConnectivityTable t1(1);
    ConnectivityTable t2(2);
    ConnectivityTable t3(3);

    SECTION("Allocating CTor") {
        SECTION("0 Atoms") {
            REQUIRE(t0.natoms() == 0);
            REQUIRE(t0.nbonds() == 0);
            REQUIRE(t0.bonds() == offset_pair_list{});
        }

        SECTION("1 Atom") {
            REQUIRE(t1.natoms() == 1);
            REQUIRE(t1.nbonds() == 0);
            REQUIRE(t1.bonds() == offset_pair_list{});
        }

        SECTION("2 Atom") {
            REQUIRE(t2.natoms() == 2);
            REQUIRE(t2.nbonds() == 0);
            REQUIRE(t2.bonds() == offset_pair_list{});
        }

        SECTION("3 Atom") {
            REQUIRE(t3.natoms() == 3);
            REQUIRE(t3.nbonds() == 0);
            REQUIRE(t3.bonds() == offset_pair_list{});
        }
    }

    SECTION("Copy CTor") {
        SECTION("Is defaulted") {
            ConnectivityTable CopyOft(t);
            REQUIRE(CopyOft.natoms() == 0);
            REQUIRE(CopyOft.nbonds() == 0);
            REQUIRE(CopyOft.bonds() == offset_pair_list{});
        }
        SECTION("Has state") {
            ConnectivityTable CopyOft3(t3);
            REQUIRE(CopyOft3.natoms() == 3);
            REQUIRE(CopyOft3.nbonds() == 0);
            REQUIRE(CopyOft3.bonds() == offset_pair_list{});
        }
    }

    SECTION("Move CTor") {
        ConnectivityTable Newt3(std::move(t3));
        REQUIRE(Newt3.natoms() == 3);
        REQUIRE(Newt3.nbonds() == 0);
        REQUIRE(Newt3.bonds() == offset_pair_list{});
        REQUIRE(t3.natoms() == 0);
        REQUIRE(t3.nbonds() == 0);
        REQUIRE(t3.bonds() == offset_pair_list{});
    }

    SECTION("Copy Assignment") {
        SECTION("Self-assignment") {
            ConnectivityTable CopyOft3(t3);
            auto pCopyOft3 = &(CopyOft3 = t3);
            REQUIRE(pCopyOft3 == &CopyOft3);
            REQUIRE(CopyOft3 == t3);
        }
        SECTION("Default copy") {
            ConnectivityTable CopyOft3(t3);
            auto pCopyOft3 = &(CopyOft3 = ConnectivityTable{});
            REQUIRE(pCopyOft3 == &CopyOft3);
            REQUIRE(CopyOft3 == t);
        }
        SECTION("Normal Copy") {
            ConnectivityTable CopyOft3;
            auto pCopyOft3 = &(CopyOft3 = t3);
            REQUIRE(pCopyOft3 == &CopyOft3);
            REQUIRE(CopyOft3 == t3);
        }
    }

    SECTION("Move Assignment") {
        SECTION("Default move") {
            ConnectivityTable CopyOft3(t3);
            auto pCopyOft3 = &(CopyOft3 = std::move(t));
            REQUIRE(pCopyOft3 == &CopyOft3);
            REQUIRE(CopyOft3 == ConnectivityTable{});
        }
        SECTION("Normal move") {
            ConnectivityTable CopyOft3(t3);
            ConnectivityTable lhs;
            auto plhs = &(lhs = std::move(CopyOft3));
            REQUIRE(plhs == &lhs);
            REQUIRE(lhs == t3);
            REQUIRE(CopyOft3 == t);
        }
    }

    SECTION("swap") {
        ConnectivityTable lhs_copy(t0);
        ConnectivityTable rhs_copy(t2);

        t0.swap(t2);

        REQUIRE(t0 == rhs_copy);
        REQUIRE(t2 == lhs_copy);
    }

    SECTION("set_n_atoms/natoms") {
        REQUIRE(t0.natoms() == 0);
        t0.set_n_atoms(4);
        REQUIRE(t0.natoms() == 4);
    }

    SECTION("add_bond/nbonds") {
        REQUIRE(t3.nbonds() == 0);
        t3.add_bond(0, 1);
        REQUIRE(t3.nbonds() == 1);

        SECTION("Throws if out of range") {
            REQUIRE_THROWS_AS(t3.add_bond(42, 42), std::out_of_range);
        }
    }

    SECTION("bonded_atoms") {
        REQUIRE_THROWS_AS(t.bonded_atoms(0), std::out_of_range);
        REQUIRE_THROWS_AS(t0.bonded_atoms(0), std::out_of_range);
        REQUIRE_THROWS_AS(t1.bonded_atoms(1), std::out_of_range);
        REQUIRE_THROWS_AS(t2.bonded_atoms(2), std::out_of_range);
        REQUIRE_THROWS_AS(t3.bonded_atoms(3), std::out_of_range);

        using set_type = std::set<size_type>;

        REQUIRE(t3.bonded_atoms(0) == set_type{});

        t3.add_bond(0, 1);
        REQUIRE(t3.bonded_atoms(0) == set_type{1});
        REQUIRE(t3.bonded_atoms(1) == set_type{0});
        t3.add_bond(1, 2);
        REQUIRE(t3.bonded_atoms(1) == set_type{0, 2});
        REQUIRE(t3.bonded_atoms(2) == set_type{1});
    }

    SECTION("Comparisons") {
        SECTION("LHS == default") {
            SECTION("RHS == default") {
                REQUIRE(t == ConnectivityTable{});
                REQUIRE_FALSE(t != ConnectivityTable{});
            }

            SECTION("RHS == 0 Atoms") {
                REQUIRE(t == t0);
                REQUIRE_FALSE(t != t0);
            }

            SECTION("RHS has different number of atoms") {
                REQUIRE_FALSE(t == t1);
                REQUIRE(t != t1);
            }

            // No way for RHS have to only a different bonds or number of bonds
        }

        SECTION("LHS has natoms set") {
            SECTION("RHS == default") {
                REQUIRE_FALSE(t3 == t);
                REQUIRE(t3 != t);
            }

            SECTION("RHS is equal") {
                REQUIRE(t3 == ConnectivityTable{3});
                REQUIRE_FALSE(t3 != ConnectivityTable{3});
            }

            SECTION("RHS has different number of atoms") {
                REQUIRE_FALSE(t3 == t2);
                REQUIRE(t3 != t2);
            }

            SECTION("RHS has different number of bonds") {
                ConnectivityTable t3_2(3);
                t3_2.add_bond(0, 1);
                REQUIRE_FALSE(t3 == t3_2);
                REQUIRE(t3 != t3_2);
            }
        }

        SECTION("LHS has bonds") {
            t3.add_bond(0, 1);

            SECTION("RHS is equal") {
                ConnectivityTable t3_2(3);
                t3_2.add_bond(0, 1);
                REQUIRE(t3 == t3_2);
                REQUIRE_FALSE(t3 != t3_2);
            }

            SECTION("RHS has a different bond") {
                ConnectivityTable t3_2(3);
                t3_2.add_bond(1, 2);
                REQUIRE_FALSE(t3 == t3_2);
                REQUIRE(t3 != t3_2);
            }
        }
    }

    SECTION("Printing") {
        t3.add_bond(0, 1);
        t3.add_bond(1, 2);

        std::stringstream ss;
        auto pss         = &(ss << t3);
        std::string corr = "0 1\n1 2\n";
        REQUIRE(pss == &ss);
        REQUIRE(ss.str() == corr);
    }
}
