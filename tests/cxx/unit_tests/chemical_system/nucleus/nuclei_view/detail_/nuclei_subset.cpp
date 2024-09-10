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
#include <chemist/chemical_system/nucleus/nuclei_view/detail_/nuclei_subset.hpp>

using namespace chemist::detail_;

template<typename NucleiType>
void test_nuclei_subset() {
    using pimpl_type       = NucleiSubset<NucleiType>;
    using view_type        = typename pimpl_type::parent_type;
    using nucleus_type     = typename pimpl_type::value_type;
    using member_list_type = typename pimpl_type::member_list_type;

    // Create some Nucleus objects. Objects do not need to be physical, just
    // distinct.
    nucleus_type h0("H", 1ul, 1.0, 4.0, 7.0, 0.0, 3.0);
    nucleus_type h1("H", 1ul, 2.0, 5.0, 8.0, 1.0, 4.0);
    nucleus_type h2("H", 1ul, 3.0, 6.0, 9.0, 2.0, 5.0);

    NucleiType nuclei{h0, h1, h2};
    view_type supersys(nuclei);

    // Create pimpl_type objects
    pimpl_type defaulted;

    member_list_type indices{1, 2}, no_indices;
    pimpl_type empty(supersys, no_indices);
    pimpl_type has_values(supersys, indices);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }

        SECTION("value") {
            REQUIRE(empty.size() == 0);

            REQUIRE(has_values.size() == 2);
            REQUIRE(has_values.get_nuke(0) == supersys[1]);
            REQUIRE(has_values.get_nuke(1) == supersys[2]);
        }

        SECTION("copy") {
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy.size() == 0);

            pimpl_type empty_copy(empty);
            REQUIRE(empty_copy.size() == 0);

            pimpl_type has_values_copy(has_values);
            REQUIRE(has_values_copy.size() == 2);
            REQUIRE(has_values_copy.get_nuke(0) == supersys[1]);
            REQUIRE(has_values_copy.get_nuke(1) == supersys[2]);
        }
    }

    SECTION("clone") {
        auto pdefaulted = defaulted.clone();
        REQUIRE(pdefaulted->size() == 0);

        auto pempty = empty.clone();
        REQUIRE(pempty->size() == 0);

        auto phas_value = has_values.clone();
        REQUIRE(phas_value->size() == 2);
        REQUIRE(phas_value->get_nuke(0) == supersys[1]);
        REQUIRE(phas_value->get_nuke(1) == supersys[2]);
    }

    SECTION("get_nuke") {
        REQUIRE(has_values.get_nuke(0) == supersys[1]);
        REQUIRE(has_values.get_nuke(1) == supersys[2]);

        // If nuclei are mutable make sure we can mutate them
        using no_cv = std::remove_cv_t<NucleiType>;
        if constexpr(std::is_same_v<NucleiType, no_cv>) {
            has_values.get_nuke(0) = h0;
            REQUIRE(has_values.get_nuke(0) == h0);
            REQUIRE(supersys[1] == h0);
        }
    }

    SECTION("get_nuke()const") {
        REQUIRE(std::as_const(has_values).get_nuke(0) == supersys[1]);
        REQUIRE(std::as_const(has_values).get_nuke(1) == supersys[2]);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(empty.size() == 0);
        REQUIRE(has_values.size() == 2);
    }

    SECTION("operator==") {
        // Defaulted == defaulted
        REQUIRE(defaulted == pimpl_type{});

        // Defaulted vs. empty
        REQUIRE(defaulted == empty);

        // Defaulted != non-default
        REQUIRE_FALSE(defaulted == has_values);

        // Empty vs. empty
        pimpl_type empty2(supersys, no_indices);
        REQUIRE(empty == empty2);

        // Empty vs. non-empty
        REQUIRE_FALSE(empty == has_values);

        // Same everything
        pimpl_type has_values2(supersys, indices);
        REQUIRE(has_values == has_values2);

        // Different instance of same supersystem
        view_type supersys2(nuclei);
        pimpl_type has_values3(supersys2, indices);
        REQUIRE(has_values == has_values3);

        // Different supersystem (same atoms)
        nucleus_type h3("H", 1ul, 0.0, 1.1, 2.2, 3.3, 4.4);
        NucleiType nuclei2{h3, h1, h2};
        view_type supersys3(nuclei2);
        pimpl_type diff_ss(supersys3, indices);
        REQUIRE(has_values == diff_ss);

        // Different atoms
        member_list_type indices2{0, 1};
        pimpl_type diff_atoms(supersys, indices2);
        REQUIRE_FALSE(has_values == diff_atoms);
    }
}

TEST_CASE("NucleiSubset<Nuclei>") { test_nuclei_subset<chemist::Nuclei>(); }

TEST_CASE("NucleiSubset<const Nuclei>") {
    test_nuclei_subset<const chemist::Nuclei>();
}
