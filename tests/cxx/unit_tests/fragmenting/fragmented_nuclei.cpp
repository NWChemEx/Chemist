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

using types2test = std::tuple<Nuclei, const Nuclei>;

TEMPLATE_LIST_TEST_CASE("FragmentedNuclei", "", types2test) {
    // Type we're testing
    using set_type = FragmentedNuclei<TestType>;

    // Non-cv qualified supersystem type
    using supersystem_type = typename set_type::supersystem_type;

    // Non-cv qualified type of a fragment
    using fragment_type = typename set_type::value_type;

    // Non-cv qualified type of each Nucleus
    using nucleus_type = typename supersystem_type::value_type;

    // Type used to map fragment index to a set of nuclei indices
    using fragment_map_type = typename set_type::nucleus_map_type;

    // Type used to hold the nuclei indices
    using index_set_type = typename set_type::nucleus_index_set;

    // Type used to hold caps
    using cap_set_type = typename set_type::cap_set_type;

    // Type of each cap
    using cap_type = typename cap_set_type::value_type;

    nucleus_type h0("H", 1ul, 1.0, 2.0, 3.0, 4.0);
    nucleus_type h1("H", 1ul, 1.0, 5.0, 6.0, 7.0);
    nucleus_type h2("H", 1ul, 1.0, 8.0, 9.0, 0.0);

    supersystem_type ss{h0, h1, h2};

    fragment_map_type disjoint;
    disjoint.push_back(index_set_type{0});
    disjoint.push_back(index_set_type{1});
    disjoint.push_back(index_set_type{2});

    fragment_map_type nondisjoint;
    nondisjoint.push_back(index_set_type{0, 1});
    nondisjoint.push_back(index_set_type{1, 2});

    set_type null_set;
    set_type no_frags(ss);
    set_type disjoint_no_caps(ss, disjoint);
    set_type nondisjoint_no_caps(ss, nondisjoint);

    // Now we'll pretend h0 and h1 are bonded, then we need two caps:
    cap_type c01(0, 1, h1); // Cap replacing h1
    cap_type c10(1, 0, h0); // Cap replacing h0
    cap_set_type caps{c01, c10};

    set_type disjoint_caps(ss, disjoint, caps);
    set_type nondisjoint_caps(ss, nondisjoint, caps);

    SECTION("CTors and assignment") {
        SECTION("Default") {
            REQUIRE(null_set.is_null());
            REQUIRE(null_set.size() == 0);
        }

        SECTION("Empty") {
            REQUIRE_FALSE(no_frags.is_null());
            REQUIRE(no_frags.size() == 0);
            REQUIRE(no_frags.supersystem() == ss);
        }

        SECTION("Frags, but no caps") {
            REQUIRE_FALSE(disjoint_no_caps.is_null());
            REQUIRE(disjoint_no_caps.size() == 3);
            REQUIRE(disjoint_no_caps[0] == fragment_type{h0});
            REQUIRE(disjoint_no_caps[1] == fragment_type{h1});
            REQUIRE(disjoint_no_caps[2] == fragment_type{h2});

            REQUIRE_FALSE(nondisjoint_no_caps.is_null());
            REQUIRE(nondisjoint_no_caps.size() == 2);
            REQUIRE(nondisjoint_no_caps[0] == fragment_type{h0, h1});
            REQUIRE(nondisjoint_no_caps[1] == fragment_type{h1, h2});
        }
    }

    SECTION("at_()") {}

    SECTION("at_()const") {}

    SECTION("size_") {
        REQUIRE(null_set.size() == 0);
        REQUIRE(no_frags.size() == 0);
        REQUIRE(disjoint_no_caps.size() == 3);
        REQUIRE(nondisjoint_no_caps.size() == 2);
        REQUIRE(disjoint_caps.size() == 3);
        REQUIRE(nondisjoint_caps.size() == 2);
    }
}
