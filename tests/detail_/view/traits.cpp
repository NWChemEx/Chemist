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

#include <catch2/catch.hpp>
#include <chemist/detail_/view/traits.hpp>

using namespace chemist::detail_;

template<typename T>
class Point;

template<typename T>
class Charge;

TEMPLATE_TEST_CASE("ViewTraits", "", Point<float>, Charge<double>) {
    using T                 = TestType;
    using const_t           = const T;
    using view_traits       = ViewTraits<T>;
    using const_view_traits = ViewTraits<const_t>;

    SECTION("type") {
        using u       = typename view_traits::type;
        using const_u = typename const_view_traits::type;
        STATIC_REQUIRE(std::is_same_v<u, T>);
        STATIC_REQUIRE(std::is_same_v<u, T>);
    }

    SECTION("const_type") {
        using u       = typename view_traits::const_type;
        using const_u = typename const_view_traits::const_type;
        STATIC_REQUIRE(std::is_same_v<u, const_t>);
        STATIC_REQUIRE(std::is_same_v<u, const_t>);
    }

    SECTION("is_const_v") {
        STATIC_REQUIRE_FALSE(view_traits::is_const_v);
        STATIC_REQUIRE(const_view_traits::is_const_v);
    }

    SECTION("apply_const") {
        using u       = typename view_traits::template apply_const<int>;
        using const_u = typename const_view_traits::template apply_const<int>;

        STATIC_REQUIRE(std::is_same_v<u, int>);
        STATIC_REQUIRE(std::is_same_v<const_u, const int>);
    }

    SECTION("apply_const_ref") {
        using u = typename view_traits::template apply_const_ref<int>;
        using const_u =
          typename const_view_traits::template apply_const_ref<int>;

        STATIC_REQUIRE(std::is_same_v<u, int&>);
        STATIC_REQUIRE(std::is_same_v<const_u, const int&>);
    }

    SECTION("apply_const_ptr") {
        using u = typename view_traits::template apply_const_ptr<int>;
        using const_u =
          typename const_view_traits::template apply_const_ptr<int>;

        STATIC_REQUIRE(std::is_same_v<u, int*>);
        STATIC_REQUIRE(std::is_same_v<const_u, const int*>);
    }
}
