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

// Everything AtomicBasisSetCommon implements is tested here, through an
// AtomicBasisSet; that the views reach the same implementation is tested in
// atomic_basis_set_view.cpp. The shell type only changes what indexing hands
// out and how many AOs a shell has, so both purities are tested at once.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/atomic_basis_set.hpp>
#include <chemist/experimental/basis_set/cca_shell.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

TEMPLATE_TEST_CASE("experimental::AtomicBasisSetCommon", "", CartesianAO,
                   SphericalAO) {
    using shell_type            = CCAShell<TestType>;
    using const_shell_view_type = CCAShellView<const shell_type>;
    using size_type             = std::size_t;
    constexpr bool pure         = std::is_same_v<TestType, SphericalAO>;
    const auto purity = pure ? ShellPurity::pure : ShellPurity::cartesian;

    // An s, a p, and a d shell, with 2, 1, and 3 primitives. So shell 0 owns
    // primitives [0, 2), shell 1 owns [2, 3), and shell 2 owns [3, 6).
    std::vector<double> s_cs{0.5, 0.25}, s_es{3.0, 0.5};
    std::vector<double> p_cs{1.0}, p_es{0.8};
    std::vector<double> d_cs{0.1, 0.2, 0.3}, d_es{4.0, 2.0, 1.0};
    Point r0(1.0, 2.0, 3.0);

    shell_type s(s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end(),
                 size_type(0), r0);
    shell_type p(p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end(),
                 size_type(1), r0);
    shell_type d(d_cs.begin(), d_cs.end(), d_es.begin(), d_es.end(),
                 size_type(2), r0);

    AtomicBasisSet abs("cc-pVDZ", 8, r0, purity);
    abs.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
    abs.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(), p_es.end());
    abs.add_shell(2, d_cs.begin(), d_cs.end(), d_es.begin(), d_es.end());
    const AtomicBasisSet& cabs = abs;

    // Downcasts the result of indexing to the concrete shell view
    auto as_shell =
      [](const AOShellView& shell) -> const const_shell_view_type& {
        return dynamic_cast<const const_shell_view_type&>(shell);
    };

    SECTION("Per-center state") {
        REQUIRE(abs.get_name() == "cc-pVDZ");
        REQUIRE(abs.get_atomic_number() == 8);
        REQUIRE(abs.get_center() == r0);
        REQUIRE(cabs.get_center() == r0);
    }

    SECTION("Shell type") {
        REQUIRE(abs.purity() == purity);
        REQUIRE(abs.ordering() == AOOrdering::cca);
        REQUIRE(abs.is_pure() == pure);
        REQUIRE(abs.is_cartesian() == !pure);
    }

    SECTION("set_name and set_atomic_number") {
        abs.set_name("STO-3G");
        abs.set_atomic_number(1);
        REQUIRE(abs.get_name() == "STO-3G");
        REQUIRE(abs.get_atomic_number() == 1);
    }

    SECTION("Moving the center moves every shell") {
        Point r1(4.0, 5.0, 6.0);
        SECTION("set_center") { abs.set_center(r1); }
        SECTION("Writing through get_center") { abs.get_center() = r1; }
        REQUIRE(abs.get_center() == r1);
        for(size_type i = 0; i < abs.size(); ++i)
            REQUIRE(abs.at(i)->get_center() == r1);
        for(size_type i = 0; i < abs.n_primitives(); ++i)
            REQUIRE(abs.primitive(i).get_center() == r1);
    }

    SECTION("size/empty") {
        REQUIRE(abs.size() == 3);
        REQUIRE_FALSE(abs.empty());
        REQUIRE(AtomicBasisSet("", 0, r0, purity).empty());
    }

    SECTION("at/operator[] build views of the shells") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(abs.at(0)), std::unique_ptr<AOShellView>>);
        REQUIRE(as_shell(*abs.at(0)) == s);
        REQUIRE(as_shell(*abs.at(1)) == p);
        REQUIRE(as_shell(*abs.at(2)) == d);
        REQUIRE(as_shell(*abs[2]) == d);
        REQUIRE(as_shell(*cabs.at(1)) == p);
        REQUIRE_THROWS_AS(abs.at(3), std::out_of_range);
    }

    SECTION("Each call builds a new view") {
        auto s0 = abs.at(0);
        auto s1 = abs.at(0);
        REQUIRE(s0.get() != s1.get());
        REQUIRE(s0->are_equal(*s1));
    }

    SECTION("The shells alias the state of the set") {
        auto shell = abs.at(2);

        // The first primitive of the d shell is the fourth primitive overall
        abs.primitive(3).set_exponent(9.0);
        auto cg = shell->get_contracted_gaussian();
        REQUIRE(as_double(cg[0].get_exponent()) == 9.0);

        abs.set_l(2, 3);
        REQUIRE(shell->get_l() == 3);
        REQUIRE(shell->size() == (pure ? 7 : 10));
    }

    SECTION("get_l/set_l") {
        REQUIRE(abs.get_l(0) == 0);
        REQUIRE(abs.get_l(1) == 1);
        REQUIRE(abs.get_l(2) == 2);
        abs.set_l(1, 3);
        REQUIRE(abs.get_l(1) == 3);
        REQUIRE_THROWS_AS(abs.get_l(3), std::out_of_range);
        REQUIRE_THROWS_AS(abs.set_l(3, 0), std::out_of_range);
    }

    SECTION("n_aos") {
        REQUIRE(abs.n_aos() == (pure ? 1 + 3 + 5 : 1 + 3 + 6));
        REQUIRE(AtomicBasisSet().n_aos() == 0);
    }

    SECTION("n_primitives") {
        REQUIRE(abs.n_primitives() == 6);
        REQUIRE(AtomicBasisSet().n_primitives() == 0);
    }

    SECTION("primitive_range") {
        using range_type = typename AtomicBasisSet::range_type;
        REQUIRE(abs.primitive_range(0) == range_type{0, 2});
        REQUIRE(abs.primitive_range(1) == range_type{2, 3});
        REQUIRE(abs.primitive_range(2) == range_type{3, 6});
        REQUIRE_THROWS_AS(abs.primitive_range(3), std::out_of_range);
    }

    SECTION("primitive_to_shell") {
        const std::vector<size_type> corr{0, 0, 1, 2, 2, 2};
        for(size_type i = 0; i < corr.size(); ++i)
            REQUIRE(abs.primitive_to_shell(i) == corr[i]);
        REQUIRE_THROWS_AS(abs.primitive_to_shell(6), std::out_of_range);
    }

    SECTION("primitive") {
        STATIC_REQUIRE(
          std::is_same_v<decltype(abs.primitive(0)), PrimitiveView<Primitive>>);
        STATIC_REQUIRE(std::is_same_v<decltype(cabs.primitive(0)),
                                      PrimitiveView<const Primitive>>);

        // The primitives are those of the shells, in order
        size_type offset = 0;
        for(const auto& shell : {s, p, d}) {
            auto cg = shell.get_contracted_gaussian();
            for(size_type j = 0; j < cg.size(); ++j, ++offset) {
                REQUIRE(abs.primitive(offset) == cg[j]);
                REQUIRE(cabs.primitive(offset) == cg[j]);
            }
        }
        REQUIRE_THROWS_AS(abs.primitive(6), std::out_of_range);
        REQUIRE_THROWS_AS(cabs.primitive(6), std::out_of_range);
    }

    SECTION("The primitives alias the state of the set") {
        auto prim = abs.primitive(2);
        prim.set_coefficient(42.0);
        REQUIRE(as_double(
                  abs.at(1)->get_contracted_gaussian()[0].get_coefficient()) ==
                42.0);

        // Including the angular momentum, which is the shell's
        prim.set_l(std::size_t(4));
        REQUIRE(abs.get_l(1) == 4);
    }

    SECTION("Parameter buffers") {
        auto cs = cabs.get_coefficient_buffer();
        auto es = cabs.get_exponent_buffer();
        const std::vector<double> corr_cs{0.5, 0.25, 1.0, 0.1, 0.2, 0.3};
        const std::vector<double> corr_es{3.0, 0.5, 0.8, 4.0, 2.0, 1.0};
        REQUIRE(cs.size() == 6);
        REQUIRE(es.size() == 6);
        REQUIRE(cs.is_contiguous());
        for(size_type i = 0; i < 6; ++i) {
            REQUIRE(as_double(cs.at(i)) == corr_cs[i]);
            REQUIRE(as_double(es.at(i)) == corr_es[i]);
        }

        // Writing through the mutable buffers writes into the set
        abs.get_coefficient_buffer().at(5) = 7.0;
        abs.get_exponent_buffer().at(5)    = 8.0;
        REQUIRE(as_double(cabs.primitive(5).get_coefficient()) == 7.0);
        REQUIRE(as_double(cabs.primitive(5).get_exponent()) == 8.0);
    }

    SECTION("Shells with no primitives") {
        // An empty shell in the middle owns no primitives, so
        // primitive_to_shell has to skip over it.
        std::vector<double> none;
        AtomicBasisSet e("", 0, r0, purity);
        e.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(), s_es.end());
        e.add_shell(1, none.begin(), none.end(), none.begin(), none.end());
        e.add_shell(2, d_cs.begin(), d_cs.end(), d_es.begin(), d_es.end());

        REQUIRE(e.size() == 3);
        REQUIRE(e.n_primitives() == 5);
        REQUIRE(e.primitive_range(1) ==
                typename AtomicBasisSet::range_type{2, 2});
        REQUIRE(e.primitive_to_shell(1) == 0);
        REQUIRE(e.primitive_to_shell(2) == 2);
        REQUIRE(e.at(1)->get_contracted_gaussian().size() == 0);
        REQUIRE(e.at(1)->get_l() == 1);
    }

    SECTION("Comparisons") {
        auto copy = abs;
        REQUIRE(copy == abs);
        REQUIRE_FALSE(copy != abs);

        SECTION("Different name") { copy.set_name("STO-3G"); }
        SECTION("Different atomic number") { copy.set_atomic_number(1); }
        SECTION("Different center") { copy.set_center(Point(0.0, 0.0, 0.0)); }
        SECTION("Different angular momentum") { copy.set_l(0, 1); }
        SECTION("Different parameters") {
            copy.primitive(0).set_coefficient(42.0);
        }
        SECTION("Different number of shells") {
            copy.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(),
                           s_es.end());
        }
        SECTION("Different purity") {
            const auto other =
              pure ? ShellPurity::cartesian : ShellPurity::pure;
            AtomicBasisSet rhs("cc-pVDZ", 8, r0, other);
            rhs.add_shell(0, s_cs.begin(), s_cs.end(), s_es.begin(),
                          s_es.end());
            rhs.add_shell(1, p_cs.begin(), p_cs.end(), p_es.begin(),
                          p_es.end());
            rhs.add_shell(2, d_cs.begin(), d_cs.end(), d_es.begin(),
                          d_es.end());
            copy = rhs;
        }
        REQUIRE(copy != abs);
        REQUIRE_FALSE(copy == abs);
    }
}
