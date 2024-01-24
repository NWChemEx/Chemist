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
#include <chemist/chemical_system/nucleus/detail_/nuclei_subset.hpp>
#include <chemist/chemical_system/nucleus/nuclei_view.hpp>
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

TEST_CASE("NucleiView") {
    using set_type    = Nuclei;
    using value_type  = typename set_type::value_type;
    using view_type   = NucleiView<set_type>;
    using const_view  = NucleiView<const set_type>;
    using pimpl_type  = detail_::NucleiSubset<set_type>;
    using const_pimpl = detail_::NucleiSubset<const set_type>;

    value_type n0("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    value_type n1("He", 2ul, 4.0, 5.0, 6.0, 7.0, 5.0);

    // N.B. we add n1 to nuclei_ss to make sure we can have the same
    //      nucleus twice
    auto defaulted_ss = std::make_shared<set_type>();
    auto nuclei_ss    = std::make_shared<set_type>(set_type{n0, n1, n1});

    std::vector<std::size_t> v01{0, 1};
    std::vector<std::size_t> v12{1, 2};

    // We intentionally use begin as the end iterator to get an empty set
    auto defaulted_pimpl =
      std::make_unique<pimpl_type>(defaulted_ss, v01.begin(), v01.begin());
    auto n01_pimpl =
      std::make_unique<pimpl_type>(nuclei_ss, v01.begin(), v01.end());
    auto n12_pimpl =
      std::make_unique<pimpl_type>(nuclei_ss, v12.begin(), v12.end());

    auto const_defaulted_pimpl =
      std::make_unique<const_pimpl>(defaulted_ss, v01.begin(), v01.begin());
    auto const_n01_pimpl =
      std::make_unique<const_pimpl>(nuclei_ss, v01.begin(), v01.end());
    auto const_n12_pimpl =
      std::make_unique<const_pimpl>(nuclei_ss, v12.begin(), v12.end());

    view_type no_pimpl;
    view_type null_pimpl(nullptr);
    view_type defaulted(std::move(defaulted_pimpl));
    view_type n01(std::move(n01_pimpl));
    view_type n12(std::move(n12_pimpl));

    const_view const_no_pimpl;
    const_view const_null_pimpl(nullptr);
    const_view const_defaulted(std::move(const_defaulted_pimpl));
    const_view const_n01(std::move(const_n01_pimpl));
    const_view const_n12(std::move(const_n12_pimpl));

    SECTION("Ctor") {
        SECTION("default") {
            REQUIRE(no_pimpl.size() == 0);
            REQUIRE(const_no_pimpl.size() == 0);
        }

        SECTION("pimpl_pointer") {
            REQUIRE(null_pimpl.size() == 0);
            REQUIRE(defaulted.size() == 0);

            REQUIRE(const_null_pimpl.size() == 0);
            REQUIRE(const_defaulted.size() == 0);

            REQUIRE(n01.size() == 2);
            REQUIRE(n01.at(0) == n0);
            REQUIRE(n01.at(1) == n1);

            REQUIRE(const_n01.size() == 2);
            REQUIRE(const_n01.at(0) == n0);
            REQUIRE(const_n01.at(1) == n1);

            REQUIRE(n12.size() == 2);
            REQUIRE(n12.at(0) == n1);
            REQUIRE(n12.at(1) == n1);

            REQUIRE(const_n12.size() == 2);
            REQUIRE(const_n12.at(0) == n1);
            REQUIRE(const_n12.at(1) == n1);
        }

        SECTION("Copy") {
            view_type no_pimpl_copy(no_pimpl);
            view_type null_pimpl_copy(null_pimpl);
            view_type defaulted_copy(defaulted);
            view_type n01_copy(n01);
            view_type n12_copy(n12);

            const_view const_no_pimpl_copy(const_no_pimpl);
            const_view const_null_pimpl_copy(const_null_pimpl);
            const_view const_defaulted_copy(const_defaulted);
            const_view const_n01_copy(const_n01);
            const_view const_n12_copy(const_n12);

            REQUIRE(no_pimpl_copy.size() == 0);
            REQUIRE(null_pimpl_copy.size() == 0);
            REQUIRE(defaulted_copy.size() == 0);
            REQUIRE(n01_copy.size() == 2);
            REQUIRE(n01_copy.at(0) == n0);
            REQUIRE(n01_copy.at(1) == n1);
            REQUIRE(n12_copy.size() == 2);
            REQUIRE(n12_copy.at(0) == n1);
            REQUIRE(n12_copy.at(1) == n1);

            REQUIRE(const_no_pimpl_copy.size() == 0);
            REQUIRE(const_null_pimpl_copy.size() == 0);
            REQUIRE(const_defaulted_copy.size() == 0);
            REQUIRE(const_n01_copy.size() == 2);
            REQUIRE(const_n01_copy.at(0) == n0);
            REQUIRE(const_n01_copy.at(1) == n1);
            REQUIRE(const_n12_copy.size() == 2);
            REQUIRE(const_n12_copy.at(0) == n1);
            REQUIRE(const_n12_copy.at(1) == n1);
        }

        SECTION("Move") {
            view_type no_pimpl_move(std::move(no_pimpl));
            view_type null_pimpl_move(std::move(null_pimpl));
            view_type defaulted_move(std::move(defaulted));
            view_type n01_move(std::move(n01));
            view_type n12_move(std::move(n12));

            const_view const_no_pimpl_move(std::move(const_no_pimpl));
            const_view const_null_pimpl_move(std::move(const_null_pimpl));
            const_view const_defaulted_move(std::move(const_defaulted));
            const_view const_n01_move(std::move(const_n01));
            const_view const_n12_move(std::move(const_n12));

            REQUIRE(no_pimpl_move.size() == 0);
            REQUIRE(null_pimpl_move.size() == 0);
            REQUIRE(defaulted_move.size() == 0);
            REQUIRE(n01_move.size() == 2);
            REQUIRE(n01_move.at(0) == n0);
            REQUIRE(n01_move.at(1) == n1);
            REQUIRE(n12_move.size() == 2);
            REQUIRE(n12_move.at(0) == n1);
            REQUIRE(n12_move.at(1) == n1);

            REQUIRE(const_no_pimpl_move.size() == 0);
            REQUIRE(const_null_pimpl_move.size() == 0);
            REQUIRE(const_defaulted_move.size() == 0);
            REQUIRE(const_n01_move.size() == 2);
            REQUIRE(const_n01_move.at(0) == n0);
            REQUIRE(const_n01_move.at(1) == n1);
            REQUIRE(const_n12_move.size() == 2);
            REQUIRE(const_n12_move.at(0) == n1);
            REQUIRE(const_n12_move.at(1) == n1);
        }

        SECTION("Copy assignment") {
            view_type no_pimpl_copy;
            auto pno_pimpl_copy = &(no_pimpl_copy = no_pimpl);
            view_type null_pimpl_copy;
            auto pnull_pimpl_copy = &(null_pimpl_copy = null_pimpl);
            view_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            view_type n01_copy;
            auto pn01_copy = &(n01_copy = n01);
            view_type n12_copy;
            auto pn12_copy = &(n12_copy = n12);

            const_view const_no_pimpl_copy;
            auto pconst_no_pimpl_copy = &(const_no_pimpl_copy = const_no_pimpl);
            const_view const_null_pimpl_copy;
            auto pconst_null_pimpl_copy =
              &(const_null_pimpl_copy = const_null_pimpl);
            const_view const_defaulted_copy;
            auto pconst_defaulted_copy =
              &(const_defaulted_copy = const_defaulted);
            const_view const_n01_copy;
            auto pconst_n01_copy = &(const_n01_copy = const_n01);
            const_view const_n12_copy;
            auto pconst_n12_copy = &(const_n12_copy = const_n12);

            REQUIRE(pno_pimpl_copy == &no_pimpl_copy);
            REQUIRE(no_pimpl_copy.size() == 0);

            REQUIRE(pnull_pimpl_copy == &null_pimpl_copy);
            REQUIRE(null_pimpl_copy.size() == 0);

            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy.size() == 0);

            REQUIRE(pn01_copy == &n01_copy);
            REQUIRE(n01_copy.size() == 2);
            REQUIRE(n01_copy.at(0) == n0);
            REQUIRE(n01_copy.at(1) == n1);

            REQUIRE(pn12_copy == &n12_copy);
            REQUIRE(n12_copy.size() == 2);
            REQUIRE(n12_copy.at(0) == n1);
            REQUIRE(n12_copy.at(1) == n1);

            REQUIRE(pconst_no_pimpl_copy == &const_no_pimpl_copy);
            REQUIRE(const_no_pimpl_copy.size() == 0);

            REQUIRE(pconst_null_pimpl_copy == &const_null_pimpl_copy);
            REQUIRE(const_null_pimpl_copy.size() == 0);

            REQUIRE(pconst_defaulted_copy == &const_defaulted_copy);
            REQUIRE(const_defaulted_copy.size() == 0);

            REQUIRE(pconst_n01_copy == &const_n01_copy);
            REQUIRE(const_n01_copy.size() == 2);
            REQUIRE(const_n01_copy.at(0) == n0);
            REQUIRE(const_n01_copy.at(1) == n1);

            REQUIRE(pconst_n12_copy == &const_n12_copy);
            REQUIRE(const_n12_copy.size() == 2);
            REQUIRE(const_n12_copy.at(0) == n1);
            REQUIRE(const_n12_copy.at(1) == n1);
        }

        SECTION("Move assignment") {
            view_type no_pimpl_move;
            auto pno_pimpl_move = &(no_pimpl_move = std::move(no_pimpl));
            view_type null_pimpl_move;
            auto pnull_pimpl_move = &(null_pimpl_move = std::move(null_pimpl));
            view_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            view_type n01_move;
            auto pn01_move = &(n01_move = std::move(n01));
            view_type n12_move;
            auto pn12_move = &(n12_move = std::move(n12));

            const_view const_no_pimpl_move;
            auto pconst_no_pimpl_move =
              &(const_no_pimpl_move = std::move(const_no_pimpl));
            const_view const_null_pimpl_move;
            auto pconst_null_pimpl_move =
              &(const_null_pimpl_move = std::move(const_null_pimpl));
            const_view const_defaulted_move;
            auto pconst_defaulted_move =
              &(const_defaulted_move = std::move(const_defaulted));
            const_view const_n01_move;
            auto pconst_n01_move = &(const_n01_move = std::move(const_n01));
            const_view const_n12_move;
            auto pconst_n12_move = &(const_n12_move = std::move(const_n12));

            REQUIRE(pno_pimpl_move == &no_pimpl_move);
            REQUIRE(no_pimpl_move.size() == 0);

            REQUIRE(pnull_pimpl_move == &null_pimpl_move);
            REQUIRE(null_pimpl_move.size() == 0);

            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_move.size() == 0);

            REQUIRE(pn01_move == &n01_move);
            REQUIRE(n01_move.size() == 2);
            REQUIRE(n01_move.at(0) == n0);
            REQUIRE(n01_move.at(1) == n1);

            REQUIRE(pn12_move == &n12_move);
            REQUIRE(n12_move.size() == 2);
            REQUIRE(n12_move.at(0) == n1);
            REQUIRE(n12_move.at(1) == n1);

            REQUIRE(pconst_no_pimpl_move == &const_no_pimpl_move);
            REQUIRE(const_no_pimpl_move.size() == 0);

            REQUIRE(pconst_null_pimpl_move == &const_null_pimpl_move);
            REQUIRE(const_null_pimpl_move.size() == 0);

            REQUIRE(pconst_defaulted_move == &const_defaulted_move);
            REQUIRE(const_defaulted_move.size() == 0);

            REQUIRE(pconst_n01_move == &const_n01_move);
            REQUIRE(const_n01_move.size() == 2);
            REQUIRE(const_n01_move.at(0) == n0);
            REQUIRE(const_n01_move.at(1) == n1);

            REQUIRE(pconst_n12_move == &const_n12_move);
            REQUIRE(const_n12_move.size() == 2);
            REQUIRE(const_n12_move.at(0) == n1);
            REQUIRE(const_n12_move.at(1) == n1);
        }
    }

    SECTION("at_()") {
        REQUIRE(n01.at(0) == n0);
        REQUIRE(n01.at(1) == n1);

        REQUIRE(n12.at(0) == n1);
        REQUIRE(n12.at(1) == n1);

        REQUIRE(const_n01.at(0) == n0);
        REQUIRE(const_n01.at(1) == n1);

        REQUIRE(const_n12.at(0) == n1);
        REQUIRE(const_n12.at(1) == n1);
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(n01).at(0) == n0);
        REQUIRE(std::as_const(n01).at(1) == n1);

        REQUIRE(std::as_const(n12).at(0) == n1);
        REQUIRE(std::as_const(n12).at(1) == n1);

        REQUIRE(std::as_const(const_n01).at(0) == n0);
        REQUIRE(std::as_const(const_n01).at(1) == n1);

        REQUIRE(std::as_const(const_n12).at(0) == n1);
        REQUIRE(std::as_const(const_n12).at(1) == n1);
    }

    SECTION("size_") {
        REQUIRE(no_pimpl.size() == 0);
        REQUIRE(null_pimpl.size() == 0);
        REQUIRE(defaulted.size() == 0);
        REQUIRE(n01.size() == 2);
        REQUIRE(n12.size() == 2);

        REQUIRE(const_no_pimpl.size() == 0);
        REQUIRE(const_null_pimpl.size() == 0);
        REQUIRE(const_defaulted.size() == 0);
        REQUIRE(const_n01.size() == 2);
        REQUIRE(const_n12.size() == 2);
    }

    SECTION("swap") {
        no_pimpl.swap(n01);
        REQUIRE(no_pimpl.size() == 2);
        REQUIRE(no_pimpl.at(0) == n0);
        REQUIRE(no_pimpl.at(1) == n1);
        REQUIRE(n01.size() == 0);

        const_no_pimpl.swap(const_n01);
        REQUIRE(const_no_pimpl.size() == 2);
        REQUIRE(const_no_pimpl.at(0) == n0);
        REQUIRE(const_no_pimpl.at(1) == n1);
        REQUIRE(const_n01.size() == 0);
    }

    SECTION("operator==(Nuclei)") {
        // Empty v empty
        set_type defaulted_nuclei;
        REQUIRE(no_pimpl == defaulted_nuclei);
        REQUIRE(null_pimpl == defaulted_nuclei);
        REQUIRE(defaulted == defaulted_nuclei);

        // Non-empty v empty
        REQUIRE_FALSE(n01 == defaulted_nuclei);

        // Empty v non-empty
        set_type n01_nuclei{n0, n1};
        REQUIRE_FALSE(no_pimpl == n01_nuclei);
        REQUIRE_FALSE(null_pimpl == n01_nuclei);
        REQUIRE_FALSE(defaulted == n01_nuclei);

        // Non-empty v non-empty
        REQUIRE(n01 == n01_nuclei);       // Same non-empty
        REQUIRE_FALSE(n12 == n01_nuclei); // Different non-empty

        // Ensure can be called symmetrically (implementation just reverses
        // argument order so  no need to revisit all possibilities as long as
        // operator==(Nuclei) works)

        REQUIRE(defaulted_nuclei == no_pimpl);
        REQUIRE_FALSE(defaulted_nuclei == n01);
    }

    SECTION("operator!=(Nuclei)") {
        // Just negates operator==, suffice to spot check
        REQUIRE_FALSE(no_pimpl != set_type{});
        REQUIRE(n01 != set_type{});

        // Ensure can be called symmetrically (implementation just reverses
        // argument order so no need to revisit all possibilities as long as
        // operator!=(Nuclei) works)
        REQUIRE_FALSE(set_type{} != no_pimpl);
        REQUIRE(set_type{} != n01);
    }

    // SECTION("serialization") {
    //     std::stringstream ss;
    //     {
    //         cereal::BinaryOutputArchive oarchive(ss);
    //         oarchive(nuclei);
    //     }
    //     {
    //         cereal::BinaryInputArchive iarchive(ss);
    //         iarchive(defaulted);
    //     }
    //     REQUIRE(defaulted == nuclei);
    // }

    SECTION("Printing") {
        std::stringstream ss;
        ss << n01;

        std::string corr =
          "H 1.000000000000000 2.000000000000000 3.000000000000000\n";
        corr.append(
          "He 5.000000000000000 6.000000000000000 7.000000000000000\n");

        REQUIRE(ss.str() == corr);
    }
}
