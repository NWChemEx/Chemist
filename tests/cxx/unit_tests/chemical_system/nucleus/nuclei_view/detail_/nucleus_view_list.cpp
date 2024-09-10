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

#include "../../../../catch.hpp"
#include <chemist/chemical_system/nucleus/nuclei_view/detail_/nucleus_view_list.hpp>

template<typename NucleiType>
void test_nucleus_view_list() {
    using nuclei_type         = NucleiType;
    using pimpl_type          = chemist::detail_::NucleusViewList<nuclei_type>;
    using nucleus_type        = typename pimpl_type::nucleus_traits::value_type;
    using reference           = typename pimpl_type::reference;
    using reference_container = typename pimpl_type::reference_container;

    nucleus_type h("H", 1ul, 1.0, 2.0, 3.0, 4.0);
    nucleus_type he("He", 2ul, 4.0, 5.0, 6.0, 7.0);
    nucleus_type li("Li", 3ul, 6.0, 7.0, 8.0, 9.0);

    reference_container no_nuclei;
    reference_container nuclei{reference(h), reference(he), reference(li)};

    // Values we're testing
    pimpl_type defaulted;
    pimpl_type empty(no_nuclei);
    pimpl_type values(nuclei);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") {
            REQUIRE(empty.size() == 0);

            REQUIRE(values.size() == 3);
            REQUIRE(values.get_nuke(0) == h);
            REQUIRE(values.get_nuke(1) == he);
            REQUIRE(values.get_nuke(2) == li);
        }
        SECTION("Copy") {
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            pimpl_type empty_copy(empty);
            REQUIRE(empty_copy == empty);

            pimpl_type values_copy(values);
            REQUIRE(values_copy == values);
        }
    }

    SECTION("clone") {
        auto pdefaulted_copy = defaulted.clone();
        REQUIRE(pdefaulted_copy->are_equal(defaulted));

        auto pempty_copy = empty.clone();
        REQUIRE(pempty_copy->are_equal(empty));

        auto pvalues_copy = values.clone();
        REQUIRE(pvalues_copy->are_equal(values));
    }

    SECTION("get_nuke()") {
        REQUIRE(values.get_nuke(0) == h);
        REQUIRE(values.get_nuke(1) == he);
        REQUIRE(values.get_nuke(2) == li);

        // Is it writeable?
        using type_wo_cv = std::remove_cv_t<NucleiType>;
        if constexpr(std::is_same_v<NucleiType, type_wo_cv>) {
            auto h_inside   = values.get_nuke(0);
            h_inside.mass() = 99.0;
            REQUIRE(h_inside.mass() == 99.0);
            REQUIRE(values.get_nuke(0).mass() == 99.0);
            REQUIRE(h.mass() == 99.0);
        }
    }

    SECTION("get_nuke() const") {
        REQUIRE(std::as_const(values).get_nuke(0) == h);
        REQUIRE(std::as_const(values).get_nuke(1) == he);
        REQUIRE(std::as_const(values).get_nuke(2) == li);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(empty.size() == 0);
        REQUIRE(values.size() == 3);
    }

    SECTION("operator==") {
        SECTION("Defaulted vs. defaulted") {
            REQUIRE(defaulted == pimpl_type{});
        }

        SECTION("Defaulted vs. empty") { REQUIRE(defaulted == empty); }

        SECTION("Defaulted vs. non-empty") {
            REQUIRE_FALSE(defaulted == values);
        }

        SECTION("Empty vs. empty") {
            pimpl_type other(no_nuclei);
            REQUIRE(other == empty);
        }

        SECTION("Empty vs. non-empty") { REQUIRE_FALSE(empty == values); }

        SECTION("Aliases literally same nuclei") {
            pimpl_type other(nuclei);
            REQUIRE(other == values);
        }

        SECTION("Same values but different nuclei") {
            nucleus_type h2(h), he2(he), li2(li);
            reference_container c2{reference(h2), reference(he2),
                                   reference(li2)};
            pimpl_type other(c2);
            REQUIRE(other == values);
        }

        SECTION("Different nuclei") {
            reference_container c2{he, li};
            pimpl_type other(c2);
            REQUIRE_FALSE(other == values);
        }
    }
}

TEST_CASE("NucleusViewList<T>") {
    using nuclei_type = chemist::Nuclei;
    test_nucleus_view_list<nuclei_type>();
}

TEST_CASE("NucleusViewList<const T>") {
    using nuclei_type = chemist::Nuclei;
    test_nucleus_view_list<const nuclei_type>();
}
