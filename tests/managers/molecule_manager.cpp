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

#include "chemist/managers/molecule_manager.hpp"
#include <catch2/catch.hpp>
#include <string>

using namespace chemist;

using Catch::Matchers::Message;

void load_elements(PeriodicTable& pt);
void load_molecules(MoleculeManager& mm, const PeriodicTable& pt);
void load_molecules_2(MoleculeManager& mm, const PeriodicTable& pt);

TEST_CASE("MoleculeManager Copy/Move") {
    PeriodicTable pt;
    MoleculeManager mm;

    load_elements(pt);
    load_molecules(mm, pt);

    SECTION("Copy Ctor") {
        MoleculeManager mm2(mm);

        REQUIRE(mm == mm2);
    }

    SECTION("Move Ctor") {
        MoleculeManager mm2(std::move(mm));
        MoleculeManager corr;
        std::string msg = "PIMPL is not set";

        load_molecules(corr, pt);

        // Compare to copy with same data values
        REQUIRE(mm2 == corr);

        // mm should no longer have a PIMPL, and will throw
        REQUIRE_THROWS_MATCHES(mm == mm2, std::runtime_error, Message(msg));
    }

    SECTION("Copy Assignment") {
        MoleculeManager mm2;

        mm2 = mm;

        REQUIRE(mm == mm2);
    }

    SECTION("Move Assignment") {
        MoleculeManager mm2;
        MoleculeManager corr;
        std::string msg = "PIMPL is not set";

        load_molecules(corr, pt);

        mm2 = std::move(mm);

        // Compare to copy with same data values
        REQUIRE(mm2 == corr);

        // bsm should no longer have a PIMPL, and will throw
        REQUIRE_THROWS_MATCHES(mm == mm2, std::runtime_error, Message(msg));
    }
}

TEST_CASE("MoleculeManager Comparison") {
    PeriodicTable pt;
    MoleculeManager mm;
    MoleculeManager mm2;

    load_elements(pt);

    SECTION("Blank equivalence") {
        std::string msg = "PIMPL is not set";

        // Empty MoleculeManagers; same object
        REQUIRE_THROWS_MATCHES(mm == mm, std::runtime_error, Message(msg));
        REQUIRE_THROWS_MATCHES(mm2 == mm2, std::runtime_error, Message(msg));

        // Empty MoleculeManagers; different objects
        REQUIRE_THROWS_MATCHES(mm == mm2, std::runtime_error, Message(msg));
        REQUIRE_THROWS_MATCHES(mm2 == mm, std::runtime_error, Message(msg));
    }

    SECTION("Filled and blank") {
        std::string msg = "PIMPL is not set";

        load_molecules(mm2, pt);

        REQUIRE_THROWS_MATCHES(mm != mm2, std::runtime_error, Message(msg));
        REQUIRE_THROWS_MATCHES(mm2 != mm, std::runtime_error, Message(msg));
    }

    SECTION("Filled with same data") {
        load_molecules(mm, pt);
        load_molecules(mm2, pt);

        // Filled MoleculeManagers; same object
        REQUIRE(mm == mm);
        REQUIRE(mm2 == mm2);

        // Filled MoleculeManagers; different objects
        REQUIRE(mm == mm2);
        REQUIRE(mm2 == mm);
    }

    SECTION("Filled with different data") {
        load_molecules(mm, pt);
        load_molecules_2(mm2, pt);

        REQUIRE(mm != mm2);
        REQUIRE(mm2 != mm);
    }
}

TEST_CASE("MoleculeManager::insert") {
    PeriodicTable pt;
    MoleculeManager mm;

    load_elements(pt);

    SECTION("Repeat molecule") {
        load_molecules(mm, pt);
        std::string msg = "Molecule already exists named ez";

        auto ez = Molecule();
        ez.push_back(Atom{"H", 1ul, pt.get_atom(0).mass(), 1.0, 1.0, 1.0});

        REQUIRE_THROWS_MATCHES(mm.insert("ez", ez), std::runtime_error,
                               Message(msg));
    }
}

TEST_CASE("MoleculeManager::at") {
    PeriodicTable pt;
    MoleculeManager mm;

    load_elements(pt);

    SECTION("Empty MoleculeManager") {
        std::string msg = "PIMPL is not set";

        REQUIRE_THROWS_MATCHES(mm.at("ez"), std::runtime_error, Message(msg));
    }

    SECTION("Nonexistant Molecule") {
        load_molecules(mm, pt);
        std::string msg = "No molecule named nonexistant";

        REQUIRE_THROWS_MATCHES(mm.at("nonexistant"), std::out_of_range,
                               Message(msg));
    }

    SECTION("Molecule exists") {
        load_molecules(mm, pt);
        auto corr = Molecule();
        corr.push_back(
          Atom{Atom{"", 1ul, pt.get_atom(0).mass(), 0.0, 0.0, 0.0}});

        REQUIRE(corr == mm.at("ez"));
    }
}

inline void load_elements(PeriodicTable& pt) {
    pt.insert(0, Atom("Ez", 0ul, 0.0, 0.0, 0.0, 0.0));

    pt.insert(1, Atom("H", 1ul, 1837.4260218693814, 0.0, 0.0, 0.0));
    pt.add_isotope(1, 1, Atom("H", 1ul, 1837.1526472934618, 0.0, 0.0, 0.0));
    pt.add_isotope(1, 2, Atom("H", 1ul, 3671.4829413173247, 0.0, 0.0, 0.0));

    pt.insert(2, Atom("He", 2ul, 7296.297100609073, 0.0, 0.0, 0.0));
    pt.add_isotope(2, 3, Atom("He", 2ul, 5497.885121445487, 0.0, 0.0, 0.0));
    pt.add_isotope(2, 4, Atom("He", 2ul, 7296.299386693523, 0.0, 0.0, 0.0));
}

inline void load_molecules(MoleculeManager& mm, const PeriodicTable& pt) {
    auto ez = Molecule();
    ez.push_back(Atom{"", 1ul, pt.get_atom(0).mass(), 0.0, 0.0, 0.0});

    mm.insert("ez", ez);

    auto test_molecule = Molecule();
    test_molecule.push_back(
      Atom{"", 1ul, pt.get_atom(1).mass(), 0.5, 0.5, 0.5});
    test_molecule.push_back(
      Atom{"", 1ul, pt.get_atom(1).mass(), -0.5, -0.5, -0.5});

    mm.insert("test_molecule", test_molecule);
}

inline void load_molecules_2(MoleculeManager& mm, const PeriodicTable& pt) {
    auto ez = Molecule();
    ez.push_back(Atom{"", 1ul, pt.get_atom(0).mass(), 1.0, 1.0, 1.0});

    mm.insert("ez", ez);

    auto test_molecule = Molecule();
    test_molecule.push_back(
      Atom{"", 1ul, pt.get_atom(2).mass(), 0.5, 0.5, 0.5});
    test_molecule.push_back(
      Atom{"", 1ul, pt.get_atom(2).mass(), -0.5, -0.5, -0.5});

    mm.insert("test_molecule", test_molecule);
}