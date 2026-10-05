/*
 * Copyright 2026 NWChemEx-Project
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

// Only what MolecularBasisSet itself implements is tested here: its ctors,
// assignment, adding atoms, swap, and serialization. Everything it inherits
// from MolecularBasisSetCommon is tested in molecular_basis_set_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <cereal/archives/binary.hpp>
#include <chemist/experimental/basis_set/molecular_basis_set_class.hpp>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::MolecularBasisSet") {
    std::vector<double> s_cs{0.5, 0.25}, s_es{3.0, 0.5};
    std::vector<double> p_cs{1.0}, p_es{0.8};
    Point r_o(0.0, 0.0, 0.0), r_h(0.0, 0.0, 1.8);

    AtomicBasisSet o("cc-pVDZ", 8, r_o, ShellPurity::cartesian);
    o.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
    o.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());
    AtomicBasisSet h("STO-3G", 1, r_h, ShellPurity::pure);
    h.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());

    MolecularBasisSet mbs;
    mbs.push_back(o);
    mbs.push_back(h);

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            MolecularBasisSet defaulted;
            REQUIRE(defaulted.empty());
            REQUIRE(defaulted.n_shells() == 0);
            REQUIRE(defaulted.n_primitives() == 0);
            REQUIRE(defaulted.get_centers().empty());
        }

        SECTION("Initializer list") {
            MolecularBasisSet il{o, h};
            REQUIRE(il == mbs);
        }

        SECTION("Iterator range") {
            std::vector<AtomicBasisSet> atoms{o, h};
            MolecularBasisSet range(atoms.begin(), atoms.end());
            REQUIRE(range == mbs);

            // Including over the views another set hands out
            MolecularBasisSet from_views(mbs.begin(), mbs.end());
            REQUIRE(from_views == mbs);
        }

        SECTION("Copy is deep") {
            MolecularBasisSet copy(mbs);
            REQUIRE(copy == mbs);
            copy.get_centers()[0] = Point(9.0, 9.0, 9.0);
            copy.primitive(0).set_coefficient(42.0);
            copy[1].set_name("6-31G");
            REQUIRE(mbs[0].get_center() == r_o);
            REQUIRE(as_double(mbs.primitive(0).get_coefficient()) == 0.5);
            REQUIRE(mbs[1].get_name() == "STO-3G");
        }

        SECTION("Move") {
            MolecularBasisSet copy(mbs);
            MolecularBasisSet moved(std::move(copy));
            REQUIRE(moved == mbs);
        }

        SECTION("Copy assignment") {
            MolecularBasisSet copy;
            auto pcopy = &(copy = mbs);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy == mbs);
            copy[0].set_name("STO-3G");
            REQUIRE(mbs[0].get_name() == "cc-pVDZ");
        }

        SECTION("Move assignment") {
            MolecularBasisSet copy(mbs);
            MolecularBasisSet moved;
            auto pmoved = &(moved = std::move(copy));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved == mbs);
        }
    }

    SECTION("push_back") {
        REQUIRE(mbs.size() == 2);
        REQUIRE(mbs[0] == o);
        REQUIRE(mbs[1] == h);
        REQUIRE(mbs.n_shells() == 3);
        REQUIRE(mbs.n_primitives() == 5);

        SECTION("Views") {
            MolecularBasisSet built;
            built.push_back(const_atomic_basis_set_view(o));
            built.push_back(atomic_basis_set_view(h));
            REQUIRE(built == mbs);
        }

        SECTION("The atoms are copied, not aliased") {
            o.set_name("changed");
            o.primitive(0).set_coefficient(42.0);
            REQUIRE(mbs[0].get_name() == "cc-pVDZ");
            REQUIRE(as_double(mbs.primitive(0).get_coefficient()) == 0.5);
        }

        SECTION("An atom of the set itself") {
            // The new atom's state is read out of the state being
            // reallocated.
            mbs.push_back(mbs[0]);
            REQUIRE(mbs.size() == 3);
            REQUIRE(mbs[2] == o);
            REQUIRE(mbs[0] == o);
        }

        SECTION("Mismatched floating-point types are rejected atomically") {
            // The exponents match the set's, the coefficients do not. As with
            // AtomicBasisSet::add_shell, nothing must change.
            std::vector<float> cs{1.0f};
            std::vector<double> es{1.0};
            AtomicBasisSet f("", 1, r_h);
            f.add_shell(0, cs.begin(), cs.end(), es.begin(), es.end());
            MolecularBasisSet copy(mbs);
            REQUIRE_THROWS_AS(mbs.push_back(f), std::runtime_error);
            REQUIRE(mbs == copy);
            REQUIRE(mbs.get_coefficient_buffer().size() == 5);
            REQUIRE(mbs.get_exponent_buffer().size() == 5);
            REQUIRE(mbs.get_centers().size() == 2);
        }

        SECTION("Mismatched center types are rejected atomically") {
            Point rf(0.0f, 0.0f, 0.0f);
            AtomicBasisSet f("", 1, rf);
            f.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
            MolecularBasisSet copy(mbs);
            REQUIRE_THROWS_AS(mbs.push_back(f), std::runtime_error);
            REQUIRE(mbs == copy);
            REQUIRE(mbs.get_coefficient_buffer().size() == 5);
            REQUIRE(mbs.get_centers().size() == 2);
        }
    }

    SECTION("swap") {
        MolecularBasisSet copy(mbs);
        MolecularBasisSet other;
        mbs.swap(other);
        REQUIRE(other == copy);
        REQUIRE(mbs == MolecularBasisSet());
    }

    SECTION("save/load") {
        auto round_trip = [](const MolecularBasisSet& input) {
            std::stringstream ss;
            {
                cereal::BinaryOutputArchive ar(ss);
                ar(input);
            }
            MolecularBasisSet output;
            {
                cereal::BinaryInputArchive ar(ss);
                ar(output);
            }
            return output;
        };
        REQUIRE(round_trip(mbs) == mbs);
        REQUIRE(round_trip(MolecularBasisSet()) == MolecularBasisSet());

        // The per-atom shell types survive the round trip
        auto loaded = round_trip(mbs);
        REQUIRE(loaded.purity(0) == ShellPurity::cartesian);
        REQUIRE(loaded.purity(1) == ShellPurity::pure);
    }
}
