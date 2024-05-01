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
#include <chemist/chemical_system/nucleus/nuclei_view/detail_/contiguous_nuclei_view.hpp>

template<typename NucleiType>
void contiguous_nuclei_guts() {
    using nuclei_type  = NucleiType;
    using pimpl_type   = chemist::detail_::ContiguousNucleiView<nuclei_type>;
    using nucleus_type = typename pimpl_type::nucleus_traits::value_type;
    using name_type    = typename pimpl_type::nucleus_traits::name_type;
    using atomic_number_type =
      typename pimpl_type::nucleus_traits::atomic_number_type;
    using mass_type         = typename pimpl_type::nucleus_traits::mass_type;
    using charges_type      = typename pimpl_type::charges_traits::value_type;
    using point_charge_type = typename charges_type::value_type;

    point_charge_type q0(-1.1, 0.0, 1.0, 2.0);
    point_charge_type q1(1.0, 3.0, 4.0, 5.0);
    point_charge_type q2(0.0, 6.0, 7.0, 8.0);

    charges_type defaulted_qs{};
    charges_type qs{q0, q1, q2};
    std::vector<name_type> names{"H", "He", "Li"};
    std::vector<atomic_number_type> zs{1, 2, 3};
    std::vector<mass_type> masses{1.0, 4.0, 7.0};

    // Values we're testing
    pimpl_type defaulted;
    pimpl_type no_values(defaulted_qs, nullptr, nullptr, nullptr);
    pimpl_type values(qs, names.data(), zs.data(), masses.data());

    // Correct values
    nucleus_type h("H", 1, 1.0, 0.0, 1.0, 2.0, -1.1);
    nucleus_type he("He", 2, 4.0, 3.0, 4.0, 5.0, 1.0);
    nucleus_type li("Li", 3, 7.0, 6.0, 7.0, 8.0, 0.0);

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Value") {
            REQUIRE(values.size() == 3);
            REQUIRE(values.get_nuke(0) == h);
            REQUIRE(values.get_nuke(1) == he);
            REQUIRE(values.get_nuke(2) == li);
        }
        SECTION("Copy") {
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            pimpl_type no_values_copy(no_values);
            REQUIRE(no_values_copy == no_values);

            pimpl_type values_copy(values);
            REQUIRE(values_copy == values);
        }
    }

    SECTION("clone") {
        auto pdefaulted_copy = defaulted.clone();
        REQUIRE(pdefaulted_copy->are_equal(defaulted));

        auto pno_values_copy = no_values.clone();
        REQUIRE(pno_values_copy->are_equal(no_values));

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
        }
    }

    SECTION("get_nuke() const") {
        REQUIRE(std::as_const(values).get_nuke(0) == h);
        REQUIRE(std::as_const(values).get_nuke(1) == he);
        REQUIRE(std::as_const(values).get_nuke(2) == li);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(no_values.size() == 0);
        REQUIRE(values.size() == 3);
    }

    SECTION("operator==") {
        SECTION("Defaulted vs. defaulted") {
            REQUIRE(defaulted == pimpl_type{});
        }

        SECTION("Defaulted vs. empty") { REQUIRE(defaulted == no_values); }

        SECTION("Defaulted vs. non-empty") {
            REQUIRE_FALSE(defaulted == values);
        }

        SECTION("Empty vs. empty") {
            pimpl_type other(defaulted_qs, nullptr, nullptr, nullptr);
            REQUIRE(other == no_values);
        }

        SECTION("Empty vs. non-empty") { REQUIRE_FALSE(no_values == values); }

        SECTION("Same values and addresses") {
            pimpl_type other(qs, names.data(), zs.data(), masses.data());
            REQUIRE(other == values);
        }

        SECTION("Same values and different addresses") {
            std::vector<name_type> names2(names);
            std::vector<atomic_number_type> zs2(zs);
            std::vector<mass_type> masses2(masses);
            pimpl_type other(qs, names2.data(), zs2.data(), masses2.data());
            REQUIRE(other == values);
        }

        SECTION("Different charges") {
            // n.b. we swapped the order of the first two charges
            charges_type qs2{q1, q0, q2};
            pimpl_type other(qs2, names.data(), zs.data(), masses.data());
            REQUIRE_FALSE(other == values);
        }

        SECTION("Different names") {
            std::vector<name_type> names2{"foo", "bar", "baz"};
            pimpl_type other(qs, names2.data(), zs.data(), masses.data());
            REQUIRE_FALSE(other == values);
        }

        SECTION("Different atomic numbers") {
            std::vector<atomic_number_type> zs2{42, 43, 44};
            pimpl_type other(qs, names.data(), zs2.data(), masses.data());
            REQUIRE_FALSE(other == values);
        }

        SECTION("Different masses") {
            std::vector<mass_type> masses2{1.23, 2.34, 3.45};
            pimpl_type other(qs, names.data(), zs.data(), masses2.data());
            REQUIRE_FALSE(other == values);
        }
    }
}

TEST_CASE("ContiguousNucleiView<T>") {
    using nuclei_type = chemist::Nuclei;
    contiguous_nuclei_guts<nuclei_type>();
}

TEST_CASE("ContiguousNucleiView<const T>") {
    using nuclei_type = chemist::Nuclei;
    contiguous_nuclei_guts<const nuclei_type>();
}
