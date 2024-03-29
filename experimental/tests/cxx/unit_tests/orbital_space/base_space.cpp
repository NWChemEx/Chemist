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

#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "chemist/types.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::orbital_space;

/* Testing Strategy:
 *
 * BaseSpace is pure abstract so we can't instantiate it. For testing purposes
 * what we do is instantiate an AOSpace instance and test the BaseSpace specific
 * parts of it, which includes:
 *
 * - size_type typedef
 * - hash function that does nothing
 *   - there's no way to access this function from outside the class though
 * - operator== which always returns true
 * - operator!= which always returns false
 *
 * All other methods require the derived class to implement them and are tested
 * there.
 */

TEST_CASE("BaseSpace") {
    using space_type  = AOSpaceD;
    using basis_type  = typename AOSpaceD::basis_type;
    using center_type = typename basis_type::value_type;

    SECTION("size_type") {
        using size_type = typename BaseSpace::size_type;
        using corr      = type::size;
        STATIC_REQUIRE(std::is_same_v<size_type, corr>);
    }

    // Make a non-empty basis to distinguish the two instances at the AOSpace
    // level
    basis_type non_empty;
    non_empty.add_center(center_type{"", 0, 1.0, 2.0, 3.0});

    AOSpaceD lhs;
    AOSpaceD rhs(non_empty);

    SECTION("operator==") {
        BaseSpace& blhs = lhs;
        BaseSpace& brhs = rhs;
        REQUIRE(blhs == brhs);
    }

    SECTION("operator!=") {
        BaseSpace& blhs = lhs;
        BaseSpace& brhs = rhs;
        REQUIRE_FALSE(blhs != brhs);
    }
}
