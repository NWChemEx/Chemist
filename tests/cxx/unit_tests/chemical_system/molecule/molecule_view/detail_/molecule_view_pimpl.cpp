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

#include "../../../../test_helpers.hpp"
#include <chemist/chemical_system/molecule/molecule_view/detail_/molecule_view_pimpl.hpp>

using namespace chemist;

using types2test = std::tuple<Molecule, const Molecule>;

TEMPLATE_LIST_TEST_CASE("MoleculeViewPIMPL", "", types2test) {
    using pimpl_type       = detail_::MoleculeViewPIMPL<TestType>;
    using parent_type      = typename pimpl_type::parent_type;
    using molecule_type    = typename parent_type::molecule_type;
    using nuclei_reference = typename pimpl_type::nuclei_reference;
    using nucleus_type     = typename nuclei_reference::value_type;
    using nuclei_type = typename nuclei_reference::nuclei_traits::value_type;
    using multiplicity_type = typename parent_type::multiplicity_type;
    using charge_type       = typename parent_type::charge_type;

    nucleus_type h("H", 1ul, 1.0, 0.0, 0.0, 0.0);
    nucleus_type he("He", 2ul, 4.0, 0.0, 1.0, 2.0);
    nucleus_type li("Li", 3ul, 7.0, 3.0, 4.0, 5.0);

    nuclei_type values_set{h, he, li};
    nuclei_type cation_set{h};
    multiplicity_type m1{1}, m3{3};
    charge_type anion_q{-1}, neutral_q{0}, cation_q{1};

    molecule_type empty_mol;
    molecule_type values_mol(0, m1, values_set);
    molecule_type anion_mol(anion_q, m3, values_set);
    molecule_type cation_mol(cation_q, m1, cation_set);

    pimpl_type empty(empty_mol.nuclei(), nullptr, nullptr);
    pimpl_type values(values_mol.nuclei(), values_mol.charge_data(),
                      values_mol.multiplicity_data());
    pimpl_type anion(anion_mol.nuclei(), anion_mol.charge_data(),
                     anion_mol.multiplicity_data());
    pimpl_type cation(cation_mol.nuclei(), cation_mol.charge_data(),
                      cation_mol.multiplicity_data());

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(empty.nuclei() == empty_mol.nuclei());
            REQUIRE(empty.charge_data() == nullptr);
            REQUIRE(empty.multiplicity_data() == nullptr);
        }
        SECTION("Value") {
            REQUIRE(values.nuclei() == values_mol.nuclei());
            REQUIRE(values.charge_data() == values_mol.charge_data());
            REQUIRE(values.multiplicity_data() ==
                    values_mol.multiplicity_data());

            REQUIRE(anion.nuclei() == anion_mol.nuclei());
            REQUIRE(anion.charge_data() == anion_mol.charge_data());
            REQUIRE(anion.multiplicity_data() == anion_mol.multiplicity_data());

            REQUIRE(cation.nuclei() == cation_mol.nuclei());
            REQUIRE(cation.charge_data() == cation_mol.charge_data());
            REQUIRE(cation.multiplicity_data() ==
                    cation_mol.multiplicity_data());
        }
        SECTION("Copy") {
            test_chemist::test_copy_ctor(empty);
            test_chemist::test_copy_ctor(values);
            test_chemist::test_copy_ctor(anion);
            test_chemist::test_copy_ctor(cation);
        }
    }

    SECTION("nuclei()") {
        REQUIRE(empty.nuclei() == empty_mol.nuclei());
        REQUIRE(values.nuclei() == values_mol.nuclei());
        REQUIRE(anion.nuclei() == anion_mol.nuclei());
        REQUIRE(cation.nuclei() == cation_mol.nuclei());
    }

    SECTION("nuclei() const") {
        REQUIRE(std::as_const(empty).nuclei() == empty_mol.nuclei());
        REQUIRE(std::as_const(values).nuclei() == values_mol.nuclei());
        REQUIRE(std::as_const(anion).nuclei() == anion_mol.nuclei());
        REQUIRE(std::as_const(cation).nuclei() == cation_mol.nuclei());
    }

    SECTION("charge()") {
        REQUIRE(values.charge() == neutral_q);
        REQUIRE(&values.charge() == values_mol.charge_data());

        REQUIRE(anion.charge() == anion_q);
        REQUIRE(&anion.charge() == anion_mol.charge_data());

        REQUIRE(cation.charge() == cation_q);
        REQUIRE(&cation.charge() == cation_mol.charge_data());
    }

    SECTION("charge() const") {
        REQUIRE(std::as_const(values).charge() == neutral_q);
        REQUIRE(&std::as_const(values).charge() == values_mol.charge_data());

        REQUIRE(std::as_const(anion).charge() == anion_q);
        REQUIRE(&std::as_const(anion).charge() == anion_mol.charge_data());

        REQUIRE(std::as_const(cation).charge() == cation_q);
        REQUIRE(&std::as_const(cation).charge() == cation_mol.charge_data());
    }

    SECTION("multiplicity()") {
        REQUIRE(values.multiplicity() == m1);
        REQUIRE(&values.multiplicity() == values_mol.multiplicity_data());

        REQUIRE(anion.multiplicity() == m3);
        REQUIRE(&anion.multiplicity() == anion_mol.multiplicity_data());

        REQUIRE(cation.multiplicity() == m1);
        REQUIRE(&cation.multiplicity() == cation_mol.multiplicity_data());
    }

    SECTION("multiplicity() const") {
        const auto& cvalues = values;
        REQUIRE(cvalues.multiplicity() == m1);
        REQUIRE(&cvalues.multiplicity() == values_mol.multiplicity_data());

        const auto& canion = anion;
        REQUIRE(canion.multiplicity() == m3);
        REQUIRE(&canion.multiplicity() == anion_mol.multiplicity_data());

        const auto& ccation = cation;
        REQUIRE(ccation.multiplicity() == m1);
        REQUIRE(&ccation.multiplicity() == cation_mol.multiplicity_data());
    }

    SECTION("charge_data()") {
        REQUIRE(empty.charge_data() == nullptr);
        REQUIRE(values.charge_data() == values_mol.charge_data());
        REQUIRE(anion.charge_data() == anion_mol.charge_data());
        REQUIRE(cation.charge_data() == cation_mol.charge_data());
    }

    SECTION("charge_data() const") {
        REQUIRE(std::as_const(empty).charge_data() == nullptr);
        REQUIRE(std::as_const(values).charge_data() ==
                values_mol.charge_data());
        REQUIRE(std::as_const(anion).charge_data() == anion_mol.charge_data());
        REQUIRE(std::as_const(cation).charge_data() ==
                cation_mol.charge_data());
    }

    SECTION("multiplicity_data()") {
        REQUIRE(empty.multiplicity_data() == nullptr);
        REQUIRE(values.multiplicity_data() == values_mol.multiplicity_data());
        REQUIRE(anion.multiplicity_data() == anion_mol.multiplicity_data());
        REQUIRE(cation.multiplicity_data() == cation_mol.multiplicity_data());
    }

    SECTION("multiplicity_data() const") {
        REQUIRE(std::as_const(empty).multiplicity_data() == nullptr);
        REQUIRE(std::as_const(values).multiplicity_data() ==
                values_mol.multiplicity_data());
        REQUIRE(std::as_const(anion).multiplicity_data() ==
                anion_mol.multiplicity_data());
        REQUIRE(std::as_const(cation).multiplicity_data() ==
                cation_mol.multiplicity_data());
    }

    SECTION("clone") {
        auto pempty = empty.clone();
        REQUIRE(*pempty == empty);

        auto pvalues = values.clone();
        REQUIRE(*pvalues == values);
    }

    SECTION("operator==") {
        SECTION("empty vs. empty") {
            pimpl_type other(empty_mol.nuclei(), nullptr, nullptr);
            REQUIRE(empty == other);
        }

        SECTION("empty vs. non-empty") { REQUIRE_FALSE(empty == values); }

        SECTION("same non-empty value") {
            pimpl_type other(anion_mol.nuclei(), &anion_q, &m3);
            REQUIRE(other == anion);
        }

        SECTION("different nuclei") {
            pimpl_type other(values_mol.nuclei(), &cation_q, &m1);
            REQUIRE_FALSE(cation == other);
        }

        SECTION("different charge") {
            pimpl_type other(anion_mol.nuclei(), &cation_q, &m3);
            REQUIRE_FALSE(anion == other);
        }

        SECTION("different multiplicity") {
            pimpl_type other(values_mol.nuclei(), &neutral_q, &m3);
            REQUIRE_FALSE(values == other);
        }
    }
}
