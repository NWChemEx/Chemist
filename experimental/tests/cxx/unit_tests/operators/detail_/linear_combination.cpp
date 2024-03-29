/*
 * Copyright 2022 NWChemEx-Project
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

#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/fock.hpp"
#include "chemist/operators/hamiltonian/hamiltonian.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace chemist;
using namespace chemist::operators;
using namespace chemist::operators::detail_;

inline constexpr const auto& as_derived_operator(const LinearCombination& op) {
    return op;
}

TEST_CASE("LinearCombination") {
    using chemist::Nucleus;
    chemist::Nuclei a{Nucleus("", 1ul, 0.0, 0.0, 0.0, 0.0)};
    chemist::Nuclei b{Nucleus("", 2ul, 0.0, 0.0, 0.0, 0.0)};

    ElectronNuclearAttraction V1(Electron{}, a);
    ElectronNuclearAttraction V2(Electron{}, b);

    Hamiltonian h_default, h_v1(V1), h_v2(V2);
    Fock f_default, f_v1(V1), f_v2(V2);

    SECTION("Default Comparisons: Nonpolymorphic") {
        Hamiltonian h_compare;
        REQUIRE(h_default == h_compare);
    }

    SECTION("Stateful Comparisons: Nonpolymorphic") {
        Hamiltonian h_cpy(h_v1);
        REQUIRE(h_v1 == h_cpy);
        REQUIRE(h_v1 != h_v2);
    }

    SECTION("Default + Stateful Comparisons: Nonpolymorphic") {
        REQUIRE(h_default != h_v1);
    }

    SECTION("Default Comparisons: Polymorphic") {
        auto& d_h = as_derived_operator(h_default);
        auto& d_f = as_derived_operator(f_default);

        REQUIRE(d_h == d_f); // Nonpolymorphic comparison

        REQUIRE(h_default.is_equal(d_h));
        REQUIRE(d_h.is_equal(h_default));
        REQUIRE(not f_default.is_equal(d_h));
        REQUIRE(not d_h.is_equal(f_default));
        REQUIRE(not d_h.is_equal(d_f));
        REQUIRE(not h_default.is_equal(d_f));
        REQUIRE(not d_f.is_equal(h_default));
        REQUIRE(not d_f.is_equal(d_h));
    }

    SECTION("Stateful Comparisons: Polymorhpic") {
        auto& d_h1 = as_derived_operator(h_v1);
        auto& d_h2 = as_derived_operator(h_v2);
        auto& d_f1 = as_derived_operator(f_v1);
        auto& d_f2 = as_derived_operator(f_v2);

        REQUIRE(d_h1 == d_f1);
        REQUIRE(d_h2 == d_f2);
        REQUIRE(d_h1 != d_f2);
        REQUIRE(d_h2 != d_f1);

        REQUIRE(h_v1.is_equal(d_h1));
        REQUIRE(d_h1.is_equal(h_v1));

        REQUIRE(not f_v1.is_equal(d_h1));
        REQUIRE(not d_f1.is_equal(h_v1));
        REQUIRE(not h_v1.is_equal(d_f1));
        REQUIRE(not d_h1.is_equal(f_v1));
    }
}
