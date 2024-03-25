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

#include <catch2/catch.hpp>
#include <chemist/chemical_system/point_charge/charges_view/detail_/charges_contiguous.hpp>

template<typename ChargesType>
void test_case_guts() {
    using charges_type = ChargesType;
    using pimpl_type   = chemist::detail_::ChargesContiguous<charges_type>;
    using charge_type  = typename pimpl_type::point_charge_traits::charge_type;
    using point_set_reference = typename pimpl_type::point_set_reference;
    using point_set_type      = typename;

    point_set_type points{1.0, 2.0, 3.0};
    std::vector<charges_type> qs{4.0, 5.0, -6.0};
    point_set_reference no_points;
    point_set_reference with_points(points);

    pimpl_type defaulted;
    pimpl_type no_charges(no_points, nullptr);
    pimpl_type charges(with_points, qs.data());

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") { REQUIRE(no_charges.size() == 0); }
    }
}

TEMPLATE_TEST_CASE("ChargesContiguous<T>", "", float, double) {
    using charges_type = chemist::Charges<TestType>;
    test_case_guts<charges_type>();
    // test_case_guts<const charges_type>();
}

TEMPLATE_TEST_CASE("ChargesContiguous<const T>", "", float, double) {
    using charges_type = chemist::Charges<TestType>;
    test_case_guts<const charges_type>();
}
