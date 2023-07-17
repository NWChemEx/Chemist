/*
 * Copyright 2023 NWChemEx-Project
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
#include <chemist/molecule/fragmented_molecule.hpp>

using namespace chemist;

using atom_type = Molecule::atom_type;

TEST_CASE("FragmentedMolecule") {
    // Make some atoms
    atom_type atom0{"H", 1, 1.0, 0.0, 0.0, 0.0};
    atom_type atom1{"H", 1, 1.0, 0.0, 0.1, 0.0};

    // Make a molecule filled with the atoms
    Molecule mol;
    mol.push_back(atom0);
    mol.push_back(atom1);

    // Fragment the molecule
    FragmentedNuclei frags(mol.nuclei());
    frags.add_fragment({0});
    frags.add_fragment({1});

    FragmentedMolecule defaulted;
    FragmentedMolecule has_frags(frags);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("fragments") {
            REQUIRE(has_frags.size() == 2);
            REQUIRE(has_frags.supersystem() == mol);
            REQUIRE(has_frags.charge(0) == 0);
            REQUIRE(has_frags.charge(1) == 0);
            REQUIRE(has_frags.multiplicity(0) == 1);
            REQUIRE(has_frags.multiplicity(1) == 1);
        }
        SECTION("fragments, charge") {
            FragmentedMolecule has_charge(frags, -2);
            REQUIRE(has_charge.size() == 2);
            mol.set_charge(-2);
            REQUIRE(has_charge.supersystem() == mol);
        }
        SECTION("fragments, charge, multiplicity") {
            FragmentedMolecule has_mult(frags, 1, 2);
            REQUIRE(has_mult.size() == 2);
            mol.set_charge(1);
            mol.set_multiplicity(2);
            REQUIRE(has_mult.supersystem() == mol);
        }

        SECTION("Copy") {
            FragmentedMolecule other_default(defaulted);
            REQUIRE(defaulted == other_default);

            FragmentedMolecule other_frags(has_frags);
            REQUIRE(has_frags == other_frags);
        }

        SECTION("Move") {
            FragmentedMolecule other_default(defaulted);
            FragmentedMolecule moved_default(std::move(defaulted));
            REQUIRE(moved_default == other_default);

            FragmentedMolecule other_frags(has_frags);
            FragmentedMolecule moved_frags(std::move(has_frags));
            REQUIRE(moved_frags == other_frags);
        }

        SECTION("Copy Assignment") {
            FragmentedMolecule other_default;
            auto pother_default = &(other_default = defaulted);
            REQUIRE(defaulted == other_default);
            REQUIRE(pother_default == &other_default);

            FragmentedMolecule other_frags;
            auto pother_frags = &(other_frags = has_frags);
            REQUIRE(has_frags == other_frags);
            REQUIRE(pother_frags == &other_frags);
        }

        SECTION("Move Assignment") {
            FragmentedMolecule other_default(defaulted);
            FragmentedMolecule moved_default;
            auto pmoved_default = &(moved_default = std::move(defaulted));
            REQUIRE(moved_default == other_default);
            REQUIRE(pmoved_default == &moved_default);

            FragmentedMolecule other_frags(has_frags);
            FragmentedMolecule moved_frags;
            auto pmoved_frags = &(moved_frags = std::move(has_frags));
            REQUIRE(moved_frags == other_frags);
            REQUIRE(pmoved_frags == &moved_frags);
        }
    }

    SECTION("supersystem") {
        REQUIRE_THROWS_AS(defaulted.supersystem(), std::runtime_error);
        REQUIRE(has_frags.supersystem() == mol);

        // Honors charge/mult
        FragmentedMolecule other(frags, 1, 2);
        mol.set_charge(1);
        mol.set_multiplicity(2);
        REQUIRE(other.supersystem() == mol);
    }

    SECTION("charge") {
        REQUIRE_THROWS_AS(defaulted.charge(0), std::out_of_range);

        REQUIRE(has_frags.charge(0) == 0);
        REQUIRE(has_frags.charge(1) == 0);

        // Is mutable?
        has_frags.charge(0) = 2;
        REQUIRE(has_frags.charge(0) == 2);
    }

    SECTION("charge() const") {
        REQUIRE_THROWS_AS(std::as_const(defaulted).charge(0),
                          std::out_of_range);

        REQUIRE(std::as_const(has_frags).charge(0) == 0);
        REQUIRE(std::as_const(has_frags).charge(1) == 0);
    }

    SECTION("multiplicity") {
        REQUIRE_THROWS_AS(defaulted.multiplicity(0), std::out_of_range);

        REQUIRE(has_frags.multiplicity(0) == 1);
        REQUIRE(has_frags.multiplicity(1) == 1);

        // Is mutable?
        has_frags.multiplicity(0) = 2;
        REQUIRE(has_frags.multiplicity(0) == 2);
    }

    SECTION("multiplicity() const") {
        REQUIRE_THROWS_AS(std::as_const(defaulted).multiplicity(0),
                          std::out_of_range);

        REQUIRE(std::as_const(has_frags).multiplicity(0) == 1);
        REQUIRE(std::as_const(has_frags).multiplicity(1) == 1);
    }

    SECTION("const_fragmented_nuclei_reference const") {
        const FragmentedNuclei test(frags);
        REQUIRE(has_frags.fragmented_nuclei() == test);
    }

    SECTION("at_") {
        // N.B. This is tested through operator[], which relies on at_

        Molecule corr0(0, 1, {atom0});
        Molecule corr1(0, 1, {atom1});
        REQUIRE(has_frags[0] == corr0);
        REQUIRE(has_frags[1] == corr1);

        // Honors charge/multiplicity
        has_frags.charge(0)       = -1;
        has_frags.multiplicity(0) = 3;
        corr0.set_charge(-1);
        corr0.set_multiplicity(3);
        REQUIRE(has_frags[0] == corr0);
    }

    SECTION("at_ const") {
        // N.B. This is tested through operator[] const, which relies on
        // at_ const
        Molecule corr0(0, 1, {atom0});
        Molecule corr1(0, 1, {atom1});
        REQUIRE(std::as_const(has_frags)[0] == corr0);
        REQUIRE(std::as_const(has_frags)[1] == corr1);

        // Honors charge/multiplicity
        has_frags.charge(0)       = -1;
        has_frags.multiplicity(0) = 3;
        corr0.set_charge(-1);
        corr0.set_multiplicity(3);
        REQUIRE(std::as_const(has_frags)[0] == corr0);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(has_frags.size() == 2);
    }

    SECTION("comparisons") {
        // Default v default
        FragmentedMolecule other_default;
        REQUIRE(defaulted == other_default);
        REQUIRE_FALSE(defaulted != other_default);

        // Default vs non-default
        REQUIRE(defaulted != has_frags);
        REQUIRE_FALSE(defaulted == has_frags);

        // Same state.
        FragmentedMolecule other_frags(frags);
        REQUIRE(has_frags == other_frags);

        // Different frags
        FragmentedMolecule diff_frags(FragmentedNuclei{});
        REQUIRE(has_frags != diff_frags);
        REQUIRE_FALSE(has_frags == diff_frags);

        // Different charge
        FragmentedMolecule diff_charge(has_frags);
        diff_charge.charge(0) = 1;
        REQUIRE(has_frags != diff_charge);
        REQUIRE_FALSE(has_frags == diff_charge);

        // Different multiplicity
        FragmentedMolecule diff_multiplicity(has_frags);
        diff_multiplicity.multiplicity(0) = 2;
        REQUIRE(has_frags != diff_multiplicity);
        REQUIRE_FALSE(has_frags == diff_multiplicity);
    }
}
