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

    // Type of a reference to a fragment
    using fragment_reference = typename set_type::reference;

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
    index_set_type i0{0}, i1{1}, i2{2}, i01{0, 1}, i12{1, 2};

    fragment_map_type disjoint;
    disjoint.push_back(i0);
    disjoint.push_back(i1);
    disjoint.push_back(i2);

    fragment_map_type nondisjoint;
    nondisjoint.push_back(i01);
    nondisjoint.push_back(i12);

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

    // The correct fragments for non-capped on
    using rvector = typename fragment_reference::reference_container;
    fragment_reference corr0(rvector{ss[0]});
    fragment_reference corr1(rvector{ss[1]});
    fragment_reference corr2(rvector{h2});
    fragment_reference corr01(rvector{ss[0], ss[1]});
    fragment_reference corr12(rvector{ss[1], ss[2]});

    // The correct fragments with caps
    fragment_reference caps0, caps1, caps12;
    if constexpr(std::is_same_v<TestType, std::remove_cv_t<TestType>>) {
        caps0  = caps.get_cap_nuclei(i0.begin(), i0.end());
        caps1  = caps.get_cap_nuclei(i1.begin(), i1.end());
        caps12 = caps.get_cap_nuclei(i12.begin(), i12.end());
    } else {
        const auto& ccaps = std::as_const(caps);
        caps0             = ccaps.get_cap_nuclei(i0.begin(), i0.end());
        caps1             = ccaps.get_cap_nuclei(i1.begin(), i1.end());
        caps12            = ccaps.get_cap_nuclei(i12.begin(), i12.end());
    }
    fragment_reference corr0_cap{std::vector{corr0, caps0}};
    fragment_reference corr1_cap{std::vector{corr1, caps1}};
    fragment_reference corr12_cap{std::vector{corr12, caps12}};

    // N.B. corr2_cap == corr2 & corr01_cap == corr01

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
            REQUIRE(disjoint_no_caps[0] == corr0);
            REQUIRE(disjoint_no_caps[1] == corr1);
            REQUIRE(disjoint_no_caps[2] == corr2);

            REQUIRE_FALSE(nondisjoint_no_caps.is_null());
            REQUIRE(nondisjoint_no_caps.size() == 2);
            REQUIRE(nondisjoint_no_caps[0] == corr01);
            REQUIRE(nondisjoint_no_caps[1] == corr12);
        }

        SECTION("Frags with caps") {
            REQUIRE_FALSE(disjoint_caps.is_null());
            REQUIRE(disjoint_caps.size() == 3);
            REQUIRE(disjoint_caps[0] == corr0_cap);
            REQUIRE(disjoint_caps[1] == corr1_cap);
            REQUIRE(disjoint_caps[2] == corr2);

            REQUIRE_FALSE(nondisjoint_caps.is_null());
            REQUIRE(nondisjoint_caps.size() == 2);
            REQUIRE(nondisjoint_caps[0] == corr01);
            REQUIRE(nondisjoint_caps[1] == corr12_cap);
        }

        SECTION("copy") {
            set_type null_set_copy(null_set);
            REQUIRE(null_set_copy == null_set);

            set_type no_frags_copy(no_frags);
            REQUIRE(no_frags_copy == no_frags);

            set_type disjoint_no_caps_copy(disjoint_no_caps);
            REQUIRE(disjoint_no_caps_copy == disjoint_no_caps);

            set_type nondisjoint_caps_copy(nondisjoint_caps);
            REQUIRE(nondisjoint_caps_copy == nondisjoint_caps);
        }

        SECTION("move") {
            set_type null_set_copy(null_set);
            set_type null_set_move(std::move(null_set));
            REQUIRE(null_set_copy == null_set_move);

            set_type no_frags_copy(no_frags);
            set_type no_frags_move(std::move(no_frags));
            REQUIRE(no_frags_copy == no_frags_move);

            set_type disjoint_no_caps_copy(disjoint_no_caps);
            set_type disjoint_no_caps_move(std::move(disjoint_no_caps));
            REQUIRE(disjoint_no_caps_copy == disjoint_no_caps_move);

            set_type nondisjoint_caps_copy(nondisjoint_caps);
            set_type nondisjoint_caps_move(std::move(nondisjoint_caps));
            REQUIRE(nondisjoint_caps_copy == nondisjoint_caps_move);
        }

        SECTION("copy assignment") {
            set_type null_set_copy;
            auto pnull_set_copy = &(null_set_copy = null_set);
            REQUIRE(pnull_set_copy == &null_set_copy);
            REQUIRE(null_set_copy == null_set);

            set_type no_frags_copy;
            auto pno_frags_copy = &(no_frags_copy = no_frags);
            REQUIRE(pno_frags_copy == &no_frags_copy);
            REQUIRE(no_frags_copy == no_frags);

            set_type disjoint_no_caps_copy;
            auto pdisjoint_no_caps_copy =
              &(disjoint_no_caps_copy = disjoint_no_caps);
            REQUIRE(pdisjoint_no_caps_copy == &disjoint_no_caps_copy);
            REQUIRE(disjoint_no_caps_copy == disjoint_no_caps);

            set_type nondisjoint_caps_copy;
            auto pnondisjoint_caps_copy =
              &(nondisjoint_caps_copy = nondisjoint_caps);
            REQUIRE(pnondisjoint_caps_copy == &nondisjoint_caps_copy);
            REQUIRE(nondisjoint_caps_copy == nondisjoint_caps);
        }

        SECTION("move assignment") {
            set_type null_set_copy(null_set);
            set_type null_set_move;
            auto pnull_set_move = &(null_set_move = std::move(null_set));
            REQUIRE(pnull_set_move == &null_set_move);
            REQUIRE(null_set_copy == null_set_move);

            set_type no_frags_copy(no_frags);
            set_type no_frags_move;
            auto pno_frags_move = &(no_frags_move = std::move(no_frags));
            REQUIRE(pno_frags_move == &no_frags_move);
            REQUIRE(no_frags_copy == no_frags_move);

            set_type disjoint_no_caps_copy(disjoint_no_caps);
            set_type disjoint_no_caps_move;
            auto pdisjoint_no_caps_move =
              &(disjoint_no_caps_move = std::move(disjoint_no_caps));
            REQUIRE(pdisjoint_no_caps_move == &disjoint_no_caps_move);
            REQUIRE(disjoint_no_caps_copy == disjoint_no_caps_move);

            set_type nondisjoint_caps_copy(nondisjoint_caps);
            set_type nondisjoint_caps_move;
            auto pnondisjoint_caps_move =
              &(nondisjoint_caps_move = std::move(nondisjoint_caps));
            REQUIRE(pnondisjoint_caps_move == &nondisjoint_caps_move);
            REQUIRE(nondisjoint_caps_copy == nondisjoint_caps_move);
        }
    }

    SECTION("swap") {
        set_type lhs_copy(nondisjoint_caps);
        set_type rhs_copy(no_frags);
        nondisjoint_caps.swap(no_frags);
        REQUIRE(nondisjoint_caps == rhs_copy);
        REQUIRE(no_frags == lhs_copy);
    }

    SECTION("at_()") {
        REQUIRE(disjoint_no_caps[0] == corr0);
        REQUIRE(disjoint_no_caps[1] == corr1);
        REQUIRE(disjoint_no_caps[2] == corr2);
        REQUIRE(nondisjoint_no_caps[0] == corr01);
        REQUIRE(nondisjoint_no_caps[1] == corr12);
        REQUIRE(disjoint_caps[0] == corr0_cap);
        REQUIRE(disjoint_caps[1] == corr1_cap);
        REQUIRE(disjoint_caps[2] == corr2);
        REQUIRE(nondisjoint_caps[0] == corr01);
        REQUIRE(nondisjoint_caps[1] == corr12_cap);
    }

    SECTION("at_()const") {
        REQUIRE(std::as_const(disjoint_no_caps)[0] == corr0);
        REQUIRE(std::as_const(disjoint_no_caps)[1] == corr1);
        REQUIRE(std::as_const(disjoint_no_caps)[2] == corr2);
        REQUIRE(std::as_const(nondisjoint_no_caps)[0] == corr01);
        REQUIRE(std::as_const(nondisjoint_no_caps)[1] == corr12);
        REQUIRE(std::as_const(disjoint_caps)[0] == corr0_cap);
        REQUIRE(std::as_const(disjoint_caps)[1] == corr1_cap);
        REQUIRE(std::as_const(disjoint_caps)[2] == corr2);
        REQUIRE(std::as_const(nondisjoint_caps)[0] == corr01);
        REQUIRE(std::as_const(nondisjoint_caps)[1] == corr12_cap);
    }

    SECTION("size_") {
        REQUIRE(null_set.size() == 0);
        REQUIRE(no_frags.size() == 0);
        REQUIRE(disjoint_no_caps.size() == 3);
        REQUIRE(nondisjoint_no_caps.size() == 2);
        REQUIRE(disjoint_caps.size() == 3);
        REQUIRE(nondisjoint_caps.size() == 2);
    }

    SECTION("operator==") {
        SECTION("null vs. null") {
            set_type rhs;
            REQUIRE(null_set == rhs);
        }

        SECTION("null vs. empty") { REQUIRE_FALSE(null_set == no_frags); }

        SECTION("null vs. value") {
            REQUIRE_FALSE(null_set == disjoint_no_caps);
        }

        SECTION("empty vs. empty") {
            set_type rhs(ss);
            REQUIRE(no_frags == rhs);
        }

        SECTION("empty vs. value") {
            REQUIRE_FALSE(no_frags == disjoint_no_caps);
        }

        SECTION("value vs. value") {
            set_type rhs(ss, disjoint, caps);
            REQUIRE(disjoint_caps == rhs);
        }

        SECTION("different frags") {
            REQUIRE_FALSE(disjoint_no_caps == nondisjoint_no_caps);
        }

        SECTION("different caps") {
            REQUIRE_FALSE(nondisjoint_no_caps == nondisjoint_caps);
        }
    }

    SECTION("operator!=") {
        // N.b. defined in terms of operator== so just spot check
        REQUIRE(null_set != disjoint_no_caps);
        REQUIRE_FALSE(null_set != set_type{});
    }
}
