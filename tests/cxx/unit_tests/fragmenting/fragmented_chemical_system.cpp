/*
 * Copyright 2024 NWChemEx-Project
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

#include "../test_helpers.hpp"
#include <chemist/fragmenting/fragmented_chemical_system.hpp>

using namespace chemist;
using namespace chemist::fragmenting;

using types2check = std::tuple<ChemicalSystem, const ChemicalSystem>;

TEMPLATE_LIST_TEST_CASE("FragmentedChemicalSystem", "", types2check) {
    using class_type           = FragmentedChemicalSystem<TestType>;
    using chemical_system_type = typename class_type::supersystem_type;
    using fragmented_molecule_type =
      typename class_type::fragmented_molecule_type;
    using frag_nuclei_type =
      typename fragmented_molecule_type::fragmented_nuclei_type;
    using molecule_type = typename fragmented_molecule_type::supersystem_type;
    using atom_type     = typename molecule_type::atom_type;
    using fragment_reference = typename class_type::reference;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);
    molecule_type heh{he, h};
    frag_nuclei_type nuclei_frags(heh.nuclei().as_nuclei());
    nuclei_frags.insert({0});
    nuclei_frags.insert({1});
    fragmented_molecule_type empty_frags;
    fragmented_molecule_type value_frags(nuclei_frags, 0, 2);

    chemical_system_type empty_cs;
    chemical_system_type value_cs(heh);
    fragment_reference frag0(value_frags[0]);
    fragment_reference frag1(value_frags[1]);

    class_type defaulted;
    class_type empty(empty_frags);
    class_type value(value_frags);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.supersystem() == empty_cs);
        }

        SECTION("Value") {
            REQUIRE(empty.size() == 0);
            REQUIRE(empty.supersystem() == empty_cs);

            REQUIRE(value.size() == 2);
            REQUIRE(value.supersystem() == value_cs);
            REQUIRE(value[0] == frag0);
            REQUIRE(value[1] == frag1);
        }

        test_chemist::test_copy_and_move(defaulted, empty, value);
    }

    SECTION("fragmented_molecule()") {
        REQUIRE_THROWS_AS(defaulted.fragmented_molecule(), std::runtime_error);
        REQUIRE(value.fragmented_molecule() == value_frags);
    }

    SECTION("fragmented_molecule() const") {
        const auto& const_defaulted = std::as_const(defaulted);
        using error_t               = std::runtime_error;
        REQUIRE_THROWS_AS(const_defaulted.fragmented_molecule(), error_t);
        REQUIRE(std::as_const(value).fragmented_molecule() == value_frags);
    }

    SECTION("supersystem()") {
        REQUIRE(defaulted.supersystem() == empty_cs);
        REQUIRE(empty.supersystem() == empty_cs);
        REQUIRE(value.supersystem() == value_cs);
    }

    SECTION("supersystem() const") {
        REQUIRE(std::as_const(defaulted).supersystem() == empty_cs);
        REQUIRE(std::as_const(empty).supersystem() == empty_cs);
        REQUIRE(std::as_const(value).supersystem() == value_cs);
    }

    SECTION("operator[]()") {
        REQUIRE(value[0] == frag0);
        REQUIRE(value[1] == frag1);
    }

    SECTION("operator[]() const") {
        REQUIRE(std::as_const(value)[0] == frag0);
        REQUIRE(std::as_const(value)[1] == frag1);
    }

    SECTION("swap") {
        class_type lhs_copy(defaulted);
        class_type rhs_copy(value);

        defaulted.swap(value);

        REQUIRE(defaulted == rhs_copy);
        REQUIRE(value == lhs_copy);
    }

    SECTION("operator==") {
        SECTION("Default vs default") {
            class_type other;
            REQUIRE(defaulted == other);
        }

        SECTION("Default vs empty") { REQUIRE(defaulted == empty); }

        SECTION("Default vs value") { REQUIRE_FALSE(defaulted == value); }

        SECTION("Empty vs empty") {
            class_type other(empty_frags);
            REQUIRE(empty == other);
        }

        SECTION("Empty vs value") { REQUIRE_FALSE(empty == value); }

        SECTION("Value vs same value") {
            class_type other(value_frags);
            REQUIRE(value == other);
        }

        SECTION("Different supersystem") {
            atom_type li("Li", 3ul, 7.0, 4.0, 5.0, 6.0);
            molecule_type hehli{he, h, li};
            frag_nuclei_type other_frags(hehli.nuclei().as_nuclei());
            other_frags.insert({0});
            other_frags.insert({1});

            class_type other(fragmented_molecule_type(other_frags, 0, 1));
            REQUIRE_FALSE(value == other);
        }

        SECTION("Different fragments") {
            frag_nuclei_type other_frags(heh.nuclei().as_nuclei());
            other_frags.insert({0});

            class_type other(fragmented_molecule_type(other_frags, 0, 2));
            REQUIRE_FALSE(value == other);
        }
    }

    SECTION("operator!=") {
        // N.b. method is implemented in terms of operator==, so just spot
        // check

        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != empty);
    }
}
