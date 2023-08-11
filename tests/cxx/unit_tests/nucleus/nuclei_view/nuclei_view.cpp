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
#include <chemist/nucleus/detail_/nuclei_subset.hpp>
#include <chemist/nucleus/nuclei_view.hpp>
#include <sstream>

using namespace chemist;

/* Testing Notes:
 *
 * We assume the base class has been tested and works correctly. Subject to
 * that assumption, we need to test the ability to access elements by reference
 * (since we wrote the implementation), and obtaining the size, but do not need
 * to retest all the ways one can access the elements (i.e., operator[], at, and
 * iterators).
 */

TEST_CASE("NucleiView") {
    using set_type   = Nuclei;
    using value_type = typename set_type::value_type;
    using view_type  = NucleiView;
    using pimpl_type = typename view_type::pimpl_type;

    value_type n0("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    value_type n1("He", 2ul, 4.0, 5.0, 6.0, 7.0, 5.0);

    // N.B. we add n1 to nuclei_ss to make sure we can have the same
    //      nucleus twice
    auto defaulted_ss = std::make_shared<set_type>();
    auto nuclei_ss    = std::make_shared<set_type>(set_type{n0, n1, n1});

    auto defaulted_pimpl = std::make_unique<pimpl_type>(defaulted_ss);
    std::vector<std::size_t> v01{0, 1};
    std::vector<std::size_t> v12{1, 2};
    auto n01_pimpl =
      std::make_unique<pimpl_type>(nuclei_ss, v01.begin(), v01.end());
    auto n12_pimpl =
      std::make_unique<pimpl_type>(nuclei_ss, v12.begin(), v12.end());

    NucleiView no_pimpl;
    NucleiView null_pimpl(nullptr);
    NucleiView defaulted(std::move(defaulted_pimpl));
    NucleiView n01(std::move(n01_pimpl));
    NucleiView n12(std::move(n12_pimpl));

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(no_pimpl.size() == 0); }

        SECTION("pimpl_pointer") {
            REQUIRE(null_pimpl.size() == 0);
            REQUIRE(defaulted.size() == 0);

            REQUIRE(n01.size() == 2);
            REQUIRE(n01.at(0) == n0);
            REQUIRE(n01.at(1) == n1);

            REQUIRE(n12.size() == 2);
            REQUIRE(n12.at(0) == n1);
            REQUIRE(n12.at(1) == n1);
        }

        SECTION("Copy") {
            view_type no_pimpl_copy(no_pimpl);
            view_type null_pimpl(null_pimpl);
            view_type defaulted_copy(defaulted);
            view_type n01_copy(n01);
            view_type n12_copy(n12);

            REQUIRE(no_pimpl_copy.size() == 0);
            REQUIRE(null_pimpl.size() == 0);
            REQUIRE(defaulted_copy.size() == 0);
            REQUIRE(n01_copy.size() == 2);
            REQUIRE(n01.at(0) == n0);
            REQUIRE(n10.at(1) == n1);
        }

        SECTION("Move") {}

        SECTION("Copy assignment") {}

        SECTION("Move assignment") {}
    }

    SECTION("at_()") {
        REQUIRE(n01.at(0) == n0);
        REQUIRE(n01.at(1) == n1);

        REQUIRE(n12.at(0) == n1);
        REQUIRE(n12.at(1) == n1);
    }

    SECTION("at_() const") {
        REQUIRE(std::as_const(n01).at(0) == n0);
        REQUIRE(std::as_const(n01).at(1) == n1);

        REQUIRE(std::as_const(n12).at(0) == n1);
        REQUIRE(std::as_const(n12).at(1) == n1);
    }

    SECTION("size_") {
        REQUIRE(no_pimpl.size() == 0);
        REQUIRE(null_pimpl.size() == 0);
        REQUIRE(defaulted.size() == 0);
        REQUIRE(n01.size() == 2);
        REQUIRE(n10.size() == 2);
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
