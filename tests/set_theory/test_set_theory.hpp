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

/** @file test_set_theory.hpp
 *
 *  This header factors out some common headers, types, and functions used while
 *  testing the various parts of the set theory component.
 */

#pragma once
#include "chemist/molecule/molecule.hpp"
#include <catch2/catch.hpp>
#include <tuple>
#include <vector>

using namespace chemist;
using container_types = std::tuple<std::vector<int>, Molecule>;

namespace testing {
// Makes a non-default object of the specified type that we can partition
template<typename T>
auto make_object() {
    if constexpr(std::is_same_v<T, std::vector<int>>) {
        return T{1, 2, 3};
    } else if constexpr(std::is_same_v<T, chemist::Molecule>) {
        using value_type = typename Molecule::value_type;
        using cart_type  = typename value_type::coord_type;
        value_type O{8ul, cart_type{0.0, 0.0, 0.0}};
        value_type H0{1ul, cart_type{0.0, 0.0, 0.5}};
        value_type H1{1ul, cart_type{0.0, 0.5, 0.0}};
        return T{O, H0, H1};
    }
}

} // namespace testing
