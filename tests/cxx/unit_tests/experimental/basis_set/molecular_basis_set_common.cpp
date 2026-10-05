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

// Everything MolecularBasisSetCommon implements is tested here, through a
// MolecularBasisSet; that the views reach the same implementation is tested
// in molecular_basis_set_view.cpp. The set mixes a Cartesian atom with a pure
// one, so that both purities, and the per-atom bookkeeping between them, are
// always exercised.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/cca_shell.hpp>
#include <chemist/experimental/basis_set/molecular_basis_set.hpp>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEST_CASE("experimental::MolecularBasisSetCommon") {
    using size_type  = std::size_t;
    using range_type = typename MolecularBasisSet::range_type;

    // O: a Cartesian s shell with 2 primitives and p shell with 1.
    // H: a pure s shell with 3 primitives and d shell with 1.
    // So the shells own primitives [0, 2), [2, 3), [3, 6), and [6, 7).
    std::vector<double> o_s_cs{0.5, 0.25}, o_s_es{3.0, 0.5};
    std::vector<double> o_p_cs{1.0}, o_p_es{0.8};
    std::vector<double> h_s_cs{0.1, 0.2, 0.3}, h_s_es{4.0, 2.0, 1.0};
    std::vector<double> h_d_cs{0.7}, h_d_es{0.4};
    Point r_o(0.0, 0.0, 0.0), r_h(0.0, 0.0, 1.8);

    AtomicBasisSet o("cc-pVDZ", 8, r_o, ShellPurity::cartesian);
    o.add_shell(0, o_s_cs.begin(), o_s_cs.end(), o_s_es.begin(), o_s_es.end());
    o.add_shell(1, o_p_cs.begin(), o_p_cs.end(), o_p_es.begin(), o_p_es.end());
    AtomicBasisSet h("STO-3G", 1, r_h, ShellPurity::pure);
    h.add_shell(0, h_s_cs.begin(), h_s_cs.end(), h_s_es.begin(), h_s_es.end());
    h.add_shell(2, h_d_cs.begin(), h_d_cs.end(), h_d_es.begin(), h_d_es.end());

    MolecularBasisSet mbs{o, h};
    const MolecularBasisSet& cmbs = mbs;

    SECTION("size/empty") {
        REQUIRE(mbs.size() == 2);
        REQUIRE_FALSE(mbs.empty());
        REQUIRE(MolecularBasisSet().size() == 0);
        REQUIRE(MolecularBasisSet().empty());
    }

    SECTION("at/operator[] build views of the atomic basis sets") {
        STATIC_REQUIRE(std::is_same_v<decltype(mbs.at(0)),
                                      AtomicBasisSetView<AtomicBasisSet>>);
        STATIC_REQUIRE(
          std::is_same_v<decltype(cmbs.at(0)),
                         AtomicBasisSetView<const AtomicBasisSet>>);
        REQUIRE(mbs.at(0) == o);
        REQUIRE(mbs.at(1) == h);
        REQUIRE(mbs[1] == h);
        REQUIRE(cmbs.at(0) == o);
        REQUIRE(cmbs[1] == h);
        REQUIRE_THROWS_AS(mbs.at(2), std::out_of_range);
        REQUIRE_THROWS_AS(cmbs[2], std::out_of_range);
    }

    SECTION("The atomic basis sets alias the state of the set") {
        auto h_view = mbs.at(1);

        // Writes through the set are visible through the atom's view ...
        mbs.primitive(4).set_exponent(9.0);
        mbs.get_centers()[1] = Point(1.0, 2.0, 3.0);
        REQUIRE(as_double(h_view.primitive(1).get_exponent()) == 9.0);
        REQUIRE(h_view.get_center() == Point(1.0, 2.0, 3.0));

        // ... and writes through the atom's view land in the set
        h_view.set_name("6-31G");
        h_view.set_atomic_number(2);
        h_view.set_l(1, 3);
        h_view.primitive(0).set_coefficient(42.0);
        REQUIRE(mbs[1].get_name() == "6-31G");
        REQUIRE(mbs[1].get_atomic_number() == 2);
        REQUIRE(mbs.get_l(3) == 3);
        REQUIRE(as_double(mbs.primitive(3).get_coefficient()) == 42.0);

        // None of which touched the other atom
        REQUIRE(mbs[0] == o);
    }

    SECTION("Iterators") {
        REQUIRE(std::distance(mbs.begin(), mbs.end()) == 2);
        REQUIRE(std::distance(cmbs.cbegin(), cmbs.cend()) == 2);
        std::vector<AtomicBasisSet> corr{o, h};
        size_type a = 0;
        for(auto atom : mbs) REQUIRE(atom == corr[a++]);
        a = 0;
        for(auto atom : cmbs) REQUIRE(atom == corr[a++]);

        // Iterating a mutable set yields mutable views
        for(auto atom : mbs) atom.set_name("changed");
        REQUIRE(mbs[0].get_name() == "changed");
        REQUIRE(mbs[1].get_name() == "changed");
    }

    SECTION("get_centers") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(mbs.get_centers()), PointSetView<PointSet>>);
        STATIC_REQUIRE(std::is_same_v<decltype(cmbs.get_centers()),
                                      PointSetView<const PointSet>>);
        auto centers = cmbs.get_centers();
        REQUIRE(centers.size() == 2);
        REQUIRE(centers[0] == r_o);
        REQUIRE(centers[1] == r_h);

        // The one copy of a center is shared by everything on the atom
        Point r1(4.0, 5.0, 6.0);
        mbs.get_centers()[0] = r1;
        REQUIRE(mbs[0].get_center() == r1);
        REQUIRE(mbs.shell(1)->get_center() == r1);
        REQUIRE(mbs.primitive(2).get_center() == r1);
        REQUIRE(mbs[1].get_center() == r_h);
    }

    SECTION("Shell types") {
        REQUIRE(mbs.purity(0) == ShellPurity::cartesian);
        REQUIRE(mbs.purity(1) == ShellPurity::pure);
        REQUIRE(mbs.ordering(0) == AOOrdering::cca);
        REQUIRE(mbs.ordering(1) == AOOrdering::cca);
        REQUIRE_THROWS_AS(mbs.purity(2), std::out_of_range);
        REQUIRE_THROWS_AS(mbs.ordering(2), std::out_of_range);

        REQUIRE_FALSE(mbs.has_uniform_purity());
        REQUIRE(mbs.has_uniform_ordering());
        REQUIRE_FALSE(mbs.is_pure());
        REQUIRE_FALSE(mbs.is_cartesian());

        MolecularBasisSet all_cart{o, o};
        REQUIRE(all_cart.has_uniform_purity());
        REQUIRE(all_cart.is_cartesian());
        REQUIRE_FALSE(all_cart.is_pure());

        MolecularBasisSet all_pure{h, h};
        REQUIRE(all_pure.has_uniform_purity());
        REQUIRE(all_pure.is_pure());
        REQUIRE_FALSE(all_pure.is_cartesian());

        // Vacuously true for an empty set
        MolecularBasisSet empty;
        REQUIRE(empty.has_uniform_purity());
        REQUIRE(empty.has_uniform_ordering());
        REQUIRE(empty.is_pure());
        REQUIRE(empty.is_cartesian());
    }

    SECTION("n_shells") {
        REQUIRE(mbs.n_shells() == 4);
        REQUIRE(MolecularBasisSet().n_shells() == 0);
    }

    SECTION("shell_range") {
        REQUIRE(mbs.shell_range(0) == range_type{0, 2});
        REQUIRE(mbs.shell_range(1) == range_type{2, 4});
        REQUIRE_THROWS_AS(mbs.shell_range(2), std::out_of_range);
    }

    SECTION("shell_to_atom") {
        const std::vector<size_type> corr{0, 0, 1, 1};
        for(size_type s = 0; s < corr.size(); ++s)
            REQUIRE(mbs.shell_to_atom(s) == corr[s]);
        REQUIRE_THROWS_AS(mbs.shell_to_atom(4), std::out_of_range);
    }

    SECTION("shell") {
        // The shells are those of the atoms, in order
        size_type s = 0;
        for(const auto& atom : {o, h})
            for(size_type i = 0; i < atom.size(); ++i, ++s)
                REQUIRE(mbs.shell(s)->are_equal(*atom.at(i)));
        REQUIRE_THROWS_AS(mbs.shell(4), std::out_of_range);

        // The shell aliases the set, and outlives the atom view it came from
        auto shell = mbs.shell(3);
        mbs.set_l(3, 1);
        REQUIRE(shell->get_l() == 1);
        REQUIRE(shell->size() == 3);
    }

    SECTION("get_l/set_l") {
        const std::vector<size_type> corr{0, 1, 0, 2};
        for(size_type s = 0; s < corr.size(); ++s)
            REQUIRE(mbs.get_l(s) == corr[s]);
        mbs.set_l(2, 3);
        REQUIRE(mbs.get_l(2) == 3);
        REQUIRE(mbs[1].get_l(0) == 3);
        REQUIRE_THROWS_AS(mbs.get_l(4), std::out_of_range);
        REQUIRE_THROWS_AS(mbs.set_l(4, 0), std::out_of_range);
    }

    SECTION("n_aos") {
        // Each atom's shells are sized by that atom's purity
        REQUIRE(mbs.n_aos() == (1 + 3) + (1 + 5));
        REQUIRE(mbs.n_aos() == o.n_aos() + h.n_aos());
        REQUIRE(MolecularBasisSet().n_aos() == 0);
    }

    SECTION("n_primitives") {
        REQUIRE(mbs.n_primitives() == 7);
        REQUIRE(MolecularBasisSet().n_primitives() == 0);
    }

    SECTION("primitive_range") {
        REQUIRE(mbs.primitive_range(0) == range_type{0, 2});
        REQUIRE(mbs.primitive_range(1) == range_type{2, 3});
        REQUIRE(mbs.primitive_range(2) == range_type{3, 6});
        REQUIRE(mbs.primitive_range(3) == range_type{6, 7});
        REQUIRE_THROWS_AS(mbs.primitive_range(4), std::out_of_range);
    }

    SECTION("primitive_to_shell") {
        const std::vector<size_type> corr{0, 0, 1, 2, 2, 2, 3};
        for(size_type p = 0; p < corr.size(); ++p)
            REQUIRE(mbs.primitive_to_shell(p) == corr[p]);
        REQUIRE_THROWS_AS(mbs.primitive_to_shell(7), std::out_of_range);
    }

    SECTION("primitive") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(mbs.primitive(0)), PrimitiveView<Primitive>>);
        STATIC_REQUIRE(std::is_same_v<decltype(cmbs.primitive(0)),
                                      PrimitiveView<const Primitive>>);

        // The primitives are those of the atoms, in order
        size_type p = 0;
        for(const auto& atom : {o, h}) {
            for(size_type i = 0; i < atom.n_primitives(); ++i, ++p) {
                REQUIRE(mbs.primitive(p) == atom.primitive(i));
                REQUIRE(cmbs.primitive(p) == atom.primitive(i));
            }
        }
        REQUIRE_THROWS_AS(mbs.primitive(7), std::out_of_range);
        REQUIRE_THROWS_AS(cmbs.primitive(7), std::out_of_range);
    }

    SECTION("The primitives alias the state of the set") {
        auto prim = mbs.primitive(6);
        prim.set_coefficient(42.0);
        REQUIRE(as_double(mbs[1].primitive(3).get_coefficient()) == 42.0);

        // Including the angular momentum, which is the shell's, and the
        // center, which is the atom's
        prim.set_l(std::size_t(4));
        prim.set_center(Point(7.0, 8.0, 9.0));
        REQUIRE(mbs.get_l(3) == 4);
        REQUIRE(mbs[1].get_center() == Point(7.0, 8.0, 9.0));
    }

    SECTION("Parameter buffers") {
        auto cs = cmbs.get_coefficient_buffer();
        auto es = cmbs.get_exponent_buffer();
        const std::vector<double> corr_cs{0.5, 0.25, 1.0, 0.1, 0.2, 0.3, 0.7};
        const std::vector<double> corr_es{3.0, 0.5, 0.8, 4.0, 2.0, 1.0, 0.4};
        REQUIRE(cs.size() == 7);
        REQUIRE(es.size() == 7);
        REQUIRE(cs.is_contiguous());
        for(size_type p = 0; p < 7; ++p) {
            REQUIRE(as_double(cs.at(p)) == corr_cs[p]);
            REQUIRE(as_double(es.at(p)) == corr_es[p]);
        }

        // Writing through the mutable buffers writes into the set
        mbs.get_coefficient_buffer().at(5) = 7.0;
        mbs.get_exponent_buffer().at(5)    = 8.0;
        REQUIRE(as_double(cmbs[1].primitive(2).get_coefficient()) == 7.0);
        REQUIRE(as_double(cmbs[1].primitive(2).get_exponent()) == 8.0);
    }

    SECTION("Atoms with no shells") {
        // An atom with no shells, owns no shells, so
        // shell_to_atom has to skip over it. Placing one last checks that
        // building its view does not read past the end of the offsets.
        AtomicBasisSet empty("", 0, Point(9.0, 9.0, 9.0));
        MolecularBasisSet e{o, empty, h, empty};

        REQUIRE(e.size() == 4);
        REQUIRE(e.n_shells() == 4);
        REQUIRE(e.shell_range(1) == range_type{2, 2});
        REQUIRE(e.shell_range(3) == range_type{4, 4});
        REQUIRE(e.shell_to_atom(1) == 0);
        REQUIRE(e.shell_to_atom(2) == 2);
        REQUIRE(e[1] == empty);
        REQUIRE(e[2] == h);
        REQUIRE(e[3] == empty);
        REQUIRE(e[3].n_primitives() == 0);
    }

    SECTION("Comparisons") {
        MolecularBasisSet copy(mbs);
        REQUIRE(copy == mbs);
        REQUIRE_FALSE(copy != mbs);

        SECTION("Different atom") { copy[1].set_name("6-31G"); }
        SECTION("Different parameters") {
            copy.primitive(0).set_coefficient(42.0);
        }
        SECTION("Different number of atoms") { copy.push_back(h); }
        SECTION("Same atoms, different order") {
            copy = MolecularBasisSet{h, o};
        }
        REQUIRE(copy != mbs);
        REQUIRE_FALSE(copy == mbs);
    }
}
