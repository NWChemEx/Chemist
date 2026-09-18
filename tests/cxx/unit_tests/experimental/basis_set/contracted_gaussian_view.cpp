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

// Only what ContractedGaussianView itself implements is tested here: its
// ctors (including the implicit conversions), assignment (which, unlike a
// normal copy, writes through the alias, and unlike PrimitiveView's must
// handle a size mismatch), swap, as_contracted_gaussian, and the aliasing
// behavior of its own coefficient_buffer_/exponent_buffer_/l_/center_.
// Everything ContractedGaussianView inherits from ContractedGaussianCommon
// is tested once, generically, in contracted_gaussian_common.cpp.

#include "../../test_helpers.hpp"
#include "../experimental_test_helpers.hpp"
#include <chemist/experimental/basis_set/contracted_gaussian_view.hpp>
#include <utility>
#include <vector>

using namespace chemist::experimental;
using test_chemist::as_double;

namespace {

/// Takes a read-only view by value, to prove conversions are implicit.
double coefficient0_of(const_contracted_gaussian_view cg) {
    return as_double(cg[0].get_coefficient());
}

} // namespace

TEST_CASE("experimental::ContractedGaussianView") {
    std::vector<double> cs{2.0, 3.0};
    std::vector<double> es{1.0, 2.0};
    ContractedGaussian s(cs.begin(), cs.end(), es.begin(), es.end(),
                         std::size_t(1), 1.0, 2.0, 3.0);

    std::vector<double> other_cs{9.0, 9.0};
    std::vector<double> other_es{9.0, 9.0};
    ContractedGaussian other(other_cs.begin(), other_cs.end(), other_es.begin(),
                             other_es.end(), std::size_t(2), 9.0, 9.0, 9.0);

    contracted_gaussian_view sv(s);
    const_contracted_gaussian_view csv(other);

    SECTION("Ctors and assignment") {
        SECTION("From a ContractedGaussian") {
            // *this aliases s's state through its own coefficient_buffer_/
            // exponent_buffer_/l_/center_, rather than copying it.
            REQUIRE(sv.size() == 2);
            REQUIRE(sv.get_l() == 1);
            REQUIRE(sv.get_center() == s.get_center());

            s.set_l(4ul);
            REQUIRE(sv.get_l() == 4);
        }

        SECTION("From a ContractedGaussian, implicitly") {
            // Would not compile if the ctor were explicit
            REQUIRE(coefficient0_of(s) == 2.0);
        }

        SECTION("From already-aliased parts") {
            // A standalone ContractedGaussian, plus a loose l and center, in
            // the same spirit as PrimitiveView's "From already-aliased
            // state" test: the view aliases parts that were never assembled
            // into a ContractedGaussian of their own.
            ContractedGaussian tmp(cs.begin(), cs.end(), es.begin(), es.end(),
                                   std::size_t(0), 0.0, 0.0, 0.0);
            auto coefficients = tmp.get_coefficient_buffer();
            auto exponents    = tmp.get_exponent_buffer();
            ContractedGaussian::angular_momentum_type l_value = 1;
            Point center(1.0, 2.0, 3.0);
            contracted_gaussian_view from_parts(coefficients, exponents,
                                                l_value, center);
            REQUIRE(from_parts == s);

            // The view aliases the loose l, it did not copy it
            from_parts.set_l(7ul);
            REQUIRE(l_value == 7);
        }

        SECTION("Mismatched buffer lengths throw") {
            std::vector<double> shorter_es{1.0};
            ContractedGaussian shorter(cs.begin(), cs.end() - 1,
                                       shorter_es.begin(), shorter_es.end(),
                                       std::size_t(0), 0.0, 0.0, 0.0);
            auto coefficients = s.get_coefficient_buffer();
            auto exponents    = shorter.get_exponent_buffer();
            ContractedGaussian::angular_momentum_type l_value = 0;
            Point center(0.0, 0.0, 0.0);
            REQUIRE_THROWS_AS(contracted_gaussian_view(coefficients, exponents,
                                                       l_value, center),
                              std::invalid_argument);
        }

        SECTION("Mutable to read-only, implicitly") {
            // Would not compile if the conversion were explicit
            REQUIRE(coefficient0_of(sv) == 2.0);

            const_contracted_gaussian_view as_const(sv);
            REQUIRE(as_const == sv);
        }

        SECTION("Copy ctor is shallow") {
            contracted_gaussian_view copy(sv);
            REQUIRE(copy == sv);

            copy.set_l(7ul);
            REQUIRE(s.get_l() == 7);
        }

        SECTION("Move ctor") {
            contracted_gaussian_view moved(std::move(sv));
            REQUIRE(moved == s);
            moved.set_l(7ul);
            REQUIRE(s.get_l() == 7);
        }

        SECTION("Copy assignment writes through") {
            // Unlike the copy ctor, assignment does NOT rebind *this; it
            // overwrites whatever *this already aliases.
            contracted_gaussian_view other_view(other);

            sv = other_view;

            // s, not sv's binding, is what changed
            REQUIRE(s == other);
        }

        SECTION("Assignment from a ContractedGaussian writes through") {
            sv = other;
            REQUIRE(s == other);
        }

        SECTION("Assignment from a read-only view writes through") {
            sv = csv;
            REQUIRE(s == other);
        }

        SECTION("Assignment throws on a size mismatch") {
            std::vector<double> shorter_cs{1.0};
            std::vector<double> shorter_es{1.0};
            ContractedGaussian shorter(shorter_cs.begin(), shorter_cs.end(),
                                       shorter_es.begin(), shorter_es.end(),
                                       std::size_t(0), 0.0, 0.0, 0.0);
            contracted_gaussian_view shorter_view(shorter);
            REQUIRE_THROWS_AS(sv = shorter_view, std::runtime_error);
        }
    }

    SECTION("swap rebinds") {
        contracted_gaussian_view other_view(other);

        sv.swap(other_view);

        // The views traded places; neither ContractedGaussian changed
        REQUIRE(sv == other);
        REQUIRE(other_view == s);
    }

    SECTION("as_contracted_gaussian is a deep copy") {
        auto copy = sv.as_contracted_gaussian();
        REQUIRE(copy == s);

        copy.set_l(7ul);
        REQUIRE(s.get_l() == 1);
    }

    SECTION("Writing through the view mutates the aliased ContractedGaussian") {
        sv.set_l(6ul);
        sv.set_center(Point(4.0, 5.0, 6.0));
        REQUIRE(s.get_l() == 6);
        REQUIRE(s.get_center() == Point(4.0, 5.0, 6.0));

        sv[0].set_coefficient(42.0);
        REQUIRE(as_double(s[0].get_coefficient()) == 42.0);
    }
}
