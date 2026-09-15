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

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/types/floating_point.hpp>
#include <utility>

using namespace chemist::experimental::detail_;
using test_chemist::as_double;

namespace {

/// The float analog of test_chemist::as_double
float as_float(const_float_reference value) { return value.value<float>(); }

} // namespace

TEST_CASE("experimental::detail_ float arithmetic") {
    float_type two(2.0);
    float_type three(3.0);
    float_type twenty_five(25.0);

    float_type two_f(2.0f);
    float_type three_f(3.0f);

    // long double is a floating-point type WTF knows about but chemist's
    // floating_point_types list deliberately does not include, so it is the
    // way to exercise the "type chemist does not know about" branch.
    float_type unknown(2.0L);

    SECTION("add") {
        REQUIRE(as_double(add(two, three)) == 5.0);
        REQUIRE(as_double(add(three, two)) == 5.0);

        // The result is held in the operands' concrete type, not widened
        REQUIRE(as_float(add(two_f, three_f)) == 5.0f);
        REQUIRE_THROWS_AS(add(two_f, three_f).as_view().value<double>(),
                          std::runtime_error);
    }

    SECTION("add rejects mixed concrete types") {
        REQUIRE_THROWS_AS(add(two, three_f), std::runtime_error);
        REQUIRE_THROWS_AS(add(two_f, three), std::runtime_error);
    }

    SECTION("add rejects a type chemist does not know about") {
        REQUIRE_THROWS_AS(add(unknown, unknown), std::runtime_error);
    }

    SECTION("subtract") {
        REQUIRE(as_double(subtract(three, two)) == 1.0);

        // Order matters
        REQUIRE(as_double(subtract(two, three)) == -1.0);

        REQUIRE(as_float(subtract(three_f, two_f)) == 1.0f);
    }

    SECTION("subtract rejects mixed concrete types") {
        REQUIRE_THROWS_AS(subtract(three, two_f), std::runtime_error);
    }

    SECTION("multiply") {
        REQUIRE(as_double(multiply(two, three)) == 6.0);
        REQUIRE(as_float(multiply(two_f, three_f)) == 6.0f);
    }

    SECTION("multiply rejects mixed concrete types") {
        REQUIRE_THROWS_AS(multiply(two, three_f), std::runtime_error);
    }

    SECTION("sqrt") {
        REQUIRE(as_double(sqrt(twenty_five)) == 5.0);
        REQUIRE(as_double(sqrt(float_type(0.0))) == 0.0);

        // Stays in the operand's concrete type
        REQUIRE(as_float(sqrt(float_type(25.0f))) == 5.0f);
    }

    SECTION("sqrt rejects a type chemist does not know about") {
        REQUIRE_THROWS_AS(sqrt(unknown), std::runtime_error);
    }

    SECTION("copy") {
        auto copied = copy(two);
        REQUIRE(as_double(copied) == 2.0);

        // The copy is deep: writing to the original leaves it alone
        two.as_view() = 42.0;
        REQUIRE(as_double(two) == 42.0);
        REQUIRE(as_double(copied) == 2.0);
    }

    SECTION("copy preserves the concrete type") {
        auto copied = copy(two_f);
        REQUIRE(as_float(copied) == 2.0f);
        REQUIRE_THROWS_AS(copied.as_view().value<double>(), std::runtime_error);
    }

    SECTION("copy rejects a type chemist does not know about") {
        REQUIRE_THROWS_AS(copy(unknown), std::runtime_error);
    }

    SECTION("assign") {
        assign(two.as_view(), three);
        REQUIRE(as_double(two) == 3.0);

        // It wrote through rather than rebinding: three is untouched
        REQUIRE(as_double(three) == 3.0);
    }

    SECTION("assign writes through an alias") {
        // This is the property PointView's assignment depends on. `alias` is
        // a second handle on two's storage, so assigning through it must be
        // visible in two.
        auto alias = two.as_view();
        assign(alias, three);
        REQUIRE(as_double(two) == 3.0);
    }

    SECTION("assign rejects mixed concrete types") {
        REQUIRE_THROWS_AS(assign(two.as_view(), three_f), std::runtime_error);

        // ... and left the target alone
        REQUIRE(as_double(two) == 2.0);
    }

    SECTION("assign rejects a type chemist does not know about") {
        REQUIRE_THROWS_AS(assign(unknown.as_view(), unknown),
                          std::runtime_error);
    }

    SECTION("to_string") {
        REQUIRE(to_string(two) == "2");
        REQUIRE(to_string(float_type(-1.5)) == "-1.5");
    }

    SECTION("to_string does not truncate a double") {
        // wtf::fp::FloatView::to_string uses the default stream precision,
        // which is six significant figures. Ours asks numeric_limits how many
        // digits the concrete type actually needs, so it keeps all fifteen.
        float_type a_third(1.0 / 3.0);
        REQUIRE(a_third.as_view().to_string() == "0.333333");
        REQUIRE(to_string(a_third) == "0.333333333333333");
    }

    SECTION("to_string uses the precision of the concrete type") {
        // float only carries six digits, so for it the two agree
        float_type a_third_f(1.0f / 3.0f);
        REQUIRE(to_string(a_third_f) == "0.333333");
    }

    SECTION("to_string rejects a type chemist does not know about") {
        REQUIRE_THROWS_AS(to_string(unknown), std::runtime_error);
    }

#ifdef ENABLE_SIGMA
    SECTION("Uncertainty-quantification types") {
        using udouble = tensorwrapper::types::udouble;

        float_type u1(udouble(2.0, 0.1));
        float_type u2(udouble(3.0, 0.2));

        SECTION("arithmetic stays in the UQ type") {
            auto sum   = add(u1, u2);
            auto value = sum.as_view().value<udouble>();
            REQUIRE(value.mean() == 5.0);

            // The uncertainty was propagated, not dropped
            REQUIRE(value.sd() > 0.0);
        }

        SECTION("multiply") {
            auto product = multiply(u1, u2);
            REQUIRE(product.as_view().value<udouble>().mean() == 6.0);
        }

        SECTION("sqrt") {
            float_type u25(udouble(25.0, 0.5));
            REQUIRE(sqrt(u25).as_view().value<udouble>().mean() == 5.0);
        }

        SECTION("copy") {
            REQUIRE(copy(u1).as_view().value<udouble>().mean() == 2.0);
        }

        SECTION("mixing a UQ type with a plain double is rejected") {
            REQUIRE_THROWS_AS(add(u1, two), std::runtime_error);
        }

        SECTION("to_string falls back to the default precision") {
            // numeric_limits is not specialized for the UQ types, so
            // to_string can not ask them how many digits they need; it must
            // still produce something rather than throwing.
            REQUIRE_FALSE(to_string(u1).empty());
        }
    }
#endif
}
