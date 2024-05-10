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

#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <chemist/chemical_system/nucleus/nuclei_view/detail_/contiguous_nuclei_view.hpp>
#include <chemist/chemical_system/nucleus/nuclei_view/nuclei_view.hpp>
#include <sstream>

using namespace chemist;

/* Testing Notes:
 *
 * We assume the base class has been tested and works correctly. Subject to
 * that assumption, we need to test the ability to access elements by reference
 * (since we wrote the implementation), and obtaining the size, but do not need
 * to retest all the ways one can access the elements (i.e., operator[], at, and
 * iterators). Also we note that NucleiView does NOT guarantee that the data it
 * aliases are stored contiguously, though some PIMPLs will provide such a
 * guarantee.
 */

template<typename NucleiType>
void test_nuclei_view() {
    constexpr bool is_const_v =
      !std::is_same_v<NucleiType, std::remove_cv_t<NucleiType>>;
    using set_type         = NucleiType;
    using value_type       = typename set_type::value_type;
    using view_type        = NucleiView<set_type>;
    using member_list_type = typename view_type::member_list_type;

    value_type n0("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    value_type n1("He", 2ul, 4.0, 5.0, 6.0, 7.0, 5.0);

    set_type defaulted_set;
    set_type value_set{n0, n1};

    view_type defaulted;
    view_type empty(defaulted_set);
    view_type value(value_set);

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }

        SECTION("Nuclei reference") {
            REQUIRE(empty.size() == 0);
            REQUIRE(value.size() == 2);
            REQUIRE(value[0] == n0);
            REQUIRE(value[1] == n1);
        }

        SECTION("mutable to read-only") {
            using no_cv_t = std::remove_cv_t<NucleiType>;
            if constexpr(std::is_same_v<NucleiType, const no_cv_t>) {
                Nuclei value_set2{n0, n1};
                NucleiView<Nuclei> is_mutable(value_set2);
                view_type is_const_now(is_mutable);
                REQUIRE(is_const_now == value);
            }
        }

        SECTION("Pointers") {
            view_type empty_pointers(defaulted_set.charges(), nullptr, nullptr,
                                     nullptr);

            REQUIRE(empty_pointers.size() == 0);

            view_type pointers(value_set.charges(), value_set.name_data(),
                               value_set.atomic_number_data(),
                               value_set.mass_data());

            REQUIRE(pointers.size() == 2);
            REQUIRE(pointers[0] == n0);
            REQUIRE(pointers[1] == n1);
        }

        SECTION("Subset") {
            view_type empty_subset(value, member_list_type{});
            REQUIRE(empty_subset.size() == 0);

            view_type proper_subset(value, member_list_type{1});
            REQUIRE(proper_subset.size() == 1);
            REQUIRE(proper_subset[0] == n1);

            view_type subset(value, member_list_type{0, 1});
            REQUIRE(subset.size() == 2);
            REQUIRE(subset[0] == n0);
            REQUIRE(subset[1] == n1);

            view_type reverse_subset(value, member_list_type{1, 0});
            REQUIRE(reverse_subset.size() == 2);
            REQUIRE(reverse_subset[0] == n1);
            REQUIRE(reverse_subset[1] == n0);
        }

        SECTION("NucleusView objects") {
            using reference_container = typename view_type::reference_container;
            using reference           = typename view_type::reference;

            view_type empty2{reference_container{}};
            REQUIRE(empty2.size() == 0);

            reference_container c{reference(n0), reference(n1)};
            view_type has_values2(c);
            REQUIRE(has_values2.size() == 2);
            REQUIRE(has_values2[0] == n0);
            REQUIRE(has_values2[1] == n1);
        }

        SECTION("NucleiView objects") {
            using container = typename view_type::nuclei_view_container;
            set_type c0{n0}, c1{n1};
            container c{view_type{c0}, view_type{c1}};

            view_type has_values2(c);
            REQUIRE(has_values2.size() == 2);
            REQUIRE(has_values2[0] == n0);
            REQUIRE(has_values2[1] == n1);

            view_type has_value3(container{});
            REQUIRE(has_value3.size() == 0);

            container c_1{view_type{c1}};
            view_type has_values4(c_1);
            REQUIRE(has_values4.size() == 1);
            REQUIRE(has_values4[0] == n1);
        }

        SECTION("Copy") {
            view_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            view_type empty_copy(empty);
            REQUIRE(empty_copy == empty);

            view_type value_copy(value);
            REQUIRE(value_copy == value);
        }

        SECTION("Move") {
            view_type defaulted_copy(defaulted);
            view_type defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);

            view_type empty_copy(empty);
            view_type empty_move(std::move(empty));
            REQUIRE(empty_copy == empty_move);

            view_type value_copy(value);
            view_type value_move(std::move(value));
            REQUIRE(value_copy == value_move);
        }

        SECTION("Assign from Nuclei") {
            if constexpr(!is_const_v) {
                set_type value1{n1, n0}; // Just swap the order
                auto pvalue = &(value = value1);
                REQUIRE(pvalue == &value);
                REQUIRE(value[0] == n1);
                REQUIRE(value[1] == n0);
                REQUIRE(value_set == value1);

                REQUIRE_THROWS_AS(defaulted = value1, std::runtime_error);
            }
        }

        SECTION("Copy assignment") {
            view_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy == defaulted);

            view_type empty_copy;
            auto pempty_copy = &(empty_copy = empty);
            REQUIRE(pempty_copy == &empty_copy);
            REQUIRE(empty_copy == empty);

            view_type value_copy;
            auto pvalue_copy = &(value_copy = value);
            REQUIRE(pvalue_copy == &value_copy);
            REQUIRE(value_copy == value);
        }

        SECTION("Move assignment") {
            view_type defaulted_copy(defaulted);
            view_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_copy == defaulted_move);

            view_type empty_copy(empty);
            view_type empty_move;
            auto pempty_move = &(empty_move = std::move(empty));
            REQUIRE(pempty_move == &empty_move);
            REQUIRE(empty_copy == empty_move);

            view_type value_copy(value);
            view_type value_move;
            auto pvalue_move = &(value_move = std::move(value));
            REQUIRE(pvalue_move == &value_move);
            REQUIRE(value_copy == value_move);
        }
    }

    SECTION("at_()") {
        REQUIRE(value[0] == n0);
        REQUIRE(value[1] == n1);

        // If we're aliasing a mutable Nuclei ensure we can write to it.
        using no_cv = std::remove_cv_t<NucleiType>;
        if constexpr(std::is_same_v<no_cv, NucleiType>) {
            value[0] = n1;
            REQUIRE(value[0] == n1);
            REQUIRE(value_set[0] == n1);
        }
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(value)[0] == n0);
        REQUIRE(std::as_const(value)[1] == n1);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(empty.size() == 0);
        REQUIRE(value.size() == 2);
    }

    SECTION("swap") {
        auto lhs_copy(defaulted);
        auto rhs_copy(value);

        defaulted.swap(value);

        REQUIRE(defaulted == rhs_copy);
        REQUIRE(value == lhs_copy);
    }

    SECTION("as_nuclei") {
        REQUIRE(defaulted.as_nuclei() == defaulted_set);
        REQUIRE(value.as_nuclei() == value_set);
    }

    SECTION("operator==(NucleiView)") {
        // defaulted vs. defaulted
        REQUIRE(defaulted == view_type{});

        // defaulted vs. empty
        REQUIRE(defaulted == empty);

        // defaulted vs. non-empty
        REQUIRE_FALSE(defaulted == value);

        // empty vs. empty
        view_type empty2(defaulted_set);
        REQUIRE(empty == empty2);

        // empty vs. non-empty
        REQUIRE_FALSE(empty == value);

        // value vs. same aliased nuclei
        view_type value2(value_set);
        REQUIRE(value == value2);

        // value vs. different nuclei object, but same value
        set_type value_set2{n0, n1};
        view_type value3(value_set2);
        REQUIRE(value == value3);

        // value vs. different value (we just switch the order)
        set_type value_set3{n1, n0};
        view_type value4(value_set3);
        REQUIRE_FALSE(value == value4);
    }

    SECTION("operator!=(NucleiView)") {
        // Negates operator== so spot check
        REQUIRE(defaulted != value);
        REQUIRE_FALSE(defaulted != empty);
    }

    SECTION("operator==(Nuclei)") {
        // Default vs. empty
        REQUIRE(defaulted == defaulted_set);

        // Default vs. value
        REQUIRE_FALSE(defaulted == value_set);

        // Empty vs. empty
        REQUIRE(empty == defaulted_set);

        // Empty vs. non-empty
        REQUIRE_FALSE(empty == value_set);

        // Value vs. value
        REQUIRE(value == value_set);

        // Ensure we can call Nuclei == NucleiView (just spot check because
        // implementation calls NucleiView == Nuclei)
        REQUIRE(defaulted_set == defaulted);
        REQUIRE_FALSE(value_set == defaulted);
    }

    SECTION("operator!=(Nuclei)") {
        // Just negates operator==, suffice to spot check
        REQUIRE_FALSE(defaulted != set_type{});
        REQUIRE(value != set_type{});

        // Ensure can be called symmetrically (implementation just reverses
        // argument order so no need to revisit all possibilities as long as
        // operator!=(Nuclei) works)
        REQUIRE_FALSE(set_type{} != defaulted);
        REQUIRE(set_type{} != value);
    }

    SECTION("Can mix and match PIMPLs") {
        view_type subset(value, member_list_type{0, 1});

        REQUIRE(subset == value);
    }
}

TEST_CASE("NucleiView<Nuclei>") { test_nuclei_view<Nuclei>(); }

TEST_CASE("NucleiView<const Nuclei>") { test_nuclei_view<const Nuclei>(); }
