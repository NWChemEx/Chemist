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

#include "../../../test_helpers.hpp"
#include <chemist/chemical_system/molecule/molecule_view/molecule_view.hpp>

using namespace chemist;

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("MoleculeView", "", types2test) {
    constexpr bool is_const_v =
      !std::is_same_v<TestType, std::remove_cv_t<TestType>>;
    using view_type              = MoleculeView<TestType>;
    using molecule_type          = typename view_type::molecule_type;
    using atom_type              = typename molecule_type::atom_type;
    using charge_type            = typename view_type::charge_type;
    using multiplicity_type      = typename view_type::multiplicity_type;
    using nuclei_reference       = typename view_type::nuclei_reference;
    using const_nuclei_reference = typename view_type::const_nuclei_reference;

    atom_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    atom_type he("He", 2ul, 4.0, 1.0, 2.0, 3.0);
    multiplicity_type m1{1}, m2{2};
    charge_type qm1{-1}, q0{0};
    molecule_type value_mol{h, he};

    view_type defaulted;
    view_type value(value_mol);

    SECTION("CTors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.nuclei() == nuclei_reference{});
            REQUIRE(defaulted.charge() == q0);
            REQUIRE(defaulted.multiplicity() == m1);
        }

        SECTION("From Molecule") {
            REQUIRE(value.size() == 2);
            REQUIRE(value[0] == value_mol[0]);
            REQUIRE(value[1] == value_mol[1]);
            REQUIRE(value.charge() == q0);
            REQUIRE(value.multiplicity() == m2);
        }

        SECTION("From pointers") {
            view_type value2(value_mol.nuclei(), &qm1, &m1);
            REQUIRE(value2.size() == 2);
            REQUIRE(value2[0] == value_mol[0]);
            REQUIRE(value2[1] == value_mol[1]);
            REQUIRE(value2.charge() == qm1);
            REQUIRE(value2.multiplicity() == m1);
        }

        SECTION("From mutable to read-only") {
            if constexpr(std::is_same_v<TestType, const Molecule>) {
                MoleculeView<Molecule> non_const(value_mol);
                view_type read_only(non_const);
                REQUIRE(read_only.size() == 2);
                REQUIRE(read_only[0] == value_mol[0]);
                REQUIRE(read_only[1] == value_mol[1]);
                REQUIRE(read_only.charge() == q0);
                REQUIRE(read_only.multiplicity() == m2);
            }
        }

        SECTION("Copy") {
            test_chemist::test_copy_ctor(defaulted);
            test_chemist::test_copy_ctor(value);
        }

        SECTION("Move") {
            test_chemist::test_move_ctor(defaulted);
            test_chemist::test_move_ctor(value);
        }

        SECTION("Assign from Molecule") {
            if constexpr(!is_const_v) {
                molecule_type value_mol2{he, h}; // Swap order
                auto pvalue = &(value = value_mol2);
                REQUIRE(pvalue == &value);
                REQUIRE(value[0] == he.nucleus());
                REQUIRE(value[1] == h.nucleus());
                REQUIRE(value_mol == value_mol2);

                REQUIRE_THROWS_AS(defaulted = value_mol, std::runtime_error);
            }
        }

        SECTION("Copy assignment") {
            test_chemist::test_copy_assignment(defaulted);
            test_chemist::test_copy_assignment(value);
        }

        SECTION("Move assignment") {
            test_chemist::test_move_assignment(defaulted);
            test_chemist::test_move_assignment(value);
        }
    }

    SECTION("nuclei()") {
        REQUIRE(defaulted.nuclei() == nuclei_reference{});
        REQUIRE(value.nuclei() == value_mol.nuclei());
    }

    SECTION("nuclei() const") {
        REQUIRE(std::as_const(defaulted).nuclei() == const_nuclei_reference{});
        REQUIRE(std::as_const(value).nuclei() == value_mol.nuclei());
    }

    SECTION("n_electrons") {
        REQUIRE(defaulted.n_electrons() == 0);
        REQUIRE(value.n_electrons() == 3);
    }

    SECTION("charge") {
        REQUIRE(defaulted.charge() == q0);
        REQUIRE(value.charge() == q0);
    }

    SECTION("set_charge") {
        if constexpr(std::is_same_v<TestType, std::remove_cv_t<TestType>>) {
            charge_type q2{2}, q6{6}, q3{3};
            // Can't set empty to non-zero charge
            REQUIRE_THROWS_AS(defaulted.set_charge(q2), std::runtime_error);

            // Can set empty to zero charge
            defaulted.set_charge(q0);
            REQUIRE(defaulted.charge() == q0);

            // Can't set charge to more than number of electrons
            REQUIRE_THROWS_AS(value.set_charge(q6), std::runtime_error);

            // Can set it to number of electrons
            value.set_charge(q3);
            REQUIRE(value.charge() == q3);
        }
    }

    SECTION("charge_data() const") {
        REQUIRE(std::as_const(defaulted).charge_data() == nullptr);
        REQUIRE(std::as_const(value).charge_data() == value_mol.charge_data());
    }

    SECTION("multiplicity") {
        REQUIRE(defaulted.multiplicity() == m1);
        REQUIRE(value.multiplicity() == m2);
    }

    SECTION("set_multiplicity") {
        if constexpr(std::is_same_v<TestType, std::remove_cv_t<TestType>>) {
            // Can't set empty to multiplicity different than 1
            REQUIRE_THROWS_AS(defaulted.set_multiplicity(m2),
                              std::runtime_error);

            // Can set empty to singlet
            defaulted.set_multiplicity(m1);
            REQUIRE(defaulted.multiplicity() == m1);

            // Can't set odd number of electrons to a single
            REQUIRE_THROWS_AS(value.set_multiplicity(m1), std::runtime_error);

            // Can set it to something else
            value.set_multiplicity(multiplicity_type{4});
            REQUIRE(value.multiplicity() == multiplicity_type{4});
        }
    }

    SECTION("multiplicity_data() const") {
        REQUIRE(std::as_const(defaulted).multiplicity_data() == nullptr);
        const auto& cvalue = value;
        REQUIRE(cvalue.multiplicity_data() == value_mol.multiplicity_data());
    }

    SECTION("as_molecule") {
        REQUIRE(defaulted.as_molecule() == molecule_type{});
        REQUIRE(value.as_molecule() == value_mol);
    }

    SECTION("swap") {
        view_type lhs_copy(defaulted);
        view_type rhs_copy(value);

        defaulted.swap(value);
        REQUIRE(defaulted == rhs_copy);
        REQUIRE(value == lhs_copy);
    }

    SECTION("operator==") {
        SECTION("empty vs. empty") {
            view_type other;
            REQUIRE(defaulted == other);
        }

        SECTION("empty vs non-empty") { REQUIRE_FALSE(defaulted == value); }

        SECTION("Same value") {
            view_type other(value_mol);
            REQUIRE(value == other);
        }

        SECTION("Different nuclei") {
            molecule_type other_mol{h};
            view_type other(other_mol.nuclei(), &q0, &m2);
            REQUIRE_FALSE(value == other);
        }

        SECTION("Different charge") {
            charge_type qm2{-2};
            view_type other(value_mol.nuclei(), &qm2, &m2);
            REQUIRE_FALSE(value == other);
        }

        SECTION("Different multiplicity") {
            multiplicity_type m3{3};
            molecule_type other_mol{he};
            view_type lhs(other_mol.nuclei(), &q0, &m1);
            view_type rhs(other_mol.nuclei(), &q0, &m3);
            REQUIRE_FALSE(lhs == rhs);
        }
    }

    SECTION("operator!=") {
        // Just negates operator== so spot check
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != view_type{});
    }

    SECTION("MoleculeView vs Molecule") {
        REQUIRE(value == value_mol);
        REQUIRE(value_mol == value); // Ensure symmetric
        REQUIRE(value != molecule_type{});
        REQUIRE(molecule_type{} != value);
    }

    SECTION("MoleculeView<Molecule> vs. MoleculeView<const Molecule>") {
        if constexpr(std::is_same_v<Molecule, TestType>) {
            MoleculeView<const TestType> rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE(rhs == defaulted);
            REQUIRE(value != rhs);
            REQUIRE(rhs != value);
        }
    }
}
