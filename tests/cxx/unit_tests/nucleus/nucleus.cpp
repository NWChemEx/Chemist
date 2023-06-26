/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/nucleus/nucleus.hpp>
#include <sstream>

using namespace chemist;

TEST_CASE("Nucleus") {
    Nucleus defaulted;
    Nucleus zm("He", 2ul, 7304.01);
    Nucleus zmxyz("C", 6ul, 21897.81, 1.0, 2.0, 3.0);
    Nucleus zmxyzq("U", 92ul, 428912.28, 4.0, 5.0, 6.0, 91.5);

    SECTION("CTors/Assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.name() == "");
            REQUIRE(defaulted.Z() == 0ul);
            REQUIRE(defaulted.mass() == 0.0);
            REQUIRE(defaulted.x() == 0.0);
            REQUIRE(defaulted.y() == 0.0);
            REQUIRE(defaulted.z() == 0.0);
        }

        SECTION("name, Z, and mass") {
            REQUIRE(zm.name() == "He");
            REQUIRE(zm.Z() == 2ul);
            REQUIRE(zm.mass() == 7304.01);
            REQUIRE(zm.x() == 0.0);
            REQUIRE(zm.y() == 0.0);
            REQUIRE(zm.z() == 0.0);
        }

        SECTION("Z, mass, and coordinates") {
            REQUIRE(zmxyz.name() == "C");
            REQUIRE(zmxyz.Z() == 6ul);
            REQUIRE(zmxyz.mass() == 21897.81);
            REQUIRE(zmxyz.x() == 1.0);
            REQUIRE(zmxyz.y() == 2.0);
            REQUIRE(zmxyz.z() == 3.0);
        }

        SECTION("Z, mass, coordinates, and charge") {
            REQUIRE(zmxyzq.name() == "U");
            REQUIRE(zmxyzq.Z() == 92ul);
            REQUIRE(zmxyzq.mass() == 428912.28);
            REQUIRE(zmxyzq.x() == 4.0);
            REQUIRE(zmxyzq.y() == 5.0);
            REQUIRE(zmxyzq.z() == 6.0);
        }

        SECTION("Copy") {
            Nucleus copy(zmxyzq);
            REQUIRE(copy == zmxyzq);
        }

        SECTION("Copy assignment") {
            Nucleus copy;
            auto pcopy = &(copy = zmxyzq);
            REQUIRE(copy == zmxyzq);
            REQUIRE(pcopy == &copy);
        }

        SECTION("Move") {
            Nucleus copy(zmxyzq);
            Nucleus moved(std::move(zmxyzq));
            REQUIRE(copy == moved);
        }

        SECTION("Move assignment") {
            Nucleus copy(zmxyzq);
            Nucleus moved;
            auto pmoved = &(moved = std::move(zmxyzq));
            REQUIRE(moved == copy);
            REQUIRE(pmoved == &moved);
        }
    }

    SECTION("name()") {
        auto pname = &(zm.name());
        REQUIRE(*pname == "He");

        // Can change it
        *pname = "H";
        REQUIRE(zm.name() == "H");
    }

    SECTION("name() const") {
        auto pname = &(std::as_const(zm).name());
        REQUIRE(*pname == "He");
    }

    SECTION("Z") {
        auto pZ = &(zm.Z());
        REQUIRE(*pZ == 2ul);

        // Can change it
        *pZ = 1ul;
        REQUIRE(zm.Z() == 1ul);
    }

    SECTION("Z const") {
        auto pZ = &(std::as_const(zm).Z());
        REQUIRE(*pZ == 2ul);
    }

    SECTION("mass") {
        auto pm = &(zm.mass());
        REQUIRE(*pm == 7304.01);

        // Can change it
        *pm = 1.2;
        REQUIRE(zm.mass() == 1.2);
    }

    SECTION("mass const") {
        auto pm = &(std::as_const(zm).mass());
        REQUIRE(*pm == 7304.01);
    }

    SECTION("comparisons") {
        SECTION("same") {
            Nucleus rhs("U", 92ul, 428912.28, 4.0, 5.0, 6.0, 91.5);
            REQUIRE(zmxyzq == rhs);
            REQUIRE_FALSE(zmxyzq != rhs);
        }

        SECTION("different name") {
            Nucleus rhs("He", 92ul, 428912.28, 4.0, 5.0, 6.0, 91.5);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }

        SECTION("Different atomic number") {
            Nucleus rhs("U", 93ul, 428912.28, 4.0, 5.0, 6.0, 91.5);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }

        SECTION("Different mass") {
            Nucleus rhs("U", 92ul, 1.28, 4.0, 5.0, 6.0, 91.5);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }

        SECTION("Different x coordinate") {
            Nucleus rhs("U", 92ul, 428912.28, 2.0, 5.0, 6.0, 91.5);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }

        SECTION("Different y coordinate") {
            Nucleus rhs("U", 92ul, 428912.28, 4.0, 2.0, 6.0, 91.5);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }

        SECTION("Different z coordinate") {
            Nucleus rhs("U", 92ul, 428912.28, 4.0, 5.0, 2.0, 91.5);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }

        SECTION("Different charge") {
            Nucleus rhs("U", 92ul, 428912.28, 4.0, 5.0, 6.0, 92.1);
            REQUIRE_FALSE(zmxyzq == rhs);
            REQUIRE(zmxyzq != rhs);
        }
    }

    SECTION("serialization") {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive oarchive(ss);
            oarchive(zmxyzq);
        }
        {
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(defaulted);
        }
        REQUIRE(defaulted == zmxyzq);
    }

    SECTION("Printing") {
        std::stringstream ss;
        ss << zmxyz;

        std::string corr = "C 1.000000000000000 2.000000000000000 3.000000000000000";

        REQUIRE(ss.str() == corr);
    }
}
