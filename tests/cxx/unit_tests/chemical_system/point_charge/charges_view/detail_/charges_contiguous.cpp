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

#include "../../../../catch.hpp"
#include <chemist/chemical_system/point_charge/charges_view/detail_/charges_contiguous.hpp>

template<typename ChargesType>
void test_case_guts() {
    using charges_type = ChargesType;
    using pimpl_type   = chemist::detail_::ChargesContiguous<charges_type>;
    using charge_type  = typename pimpl_type::point_charge_traits::charge_type;
    using point_set_reference = typename pimpl_type::point_set_reference;
    using const_point_set_reference =
      typename pimpl_type::const_point_set_reference;
    using point_set_type = typename pimpl_type::point_set_traits::value_type;
    using point_type     = typename point_set_type::value_type;
    using reference      = typename pimpl_type::reference;

    // Make some Point objects and put them in a PointSet
    point_type p0{0.0, 0.0, 0.0}, p1{1.0, 2.0, 3.0}, p2{4.0, 5.0, 6.0};
    point_set_type no_points;
    point_set_type points{p0, p1, p2};

    // Make some charges
    std::vector<charge_type> qs{-1.1, 2.2, -3.3};

    // These are the point charges we effectively just made
    reference q0(qs[0], p0);
    reference q1(qs[1], p1);
    reference q2(qs[2], p2);

    pimpl_type defaulted;
    pimpl_type no_charges(point_set_reference(no_points), nullptr);
    pimpl_type charges(point_set_reference(points), qs.data());

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }

        SECTION("Value") {
            REQUIRE(no_charges.size() == 0);
            REQUIRE(charges.size() == 3);
            REQUIRE(charges[0] == q0);
            REQUIRE(charges[1] == q1);
            REQUIRE(charges[2] == q2);
        }

        SECTION("Copy") {
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted == defaulted_copy);

            pimpl_type no_charges_copy(no_charges);
            REQUIRE(no_charges_copy == no_charges);

            pimpl_type charges_copy(charges);
            REQUIRE(charges_copy == charges);
        }
    }

    SECTION("clone") {
        auto defaulted_copy = defaulted.clone();
        REQUIRE(defaulted_copy->are_equal(defaulted));

        auto no_charges_copy = no_charges.clone();
        REQUIRE(no_charges_copy->are_equal(no_charges));

        auto charges_copy = charges.clone();
        REQUIRE(charges_copy->are_equal(charges));
    }

    // n.b. base class does bounds checks our checks should be in bounds

    SECTION("point_set()") {
        REQUIRE(defaulted.point_set() == point_set_reference{});
        REQUIRE(no_charges.point_set() == point_set_reference{});
        REQUIRE(charges.point_set() == point_set_reference{points});
    }

    SECTION("point_set() const") {
        REQUIRE(std::as_const(defaulted).point_set() ==
                const_point_set_reference{});
        REQUIRE(std::as_const(no_charges).point_set() ==
                const_point_set_reference{});
        REQUIRE(std::as_const(charges).point_set() ==
                const_point_set_reference{points});
    }

    SECTION("at_()") {
        REQUIRE(charges[0] == q0);
        REQUIRE(charges[1] == q1);
        REQUIRE(charges[2] == q2);
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(charges)[0] == q0);
        REQUIRE(std::as_const(charges)[1] == q1);
        REQUIRE(std::as_const(charges)[2] == q2);
    }

    SECTION("size_()") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(no_charges.size() == 0);
        REQUIRE(charges.size() == 3);
    }

    SECTION("operator==") {
        SECTION("Default vs default") { REQUIRE(defaulted == pimpl_type{}); }
        SECTION("Default vs. empty") { REQUIRE(defaulted == no_charges); }
        SECTION("Default vs. non-empty") {
            REQUIRE_FALSE(defaulted == charges);
        }
        SECTION("Empty vs. empty") {
            // N.b., non-null address to verify addresses aren't compared
            pimpl_type no_charges2(point_set_reference(no_points), qs.data());
            REQUIRE(no_charges == no_charges2);
        }
        SECTION("Empty vs. non-empty") { REQUIRE_FALSE(no_charges == charges); }
        SECTION("Same non-empty state") {
            pimpl_type charges2(point_set_reference(points), qs.data());
            REQUIRE(charges == charges2);
        }
        SECTION("Same non-empty state, different charge address") {
            std::vector<charge_type> qs2(qs);
            pimpl_type charges2(point_set_reference(points), qs2.data());
            REQUIRE(charges == charges2);
        }
        SECTION("Same non-empty state, different PointSet") {
            point_set_type points2{p0, p1, p2};
            pimpl_type charges2(point_set_reference(points2), qs.data());
            REQUIRE(charges == charges2);
        }
        SECTION("Different points") {
            // n.b. we switched the order
            point_set_type points2{p1, p0, p2};
            pimpl_type charges2(point_set_reference(points2), qs.data());
            REQUIRE_FALSE(charges == charges2);
        }
        SECTION("Different charges") {
            std::vector<charge_type> qs2{-9.9, -8.8, -7.7};
            pimpl_type charges2(point_set_reference(points), qs2.data());
            REQUIRE_FALSE(charges == charges2);
        }
    }
}

TEMPLATE_TEST_CASE("ChargesContiguous<T>", "", float, double) {
    using charges_type = chemist::Charges<TestType>;
    test_case_guts<charges_type>();
}

TEMPLATE_TEST_CASE("ChargesContiguous<const T>", "", float, double) {
    using charges_type = chemist::Charges<TestType>;
    test_case_guts<const charges_type>();
}
