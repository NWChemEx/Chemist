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

#include "chemist/managers/periodic_table.hpp"
#include <catch2/catch.hpp>
#include <string>

using namespace chemist;

using Catch::Matchers::Message;

inline void load_elements(PeriodicTable& pt) {
    pt.insert(0, Atom(0ul, 0.0, "Ez"));

    pt.insert(1, Atom(1ul, 1837.4260218693814, "H"));
    pt.add_isotope(1, 1, Atom(1ul, 1837.1526472934618, "H"));
    pt.add_isotope(1, 2, Atom(1ul, 3671.4829413173247, "H"));

    pt.insert(2, Atom(2ul, 7296.297100609073, "He"));
    pt.add_isotope(2, 3, Atom(2ul, 5497.885121445487, "He"));
    pt.add_isotope(2, 4, Atom(2ul, 7296.299386693523, "He"));
}

TEST_CASE("PeriodicTable Copy/Move") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("Copy Ctor") {
        PeriodicTable pt2(pt);

        REQUIRE(pt == pt2);
    }

    SECTION("Move Ctor") {
        PeriodicTable pt2(std::move(pt));
        PeriodicTable corr;
        std::string msg = "PIMPL is not set";

        load_elements(corr);

        // Compare to copy with same data values
        REQUIRE(pt2 == corr);

        // pt should no longer have a PIMPL, and will throw
        REQUIRE_THROWS_MATCHES(pt == pt2, std::runtime_error, Message(msg));
    }

    SECTION("Copy Assignment") {
        PeriodicTable pt2;

        pt2 = pt;

        REQUIRE(pt == pt2);
    }

    SECTION("Move Assignment") {
        PeriodicTable pt2;
        PeriodicTable corr;
        std::string msg = "PIMPL is not set";

        load_elements(corr);

        pt2 = std::move(pt);

        // Compare to copy with same data values
        REQUIRE(pt2 == corr);

        // pt should no longer have a PIMPL, and will throw
        REQUIRE_THROWS_MATCHES(pt == pt2, std::runtime_error, Message(msg));
    }
}

TEST_CASE("PeriodicTable Comparison") {
    PeriodicTable pt;
    PeriodicTable pt2;

    SECTION("Blank equivalence") {
        std::string msg = "PIMPL is not set";

        // Empty PeriodicTables; same object
        REQUIRE_THROWS_MATCHES(pt == pt, std::runtime_error, Message(msg));
        REQUIRE_THROWS_MATCHES(pt2 == pt2, std::runtime_error, Message(msg));

        // Empty PeriodicTables; different objects
        REQUIRE_THROWS_MATCHES(pt == pt2, std::runtime_error, Message(msg));
        REQUIRE_THROWS_MATCHES(pt2 == pt, std::runtime_error, Message(msg));
    }

    SECTION("Filled and blank") {
        std::string msg = "PIMPL is not set";

        load_elements(pt2);

        REQUIRE_THROWS_MATCHES(pt != pt2, std::runtime_error, Message(msg));
        REQUIRE_THROWS_MATCHES(pt2 != pt, std::runtime_error, Message(msg));
    }

    SECTION("Filled with same data") {
        load_elements(pt);
        load_elements(pt2);

        // Filled PeriodicTables; same object
        REQUIRE(pt == pt);
        REQUIRE(pt2 == pt2);

        // Filled PeriodicTables; different objects
        REQUIRE(pt == pt2);
        REQUIRE(pt2 == pt);
    }

    SECTION("Filled with different elements") {
        load_elements(pt);
        pt2.insert(0, Atom(0ul, 0.0, "Ez"));

        REQUIRE(pt != pt2);
        REQUIRE(pt2 != pt);
    }

    SECTION("Filled with same element; different isotopes") {
        pt.insert(1, Atom(1ul, 1837.4260218693814, "H"));
        pt.add_isotope(1, 1, Atom(1ul, 1837.1526472934618, "H"));

        pt2.insert(1, Atom(1ul, 1837.4260218693814, "H"));
        pt2.add_isotope(1, 2, Atom(1ul, 3671.4829413173247, "H"));

        REQUIRE(pt != pt2);
        REQUIRE(pt2 != pt);
    }

    SECTION("Filled with different density matrices") {
        load_elements(pt);
        load_elements(pt2);
        pt.add_atom_dm(3, {1.0, 1.0, 1.0, 1.0});

        REQUIRE(pt != pt2);
        REQUIRE(pt2 != pt);
    }
}

TEST_CASE("PeriodicTable::insert") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("Repeat elements") {
        // Element with no isotopes
        REQUIRE_THROWS_MATCHES(pt.insert(0, Atom(0ul, 0.0, "Ez")),
                               std::runtime_error,
                               Message("Element already exists with Z = 0"));
        // Element with isotopes
        REQUIRE_THROWS_MATCHES(pt.insert(1, Atom(1ul, 1837.4260218693814, "H")),
                               std::runtime_error,
                               Message("Element already exists with Z = 1"));
    }
}

