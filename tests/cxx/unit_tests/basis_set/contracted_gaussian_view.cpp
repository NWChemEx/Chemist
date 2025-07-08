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

#include "../catch.hpp"
#include <chemist/basis_set/contracted_gaussian_traits.hpp>
#include <chemist/basis_set/contracted_gaussian_view.hpp>
#include <utility>

using namespace chemist::basis_set;

template<typename LHSType, typename RHSType>
void compare_addresses(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.center().x() == &rhs.center().x());
    REQUIRE(&lhs.center().y() == &rhs.center().y());
    REQUIRE(&lhs.center().z() == &rhs.center().z());

    for(std::size_t prim_i = 0; prim_i < lhs.size(); ++prim_i) {
        REQUIRE(&lhs[prim_i].coefficient() == &rhs[prim_i].coefficient());
        REQUIRE(&lhs[prim_i].exponent() == &rhs[prim_i].exponent());
    }
}

TEMPLATE_TEST_CASE("ContractedGaussianView", "", float, double) {
    using prim_type  = Primitive<TestType>;
    using cg_type    = ContractedGaussian<prim_type>;
    using view_type  = ContractedGaussianView<cg_type>;
    using const_view = ContractedGaussianView<const cg_type>;
    using cg_traits  = ContractedGaussianTraits<cg_type>;

    using center_type = typename cg_traits::center_type;
    using coefficient_vector =
      std::vector<typename cg_traits::coefficient_type>;
    using exponent_vector = std::vector<typename cg_traits::exponent_type>;

    coefficient_vector cs{1.0, 2.0, 3.0};
    exponent_vector es{4.0, 5.0, 6.0};
    center_type r0{7.0, 8.0, 9.0};

    cg_type cg0;
    cg_type cg1(cs.begin(), cs.end(), es.begin(), es.end(), r0);

    auto& c0 = cg1[0].coefficient();
    auto& e0 = cg1[0].exponent();

    view_type cg0_view;
    const_view const_cg0_view;

    view_type cg1_view(cg1);
    const_view const_cg1_view(cg1);

    SECTION("Ctor and assignment") {
        SECTION("Default") {
            REQUIRE(cg0_view.size() == 0);
            REQUIRE(cg0_view.is_null());

            REQUIRE(const_cg0_view.size() == 0);
            REQUIRE(const_cg0_view.is_null());
        }

        SECTION("n_prims, c0, e0, r") {
            view_type cg2_view(3, c0, e0, cg1.center());

            REQUIRE(cg2_view.size() == 3);
            REQUIRE(cg2_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg2_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg2_view[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg2_view, cg1);

            const_view const_cg2_view(3, c0, e0, cg1.center());

            REQUIRE(const_cg2_view.size() == 3);
            REQUIRE(const_cg2_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg2_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg2_view[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg2_view, cg1);
        }

        SECTION("ContractedGaussian") {
            view_type null_view(cg0);
            REQUIRE(null_view.is_null());
            REQUIRE(null_view.size() == 0);

            const_view const_null_view(cg0);
            REQUIRE(const_null_view.is_null());
            REQUIRE(const_null_view.size() == 0);

            REQUIRE(cg1_view.size() == 3);
            REQUIRE(cg1_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1_view[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg1_view, cg1);

            REQUIRE(const_cg1_view.size() == 3);
            REQUIRE(const_cg1_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1_view[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg1_view, cg1);
        }

        SECTION("Assign ContractedGaussian") {
            view_type null_view;
            // Throws if assigning to a null view
            REQUIRE_THROWS_AS(null_view = cg1, std::runtime_error);

            // Throws if assigning a null CG
            REQUIRE_THROWS_AS(cg1_view = cg0, std::runtime_error);

            auto ebegin = es.begin();
            auto cbegin = cs.begin();

            // Throws if sizes don't match
            cg_type cg2(ebegin, ebegin + 1, cbegin, cbegin + 1, r0);
            REQUIRE_THROWS_AS(cg1_view = cg2, std::runtime_error);

            cg2 = cg_type(ebegin, es.end(), cbegin, cs.end(), r0);

            auto pcg1_view = &(cg1_view = cg2);
            REQUIRE(cg1_view.size() == 3);
            REQUIRE(cg1_view[0] == prim_type(4.0, 1.0, r0));
            REQUIRE(cg1_view[1] == prim_type(5.0, 2.0, r0));
            REQUIRE(cg1_view[2] == prim_type(6.0, 3.0, r0));
            compare_addresses(cg1_view, cg1);
            REQUIRE(pcg1_view == &cg1_view);
        }

        SECTION("Const from non-const") {
            const_view const_null(cg0_view);
            REQUIRE(const_null.is_null());
            REQUIRE(const_null.size() == 0);

            const_view const_cg2_view(cg1_view);
            REQUIRE(const_cg2_view.size() == 3);
            REQUIRE(const_cg2_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg2_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg2_view[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(const_cg2_view, cg1);
        }

        SECTION("Copy") {
            view_type cg0_copy(cg0_view);
            REQUIRE(cg0_copy.size() == 0);
            REQUIRE(cg0_copy.is_null());

            const_view const_cg0_copy(const_cg0_view);
            REQUIRE(const_cg0_copy.size() == 0);
            REQUIRE(const_cg0_copy.is_null());

            view_type cg1_copy(cg1_view);
            REQUIRE(cg1_copy.size() == 3);
            REQUIRE(cg1_copy[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1_copy[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1_copy[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg1_copy, cg1);

            const_view const_cg1_copy(const_cg1_view);
            REQUIRE(const_cg1_copy.size() == 3);
            REQUIRE(const_cg1_copy[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1_copy[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1_copy[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(const_cg1_copy, cg1);
        }

        SECTION("Copy assignment") {
            view_type cg0_copy;
            auto pcg0_copy = &(cg0_copy = cg0_view);
            REQUIRE(cg0_copy.size() == 0);
            REQUIRE(cg0_copy.is_null());
            REQUIRE(pcg0_copy == &cg0_copy);

            const_view const_cg0_copy;
            auto pconst_cg0_copy = &(const_cg0_copy = const_cg0_view);
            REQUIRE(const_cg0_copy.size() == 0);
            REQUIRE(const_cg0_copy.is_null());
            REQUIRE(pconst_cg0_copy == &const_cg0_copy);

            view_type cg1_copy;
            auto pcg1_copy = &(cg1_copy = cg1_view);
            REQUIRE(cg1_copy.size() == 3);
            REQUIRE(cg1_copy[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1_copy[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1_copy[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg1_copy, cg1);
            REQUIRE(pcg1_copy == &cg1_copy);

            const_view const_cg1_copy;
            auto pconst_cg1_copy = &(const_cg1_copy = const_cg1_view);
            REQUIRE(const_cg1_copy.size() == 3);
            REQUIRE(const_cg1_copy[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1_copy[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1_copy[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(const_cg1_copy, cg1);
            REQUIRE(pconst_cg1_copy == &const_cg1_copy);
        }

        SECTION("Move") {
            view_type cg0_move(std::move(cg0_view));
            REQUIRE(cg0_move.size() == 0);
            REQUIRE(cg0_move.is_null());

            const_view const_cg0_move(std::move(const_cg0_view));
            REQUIRE(const_cg0_move.size() == 0);
            REQUIRE(const_cg0_move.is_null());

            view_type cg1_move(std::move(cg1_view));
            REQUIRE(cg1_move.size() == 3);
            REQUIRE(cg1_move[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1_move[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1_move[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg1_move, cg1);

            const_view const_cg1_move(std::move(const_cg1_view));
            REQUIRE(const_cg1_move.size() == 3);
            REQUIRE(const_cg1_move[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1_move[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1_move[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(const_cg1_move, cg1);
        }

        SECTION("move assignment") {
            view_type cg0_move;
            auto pcg0_move = &(cg0_move = std::move(cg0_view));
            REQUIRE(cg0_move.size() == 0);
            REQUIRE(cg0_move.is_null());
            REQUIRE(pcg0_move == &cg0_move);

            const_view const_cg0_move;
            auto pconst_cg0_move =
              &(const_cg0_move = std::move(const_cg0_view));
            REQUIRE(const_cg0_move.size() == 0);
            REQUIRE(const_cg0_move.is_null());
            REQUIRE(pconst_cg0_move == &const_cg0_move);

            view_type cg1_move;
            auto pcg1_move = &(cg1_move = std::move(cg1_view));
            REQUIRE(cg1_move.size() == 3);
            REQUIRE(cg1_move[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1_move[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1_move[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(cg1_move, cg1);
            REQUIRE(pcg1_move == &cg1_move);

            const_view const_cg1_move;
            auto pconst_cg1_move =
              &(const_cg1_move = std::move(const_cg1_view));
            REQUIRE(const_cg1_move.size() == 3);
            REQUIRE(const_cg1_move[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1_move[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1_move[2] == prim_type(3.0, 6.0, r0));
            compare_addresses(const_cg1_move, cg1);
            REQUIRE(pconst_cg1_move == &const_cg1_move);
        }
    }

    SECTION("Getters/setters") {
        SECTION("center()") {
            REQUIRE_THROWS_AS(cg0_view.center(), std::runtime_error);
            REQUIRE_THROWS_AS(const_cg0_view.center(), std::runtime_error);

            REQUIRE(cg1_view.center() == r0);
            REQUIRE(const_cg1_view.center() == r0);

            // Is writable?
            center_type origin;
            cg1_view.center() = origin;
            REQUIRE(cg1.center() == origin);
        }

        SECTION("center() const") {
            using re = std::runtime_error;
            REQUIRE_THROWS_AS(std::as_const(cg0_view).center(), re);
            REQUIRE_THROWS_AS(std::as_const(const_cg0_view).center(), re);
            REQUIRE(std::as_const(cg1_view).center() == r0);
            REQUIRE(std::as_const(const_cg1_view).center() == r0);
        }

        SECTION("operator[]") {
            REQUIRE(cg1_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(cg1_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(cg1_view[2] == prim_type(3.0, 6.0, r0));

            REQUIRE(const_cg1_view[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1_view[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1_view[2] == prim_type(3.0, 6.0, r0));

            // Is writeable?
            cg1_view[0] = prim_type(4.0, 7.0, center_type());
            REQUIRE(cg1_view[0].coefficient() == 4.0);
            REQUIRE(cg1_view[0].exponent() == 7.0);
            REQUIRE(cg1_view.center() == center_type{});
        }

        SECTION("operator[] const") {
            const auto& const_cg1 = std::as_const(cg1_view);
            REQUIRE(const_cg1[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_cg1[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_cg1[2] == prim_type(3.0, 6.0, r0));

            const auto& const_const_cg1 = std::as_const(const_cg1_view);
            REQUIRE(const_const_cg1[0] == prim_type(1.0, 4.0, r0));
            REQUIRE(const_const_cg1[1] == prim_type(2.0, 5.0, r0));
            REQUIRE(const_const_cg1[2] == prim_type(3.0, 6.0, r0));
        }

        SECTION("size") {
            REQUIRE(cg0_view.size() == 0);
            REQUIRE(const_cg0_view.size() == 0);
            REQUIRE(cg1_view.size() == 3);
            REQUIRE(const_cg1_view.size() == 3);
        }
    }

    SECTION("utility") {
        SECTION("swap") {
            view_type cg0_copy(cg0_view);
            view_type cg1_copy(cg1_view);

            cg0_view.swap(cg1_view);

            REQUIRE(cg0_copy == cg1_view);
            REQUIRE(cg1_copy == cg0_view);

            const_view const_cg0_copy(const_cg0_view);
            const_view const_cg1_copy(const_cg1_view);

            const_cg0_view.swap(const_cg1_view);

            REQUIRE(const_cg0_copy == const_cg1_view);
            REQUIRE(const_cg1_copy == const_cg0_view);
        }

        SECTION("is_null") {
            REQUIRE(cg0_view.is_null());
            REQUIRE(const_cg0_view.is_null());
            REQUIRE_FALSE(cg1_view.is_null());
            REQUIRE_FALSE(const_cg1_view.is_null());
        }

        SECTION("operator==(ContractedGaussianView)") {
            // Default v default
            REQUIRE(cg0_view == view_type{});
            REQUIRE(const_cg0_view == const_view{});

            // Default v non-default
            REQUIRE_FALSE(cg0_view == cg1_view);
            REQUIRE_FALSE(const_cg0_view == const_cg1_view);

            auto& c0 = cs[0];
            auto& e0 = es[0];

            // Non-default: same value
            REQUIRE(cg1_view == view_type(3, c0, e0, cg1.center()));
            REQUIRE(const_cg1_view == const_view(3, c0, e0, cg1.center()));

            // Non-default: different coefficients
            REQUIRE_FALSE(cg1_view == view_type(3, e0, e0, cg1.center()));
            REQUIRE_FALSE(const_cg1_view ==
                          const_view(3, e0, e0, cg1.center()));

            // Non-default: different exponents
            REQUIRE_FALSE(cg1_view == view_type(3, c0, c0, cg1.center()));
            REQUIRE_FALSE(const_cg1_view ==
                          const_view(3, c0, c0, cg1.center()));

            // Non-default: different center
            REQUIRE_FALSE(cg1_view == view_type(3, c0, e0, cg0.center()));
            REQUIRE_FALSE(const_cg1_view ==
                          const_view(3, c0, e0, cg0.center()));
        }

        SECTION("operator==(ContractedGaussian)") {
            // Default v default
            REQUIRE(cg0_view == cg0);
            REQUIRE(const_cg0_view == cg0);

            // Default v non-default
            REQUIRE_FALSE(cg0_view == cg1);
            REQUIRE_FALSE(const_cg0_view == cg1);

            auto cbegin = cs.begin();
            auto cend   = cs.end();
            auto ebegin = es.begin();
            auto eend   = es.end();

            // Non-default: same value
            REQUIRE(cg1_view == cg_type(cbegin, cend, ebegin, eend, r0));
            REQUIRE(const_cg1_view == cg_type(cbegin, cend, ebegin, eend, r0));

            // Non-default: different coefficients
            REQUIRE_FALSE(cg1_view == cg_type(ebegin, eend, ebegin, eend, r0));
            REQUIRE_FALSE(const_cg1_view ==
                          cg_type(ebegin, eend, ebegin, eend, r0));

            // Non-default: different exponents
            REQUIRE_FALSE(cg1_view == cg_type(cbegin, cend, cbegin, cend, r0));
            REQUIRE_FALSE(const_cg1_view ==
                          cg_type(cbegin, cend, cbegin, cend, r0));

            // Non-default: different center
            REQUIRE_FALSE(cg1_view ==
                          cg_type(cbegin, cend, ebegin, eend, center_type{}));
            REQUIRE_FALSE(const_cg1_view ==
                          cg_type(cbegin, cend, ebegin, eend, center_type{}));
        }

        SECTION("operator!=(ContractedGaussianView)") {
            // Just negates operator== so okay to spot check
            REQUIRE(cg0_view != cg1_view);
            REQUIRE(const_cg0_view != const_cg1_view);
        }

        SECTION("operator!=(ContractedGaussian)") {
            // Just negates operator== so okay to spot check
            REQUIRE(cg0_view != cg1);
            REQUIRE(const_cg0_view != cg1);
        }
    }
}
