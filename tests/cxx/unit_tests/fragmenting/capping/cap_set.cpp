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
#include <chemist/fragmenting/capping/cap_set.hpp>

using namespace chemist::fragmenting;

TEST_CASE("CapSet") {
    using index_set_type         = typename CapSet::index_set_type;
    using nuclei_reference       = typename CapSet::nuclei_reference;
    using const_nuclei_reference = typename CapSet::const_nuclei_reference;

    CapSet defaulted;

    CapSet::nucleus_type atom0;

    Cap cap0;
    Cap cap1(1, 2);
    Cap cap2(2, 3, atom0);
    Cap cap3(3, 4, atom0, atom0);

    CapSet has_values{cap0, cap1, cap2, cap3};

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.size() == 0); }
        SECTION("Initializer List") {
            REQUIRE(has_values.size() == 4);
            REQUIRE(has_values.at(0) == cap0);
            REQUIRE(has_values.at(1) == cap1);
            REQUIRE(has_values.at(2) == cap2);
            REQUIRE(has_values.at(3) == cap3);
        }
        SECTION("Range") {
            std::vector<Cap> caps{cap0, cap1};
            CapSet range(caps.begin(), caps.end());

            REQUIRE(range.size() == 2);
            REQUIRE(range.at(0) == cap0);
            REQUIRE(range.at(1) == cap1);
        }

        SECTION("Copy") {
            CapSet other_defaulted(defaulted);
            REQUIRE(defaulted == other_defaulted);

            CapSet other_values(has_values);
            REQUIRE(has_values == other_values);
        }

        SECTION("Move") {
            CapSet other_defaulted(defaulted);
            CapSet moved_defaulted(std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);

            CapSet other_values(has_values);
            CapSet moved_values(std::move(has_values));
            REQUIRE(moved_values == other_values);
        }

        SECTION("Copy Assignment") {
            CapSet other_defaulted;
            auto pother_defaulted = &(other_defaulted = defaulted);
            REQUIRE(defaulted == other_defaulted);
            REQUIRE(pother_defaulted == &other_defaulted);

            CapSet other_values;
            auto pother_values = &(other_values = has_values);
            REQUIRE(has_values == other_values);
            REQUIRE(pother_values == &other_values);
        }

        SECTION("Move Assignment") {
            CapSet other_defaulted(defaulted);
            CapSet moved_defaulted;
            auto pmoved_defaulted = &(moved_defaulted = std::move(defaulted));
            REQUIRE(moved_defaulted == other_defaulted);
            REQUIRE(pmoved_defaulted == &moved_defaulted);

            CapSet other_values(has_values);
            CapSet moved_values;
            auto pmoved_values = &(moved_values = std::move(has_values));
            REQUIRE(moved_values == other_values);
            REQUIRE(pmoved_values == &moved_values);
        }
    }

    SECTION("push_back") {
        defaulted.push_back(cap0);
        defaulted.push_back(cap1);
        defaulted.push_back(cap2);
        defaulted.push_back(cap3);
        REQUIRE(defaulted == has_values);
    }

    SECTION("emplace_back") {
        defaulted.push_back(cap0);

        // Cap with no atoms
        defaulted.emplace_back(1, 2);
        REQUIRE(defaulted == CapSet{cap0, cap1});

        // Cap with one atom
        defaulted.emplace_back(2, 3, atom0);
        REQUIRE(defaulted[2] == Cap(2, 3, atom0));

        // Cap with two atoms
        defaulted.emplace_back(3, 4, atom0, atom0);
        REQUIRE(defaulted[3] == Cap(3, 4, atom0, atom0));

        // Cap with two atoms coming from container
        std::vector<CapSet::nucleus_type> buffer{atom0, atom0, atom0};
        defaulted.emplace_back(4, 5, buffer.begin(), buffer.end());
        REQUIRE(defaulted[4] == Cap(4, 5, buffer.begin(), buffer.end()));
    }

    SECTION("get_cap_indices(range)") {
        SECTION("Neither the anchor or replaced") {
            index_set_type input{};
            index_set_type corr;
            auto rv = has_values.get_cap_indices(input.begin(), input.end());
            REQUIRE(rv == corr);
        }
        SECTION("Anchor only") {
            index_set_type input{1, 3};
            index_set_type corr{1, 3};
            auto rv = has_values.get_cap_indices(input.begin(), input.end());
            REQUIRE(rv == corr);
        }
        SECTION("Replaced only") {
            index_set_type input{4};
            index_set_type corr;
            auto rv = has_values.get_cap_indices(input.begin(), input.end());
            REQUIRE(rv == corr);
        }
        SECTION("Both the anchor and the replaced") {
            index_set_type input{1, 2};
            index_set_type corr{2};
            auto rv = has_values.get_cap_indices(input.begin(), input.end());
            REQUIRE(rv == corr);
        }
    }

    SECTION("get_cap_indices(container) const") {
        SECTION("Neither the anchor or replaced") {
            index_set_type input{};
            index_set_type corr;
            auto rv = has_values.get_cap_indices(input);
            REQUIRE(rv == corr);
        }
        SECTION("Anchor only") {
            index_set_type input{1, 3};
            index_set_type corr{1, 3};
            auto rv = has_values.get_cap_indices(input);
            REQUIRE(rv == corr);
        }
        SECTION("Replaced only") {
            index_set_type input{4};
            index_set_type corr;
            auto rv = has_values.get_cap_indices(input);
            REQUIRE(rv == corr);
        }
        SECTION("Both the anchor or replaced") {
            index_set_type input{1, 2};
            index_set_type corr{2};
            auto rv = has_values.get_cap_indices(input);
            REQUIRE(rv == corr);
        }
    }

    SECTION("get_cap_nuclei(range)") {
        typename nuclei_reference::reference a0_view(atom0);

        SECTION("Neither anchor or replaced") {
            index_set_type input;
            auto rv = has_values.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == nuclei_reference{});
        }
        SECTION("Anchor only") {
            index_set_type input0{1};
            auto rv0 = has_values.get_cap_nuclei(input0.begin(), input0.end());
            REQUIRE(rv0 == nuclei_reference{});

            index_set_type input{1, 3};
            auto rv = has_values.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == nuclei_reference{std::vector{a0_view, a0_view}});
        }
        SECTION("Replaced only") {
            index_set_type input{4};
            auto rv = has_values.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == nuclei_reference{});
        }
        SECTION("Both the anchor and replaced") {
            index_set_type input{1, 2};
            auto rv = has_values.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == nuclei_reference{std::vector{a0_view}});
        }
    }

    SECTION("get_cap_nuclei(container)") {
        typename nuclei_reference::reference a0_view(atom0);

        SECTION("Neither anchor or replaced") {
            index_set_type input;
            auto rv = has_values.get_cap_nuclei(input);
            REQUIRE(rv == nuclei_reference{});
        }
        SECTION("Anchor only") {
            index_set_type input0{1};
            auto rv0 = has_values.get_cap_nuclei(input0);
            REQUIRE(rv0 == nuclei_reference{});

            index_set_type input{1, 3};
            auto rv = has_values.get_cap_nuclei(input);
            REQUIRE(rv == nuclei_reference{std::vector{a0_view, a0_view}});
        }
        SECTION("Replaced only") {
            index_set_type input{4};
            auto rv = has_values.get_cap_nuclei(input);
            REQUIRE(rv == nuclei_reference{});
        }
        SECTION("Both the anchor and replaced") {
            index_set_type input{1, 2};
            auto rv = has_values.get_cap_nuclei(input);
            REQUIRE(rv == nuclei_reference{std::vector{a0_view}});
        }
    }

    SECTION("get_cap_nuclei(range) const") {
        typename const_nuclei_reference::const_reference a0_view(atom0);
        auto& cvalues = std::as_const(has_values);

        SECTION("Neither anchor or replaced") {
            index_set_type input;
            auto rv = cvalues.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == const_nuclei_reference{});
        }
        SECTION("Anchor only") {
            index_set_type input0{1};
            auto rv0 = cvalues.get_cap_nuclei(input0.begin(), input0.end());
            REQUIRE(rv0 == const_nuclei_reference{});

            index_set_type input{1, 3};
            auto rv = cvalues.get_cap_nuclei(input.begin(), input.end());
            std::vector a0{a0_view, a0_view};
            REQUIRE(rv == const_nuclei_reference{a0});
        }
        SECTION("Replaced only") {
            index_set_type input{4};
            auto rv = cvalues.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == const_nuclei_reference{});
        }
        SECTION("Both the anchor and replaced") {
            index_set_type input{1, 2};
            auto rv = cvalues.get_cap_nuclei(input.begin(), input.end());
            REQUIRE(rv == const_nuclei_reference{std::vector{a0_view}});
        }
    }

    SECTION("get_cap_nuclei(container) const") {
        typename const_nuclei_reference::reference a0_view(atom0);
        auto& cvalues = std::as_const(has_values);

        SECTION("Neither anchor or replaced") {
            index_set_type input;
            auto rv = cvalues.get_cap_nuclei(input);
            REQUIRE(rv == const_nuclei_reference{});
        }
        SECTION("Anchor only") {
            index_set_type input0{1};
            auto rv0 = cvalues.get_cap_nuclei(input0);
            REQUIRE(rv0 == const_nuclei_reference{});

            index_set_type input{1, 3};
            auto rv = cvalues.get_cap_nuclei(input);
            std::vector<decltype(a0_view)> a0{a0_view, a0_view};
            REQUIRE(rv == const_nuclei_reference{a0});
        }
        SECTION("Replaced only") {
            index_set_type input{4};
            auto rv = cvalues.get_cap_nuclei(input);
            REQUIRE(rv == const_nuclei_reference{});
        }
        SECTION("Both the anchor and replaced") {
            index_set_type input{1, 2};
            auto rv = cvalues.get_cap_nuclei(input);
            REQUIRE(rv == const_nuclei_reference{std::vector{a0_view}});
        }
    }

    SECTION("at_") {
        // N.B. at_ is used by the base class to implement element access. We
        // rely on the public API to test that we actually implemented it right.

        REQUIRE(has_values[0] == cap0);
        REQUIRE(has_values[1] == cap1);
        REQUIRE(has_values[2] == cap2);
        REQUIRE(has_values[3] == cap3);

        // Is writeable
        has_values[1] = cap0;
        REQUIRE(has_values[1] == cap0);
    }

    SECTION("at_() const") {
        // N.B. at_ is used by the base class to implement element access. We
        // rely on the public API to test that we actually implemented it right.

        REQUIRE(std::as_const(has_values)[0] == cap0);
        REQUIRE(std::as_const(has_values)[1] == cap1);
        REQUIRE(std::as_const(has_values)[2] == cap2);
        REQUIRE(std::as_const(has_values)[3] == cap3);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(has_values.size() == 4);
    }

    SECTION("Comparisons") {
        // Default v default
        CapSet other_default;
        REQUIRE(defaulted == other_default);
        REQUIRE_FALSE(defaulted != other_default);

        // Default vs non-default
        REQUIRE(defaulted != has_values);
        REQUIRE_FALSE(defaulted == has_values);

        // Same value
        CapSet other_values{cap0, cap1, cap2, cap3};
        REQUIRE(has_values == other_values);
        REQUIRE_FALSE(has_values != other_values);

        // Different order
        CapSet diff_order{cap1, cap0, cap2, cap3};
        REQUIRE(has_values != diff_order);
        REQUIRE_FALSE(has_values == diff_order);

        // Different caps
        CapSet cs0{cap0}, cs1{cap1};
        REQUIRE(cs0 != cs1);
        REQUIRE_FALSE(cs0 == cs1);
    }
}
