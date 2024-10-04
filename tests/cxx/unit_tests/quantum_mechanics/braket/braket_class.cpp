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

#include "../../test_helpers.hpp"
#include "../test_qm.hpp"
#include <chemist/quantum_mechanics/braket/braket_class.hpp>
#include <chemist/quantum_mechanics/operator/operator.hpp>
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>

using namespace chemist;
using namespace chemist::qm_operator;
using namespace chemist::wavefunction;
using namespace chemist::braket;

TEST_CASE("bra_ket_base_type") {
    using vector_space_type = wavefunction::AOs;
    using op_type           = qm_operator::Kinetic<Electron>;

    using tr_type =
      chemist::braket::detail_::bra_ket_base_type<vector_space_type, op_type,
                                                  vector_space_type>;

    STATIC_REQUIRE(std::is_same_v<TensorRepresentation, tr_type>);
}

/* Testing strategy.
 *
 * Methods implemented via this class, but accessed via base classes (e.g.,
 * clone and are_equal) are tested in test cases dedicated to those base
 * classes. Here we only test methods defined and implemented in the BraKet
 * class. Ultimately there are a lot of possible permutations of BraKet objects
 * and we only spot check a few. In practice, while the BraKet class represents
 * a fairly complicated object, aside from the template meta-programming, the
 * BraKet class's implementation is actually fairly simple and we anticipate
 * that our spot checking represents fairly thorough coverage.
 */

using bra_ket_tuples =
  std::tuple<std::tuple<AOs, t_e_type, AOs>, std::tuple<AOs, T_e_type, AOs>>;

TEMPLATE_LIST_TEST_CASE("BraKet", "", bra_ket_tuples) {
    using operator_type = std::tuple_element_t<1, TestType>;
    using bra_type      = std::tuple_element_t<0, TestType>;
    using ket_type      = std::tuple_element_t<2, TestType>;

    auto operators     = std::make_tuple(t_e_type{}, T_e_type{});
    auto bras_and_kets = std::make_tuple(AOs{test_chemist::h2_basis()});

    // If the bra_type == ket_type we'll grab the ket out of this array to make
    // sure they can be told a part
    auto alt_bras_and_kets = std::make_tuple(AOs{});

    auto op  = std::get<operator_type>(operators);
    auto bra = std::get<bra_type>(bras_and_kets);
    auto ket = std::get<ket_type>(bras_and_kets);
    if constexpr(std::is_same_v<bra_type, ket_type>) {
        ket = std::get<ket_type>(alt_bras_and_kets);
    }

    BraKet o_ij(bra, op, ket);

    SECTION("Ctors/assignment") {
        SECTION("value") {
            REQUIRE(o_ij.bra() == bra);
            REQUIRE(o_ij.op() == op);
            REQUIRE(o_ij.ket() == ket);
        }
        SECTION("copy") {
            BraKet copy_o(o_ij);
            REQUIRE(copy_o == o_ij);
        }
        SECTION("move") {
            BraKet copy_o(o_ij);
            BraKet move_o(std::move(o_ij));
            REQUIRE(copy_o == move_o);
        }
        SECTION("copy assignment") {
            BraKet lhs(ket, op, ket);
            auto plhs = &(lhs = o_ij);
            REQUIRE(lhs == o_ij);
            REQUIRE(plhs == &lhs);
        }
        SECTION("move assignment") {
            BraKet lhs(ket, op, ket);
            BraKet copy_o(o_ij);
            auto plhs = &(lhs = std::move(o_ij));
            REQUIRE(lhs == copy_o);
            REQUIRE(plhs == &lhs);
        }
    }

    SECTION("bra()") { REQUIRE(o_ij.bra() == bra); }

    SECTION("bra() const") { REQUIRE(std::as_const(o_ij).bra() == bra); }

    SECTION("op()") { REQUIRE(o_ij.op() == op); }

    SECTION("op() const") { REQUIRE(std::as_const(o_ij).op() == op); }

    SECTION("ket()") { REQUIRE(o_ij.ket() == ket); }

    SECTION("ket() const") { REQUIRE(std::as_const(o_ij).ket() == ket); }

    SECTION("operator==") {
        SECTION("Same value") {
            BraKet rhs(bra, op, ket);
            REQUIRE(o_ij == rhs);
        }
        SECTION("Bra and ket flipped") {
            BraKet rhs(ket, op, bra);
            REQUIRE_FALSE(o_ij == rhs);
        }
        SECTION("Different bra") {
            BraKet rhs(ket, op, bra);
            REQUIRE_FALSE(o_ij == rhs);
        }
        SECTION("Different operator") {
            constexpr auto is_t_e = std::is_same_v<operator_type, t_e_type>;
            using op2_type = std::conditional_t<is_t_e, T_e_type, t_e_type>;

            auto op2 = std::get<op2_type>(operators);
            BraKet rhs(bra, op2, ket);
            REQUIRE_FALSE(o_ij == rhs);
        }
        SECTION("Different ket") {
            BraKet rhs(bra, op, bra);
            REQUIRE_FALSE(o_ij == rhs);
        }
    }

    SECTION("operator!=") {
        // N.b. implemented in terms of operator== just spot check
        BraKet same(bra, op, ket);
        REQUIRE_FALSE(o_ij != same);

        BraKet diff(ket, op, bra);
        REQUIRE(o_ij != diff);
    }
}