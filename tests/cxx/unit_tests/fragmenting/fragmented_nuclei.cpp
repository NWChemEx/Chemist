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
#include <chemist/fragmenting/fragmented_nuclei.hpp>

using namespace chemist;
using namespace chemist::fragmenting;

TEST_CASE("FragmentedNuclei") {
    using set_type          = FragmentedNuclei;
    using supersystem_type  = typename set_type::supersystem_type;
    using nucleus_type      = typename supersystem_type::value_type;
    using fragment_map_type = typename set_type::fragment_map_type;

    nucleus_type h0("H", 1ul, 1.0, 2.0, 3.0, 4.0);
    nucleus_type h1("H", 1ul, 1.0, 5.0, 6.0, 7.0);
    nucleus_type h2("H", 1ul, 1.0, 8.0, 9.0, 0.0);

    supersystem_type ss{h0, h1, h2};

    fragment_map_type disjoint;
    disjoint.emplace({0});
    disjoint.emplace({1});
    disjoint.emplace({2});

    fragment_map_type nondisjoint;
    nondisjoint.emplace({0, 1});
    nondisjoint.emplace({1, 2});

    set_type null_set;
    set_type no_frags(ss);
    set_type disjoint_no_caps(ss, disjoint);
    set_type nondisjoint_no_caps(ss, nondisjoint);
}
