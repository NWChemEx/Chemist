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

#include "../../../../catch.hpp"
#include <chemist/chemical_system/nucleus/nuclei_view/detail_/nuclei_union.hpp>

using namespace chemist::detail_;

template<typename NucleiType>
void test_nuclei_union() {
    using pimpl_type       = NucleiUnion<NucleiType>;
    using view_type        = typename pimpl_type::parent_type;
    using nucleus_type     = typename pimpl_type::value_type;
    using nuclei_container = typename pimpl_type::nuclei_view_container;

    // Create some Nucleus objects. Objects do not need to be physical, just
    // distinct.
    nucleus_type h0("H", 1ul, 1.0, 4.0, 7.0, 0.0, 3.0);
    nucleus_type h1("H", 1ul, 2.0, 5.0, 8.0, 1.0, 4.0);
    nucleus_type h2("H", 1ul, 3.0, 6.0, 9.0, 2.0, 5.0);

    NucleiType n01{h0, h1}, n2{h2};

    nuclei_container no_nuclei;
    nuclei_container nuclei{view_type(n01), view_type(n2)};

    // Create pimpl_type objects
    pimpl_type defaulted;
    pimpl_type empty(no_nuclei);
    pimpl_type has_values(nuclei);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }

        SECTION("value") {
            REQUIRE(empty.size() == 0);

            REQUIRE(has_values.size() == 3);
            REQUIRE(has_values.get_nuke(0) == h0);
            REQUIRE(has_values.get_nuke(1) == h1);
            REQUIRE(has_values.get_nuke(2) == h2);

            // Three partitions (assumes we can repeat elements)
            nuclei_container c2{view_type{n01}, view_type{n01}, view_type{n2}};
            pimpl_type has_values2(c2);
            REQUIRE(has_values2.size() == 5);
            REQUIRE(has_values2.get_nuke(0) == h0);
            REQUIRE(has_values2.get_nuke(1) == h1);
            REQUIRE(has_values2.get_nuke(2) == h0);
            REQUIRE(has_values2.get_nuke(3) == h1);
            REQUIRE(has_values2.get_nuke(4) == h2);
        }

        SECTION("copy") {
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy.size() == 0);

            pimpl_type empty_copy(empty);
            REQUIRE(empty_copy.size() == 0);

            pimpl_type has_values_copy(has_values);
            REQUIRE(has_values_copy.size() == 3);
            REQUIRE(has_values_copy.get_nuke(0) == h0);
            REQUIRE(has_values_copy.get_nuke(1) == h1);
            REQUIRE(has_values_copy.get_nuke(2) == h2);
        }
    }

    SECTION("clone") {
        auto pdefaulted = defaulted.clone();
        REQUIRE(pdefaulted->size() == 0);

        auto pempty = empty.clone();
        REQUIRE(pempty->size() == 0);

        auto phas_value = has_values.clone();
        REQUIRE(phas_value->size() == 3);
        REQUIRE(phas_value->get_nuke(0) == h0);
        REQUIRE(phas_value->get_nuke(1) == h1);
        REQUIRE(phas_value->get_nuke(2) == h2);
    }

    SECTION("get_nuke") {
        REQUIRE(has_values.get_nuke(0) == h0);
        REQUIRE(has_values.get_nuke(1) == h1);
        REQUIRE(has_values.get_nuke(2) == h2);

        // If nuclei are mutable make sure we can mutate them
        using no_cv = std::remove_cv_t<NucleiType>;
        if constexpr(std::is_same_v<NucleiType, no_cv>) {
            has_values.get_nuke(1) = h0;
            REQUIRE(has_values.get_nuke(1) == h0);
            REQUIRE(n01[1] == h0);
        }
    }

    SECTION("get_nuke()const") {
        REQUIRE(std::as_const(has_values).get_nuke(0) == h0);
        REQUIRE(std::as_const(has_values).get_nuke(1) == h1);
        REQUIRE(std::as_const(has_values).get_nuke(2) == h2);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(empty.size() == 0);
        REQUIRE(has_values.size() == 3);
    }

    SECTION("operator==") {
        // Defaulted == defaulted
        REQUIRE(defaulted == pimpl_type{});

        // Defaulted vs. empty
        REQUIRE(defaulted == empty);

        // Defaulted != non-default
        REQUIRE_FALSE(defaulted == has_values);

        // Empty vs. empty
        pimpl_type empty2(no_nuclei);
        REQUIRE(empty == empty2);

        // Empty vs. non-empty
        REQUIRE_FALSE(empty == has_values);

        // Same everything
        pimpl_type has_values2(nuclei);
        REQUIRE(has_values == has_values2);

        // Different views of the same nuclei
        NucleiType n01_2{h0, h1}, n2_2{h2};
        nuclei_container nuclei2{view_type{n01_2}, view_type{n2_2}};
        pimpl_type has_values3(nuclei2);
        REQUIRE(has_values == has_values3);

        // Different partition
        NucleiType n0{h0}, n12{h1, h2};
        nuclei_container nuclei3{view_type{n0}, view_type{n12}};
        pimpl_type has_values4(nuclei3);
        REQUIRE(has_values == has_values4);

        // Different members
        nuclei_container nuclei4{view_type{n0}};
        pimpl_type diff_atoms(nuclei4);
        REQUIRE_FALSE(has_values == diff_atoms);
    }
}

TEST_CASE("NucleiUnion<Nuclei>") { test_nuclei_union<chemist::Nuclei>(); }

TEST_CASE("NucleiUnion<const Nuclei>") {
    test_nuclei_union<const chemist::Nuclei>();
}