TEST_CASE("PeriodicTable::add_isotope") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("Repeat isotopes") {
        REQUIRE_THROWS_MATCHES(
          pt.add_isotope(1, 1, Atom(1ul, 1837.1526472934618, "H")),
          std::runtime_error, Message("Isotope Z = 1, A = 1 already exists"));
    }

    SECTION("No element") {
        REQUIRE_THROWS_MATCHES(
          pt.add_isotope(3, 6, Atom(3ul, 10964.898253742283, "Li")),
          std::runtime_error, Message("Element does not exist with Z = 3"));
    }
}

TEST_CASE("PeriodicTable::sym_2_Z") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("No element") {
        REQUIRE_THROWS_MATCHES(pt.sym_2_Z("Li"), std::out_of_range,
                               Message("Unrecognized atomic symbol: Li"));
    }

    SECTION("Correct usage") {
        PeriodicTable::size_type corr = 2;

        REQUIRE(corr == pt.sym_2_Z("He"));

        // Ensure case insensitivity
        REQUIRE(corr == pt.sym_2_Z("HE"));
        REQUIRE(corr == pt.sym_2_Z("he"));
    }
}

TEST_CASE("PeriodicTable::isotopes") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("No element") {
        REQUIRE_THROWS_MATCHES(pt.isotopes(3), std::out_of_range,
                               Message("Element does not exist with Z = 3"));
    }

    SECTION("No isotopes") {
        auto corr = PeriodicTable::isotope_list();

        REQUIRE(corr == pt.isotopes(0));
    }

    SECTION("Isotopes exist") {
        auto corr = PeriodicTable::isotope_list{1, 2};

        REQUIRE(corr == pt.isotopes(1));
    }
}

TEST_CASE("PeriodicTable::get_atom") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("No element") {
        REQUIRE_THROWS_MATCHES(pt.get_atom(3), std::out_of_range,
                               Message("Element does not exist with Z = 3"));
    }

    SECTION("Element exists") {
        auto corr = Atom(0ul, 0.0, "Ez");

        REQUIRE(corr == pt.get_atom(0));
    }
}

TEST_CASE("PeriodicTable::get_atom_dm") {
    PeriodicTable pt;
    load_elements(pt);
    pt.add_atom_dm(1, {0.18269721, 0.28443345, 0.28443345,
                       0.44282224}); // add precalculated density matrix with
                                     // the basis set 6-31G for H
    pt.add_atom_dm(2, {0.70112023, 0.60816932, 0.60816932,
                       0.52754136}); // add precalculated density matrix with
                                     // the basis set 6-31G for He

    SECTION("No density matrix") {
        REQUIRE_THROWS_MATCHES(
          pt.get_atom_dm(3), std::out_of_range,
          Message("Density matrix does not exist for Z = 3"));
    }

    SECTION("Add existing density matrix") {
        REQUIRE_THROWS_MATCHES(
          pt.add_atom_dm(1, {1.0, 1.0, 1.0, 1.0}), std::runtime_error,
          Message("Atomic density matrix for Z = 1 already exists"));
    }

    SECTION("Density matrix exists 1") {
        PeriodicTable::atom_dm_t corr = {0.18269721, 0.28443345, 0.28443345,
                                         0.44282224};

        REQUIRE(corr == pt.get_atom_dm(1));
        REQUIRE(corr == pt.get_atom_dm("H"));
    }

    SECTION("Density matrix exists 2") {
        PeriodicTable::atom_dm_t corr = {0.70112023, 0.60816932, 0.60816932,
                                         0.52754136};

        REQUIRE(corr == pt.get_atom_dm(2));
        REQUIRE(corr == pt.get_atom_dm("He"));
    }

    SECTION("Density matrix exists without atom") {
        pt.add_atom_dm(42, {9.0, 18.0, 15.0});
        PeriodicTable::atom_dm_t corr = {9.0, 18.0, 15.0};

        REQUIRE(corr == pt.get_atom_dm(42));
        REQUIRE_THROWS_MATCHES(corr == pt.get_atom_dm("Mo"), std::out_of_range,
                               Message("Unrecognized atomic symbol: Mo"));
    }
}

TEST_CASE("PeriodicTable::get_isotope") {
    PeriodicTable pt;
    load_elements(pt);

    SECTION("No element") {
        REQUIRE_THROWS_MATCHES(pt.get_isotope(3, 6), std::out_of_range,
                               Message("Element does not exist with Z = 3"));
    }

    SECTION("Element exists, but no isotopes") {
        REQUIRE_THROWS_MATCHES(pt.get_isotope(0, 0), std::out_of_range,
                               Message("No isotope data for Z = 0, A = 0"));
    }

    SECTION("Element exists, but incorrect isotope") {
        REQUIRE_THROWS_MATCHES(pt.get_isotope(1, 10), std::out_of_range,
                               Message("No isotope data for Z = 1, A = 10"));
    }

    SECTION("Element exists, isotope exists") {
        auto corr = Atom(1ul, 1837.1526472934618, "H");

        REQUIRE(corr == pt.get_isotope(1, 1));
    }
}
