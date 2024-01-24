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
#include <chemist/chemical_system/nucleus/nuclei.hpp>
#include <sstream>

using namespace chemist;

/* Testing Notes:
 *
 * We assume the base class has been tested and works correctly. Subject to
 * that assumption, we need to test the ability to access elements by reference
 * (since we wrote the implementation), and obtaining the size, but do not need
 * to retest all the ways one can access the elements (i.e., operator[], at, and
 * iterators). It should also be noted that contiguous storage of data is not
 * guaranteed by Nuclei, though some of its PIMPLs do guarantee it.
 */

TEST_CASE("Nuclei") {
    using set_type   = Nuclei;
    using value_type = typename set_type::value_type;

    value_type n0;
    value_type n1("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    value_type n2("He", 2ul, 4.0, 5.0, 6.0, 7.0, 5.0);

    set_type defaulted;
    set_type nuclei{n0, n1, n1, n2}; // <- ensure we can add same charge 2x

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.size() == 0); }

        SECTION("initializer_list") {
            REQUIRE(nuclei.size() == 4);
            REQUIRE(nuclei[0] == n0);
            REQUIRE(nuclei[1] == n1);
            REQUIRE(nuclei[2] == n1);
            REQUIRE(nuclei[3] == n2);
        }

        SECTION("Copy") {
            set_type copy0(defaulted);
            set_type copy1(nuclei);
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == nuclei);
        }

        SECTION("Move") {
            set_type copy0(defaulted);
            set_type move0(std::move(defaulted));
            set_type copy1(nuclei);

            // Do the move
            set_type move1(std::move(nuclei));

            // Check the values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);
        }

        SECTION("Copy assignment") {
            set_type copy0;
            auto pcopy0 = &(copy0 = defaulted);
            set_type copy1;
            auto pcopy1 = &(copy1 = nuclei);

            // Check values
            REQUIRE(copy0 == defaulted);
            REQUIRE(copy1 == nuclei);

            // Check returns *this for chaining
            REQUIRE(pcopy0 == &copy0);
            REQUIRE(pcopy1 == &copy1);
        }

        SECTION("Move assignment") {
            set_type copy0(defaulted);
            set_type move0;
            auto pmove0 = &(move0 = std::move(defaulted));
            set_type copy1(nuclei);
            set_type move1;

            // Do the move
            auto pmove1 = &(move1 = std::move(nuclei));

            // Check values
            REQUIRE(copy0 == move0);
            REQUIRE(copy1 == move1);

            // Check returns *this for chaining
            REQUIRE(pmove0 == &move0);
            REQUIRE(pmove1 == &move1);
        }
    }

    SECTION("push_back") {
        defaulted.push_back(n0);
        defaulted.push_back(n1);
        defaulted.push_back(n1);
        defaulted.push_back(n2);
        REQUIRE(defaulted == nuclei);
    }

    SECTION("at_()") {
        using rtype = decltype(nuclei[0]);
        STATIC_REQUIRE(std::is_same_v<rtype, typename set_type::reference>);

        REQUIRE(nuclei[0] == n0);
        REQUIRE(nuclei[1] == n1);
        REQUIRE(nuclei[2] == n1);
        REQUIRE(nuclei[3] == n2);

        // Can write to an element?
        nuclei[0] = n1;
        REQUIRE(nuclei[0] == n1);
    }

    SECTION("at_() const") {
        using rtype = decltype(std::as_const(nuclei)[0]);
        using corr  = typename set_type::const_reference;
        STATIC_REQUIRE(std::is_same_v<rtype, corr>);

        REQUIRE(nuclei[0] == n0);
        REQUIRE(nuclei[1] == n1);
        REQUIRE(nuclei[2] == n1);
        REQUIRE(nuclei[3] == n2);
    }

    SECTION("size_") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(nuclei.size() == 4);
    }

    SECTION("serialization") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive oarchive(ss);
            oarchive(nuclei);
        }
        {
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(defaulted);
        }
        REQUIRE(defaulted == nuclei);
    }

    SECTION("Printing") {
        std::stringstream ss;
        ss << nuclei;

        std::string corr =
          " 0.000000000000000 0.000000000000000 0.000000000000000\n";
        corr.append(
          "H 1.000000000000000 2.000000000000000 3.000000000000000\n");
        corr.append(
          "H 1.000000000000000 2.000000000000000 3.000000000000000\n");
        corr.append(
          "He 5.000000000000000 6.000000000000000 7.000000000000000\n");

        REQUIRE(ss.str() == corr);
    }
}
