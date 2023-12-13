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

#include <chemist/vector_space/ao_space.hpp>
#include <catch2/catch.hpp>
#include <utility>

using namespace chemist::vector_space;
using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("vector_space::AOSpace", "", AOBasisSetF, AOBasisSetD) {
    // Determine the types for this unit test
    using basis_set_type        = TestType;
    using atomic_basis_set_type = typename basis_set_type::value_type;
    using space_type            = AOSpace<basis_set_type>;

    auto& world = TA::get_default_world();
    basis_set_type bs;
    bs.add_center(atomic_basis_set_type("", 0, 1.0, 2.0, 3.0));
    basis_set_type bs3;
    bs3.add_center(atomic_basis_set_type("", 0, 1.0, 2.0, 3.0));
    bs3.add_center(atomic_basis_set_type("", 1, 1.0, 2.0, 3.0));
    bs3.add_center(atomic_basis_set_type("", 2, 1.0, 2.0, 3.0));

    SECTION("Typedefs") {
        SECTION("basis_type") {
            using basis_t = typename space_type::basis_type;
            STATIC_REQUIRE(std::is_same_v<basis_t, basis_set_type>);
        }

        SECTION("size_type") {
            using size_t = typename space_type::size_type;
            using corr_t = typename BaseSpace::size_type;
            STATIC_REQUIRE(std::is_same_v<size_t, corr_t>);
        }
    }

    space_type defaulted;

    SECTION("Default Ctor") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(defaulted.get_basis_set() == basis_set_type{});
    }

    space_type non_default_bs(bs);
    space_type multi_bs(bs3);

    SECTION("Value Ctor") {
        REQUIRE(non_default_bs.size() == 1);
        REQUIRE(non_default_bs.get_basis_set() == bs);
    }

    SECTION("Copy Ctor") {
        space_type copy(non_default_bs);
        REQUIRE(copy.get_basis_set() == bs);
    }

    SECTION("Move") {
        space_type moved(std::move(non_default_bs));
        REQUIRE(moved.get_basis_set() == bs);
    }

    SECTION("Copy Assignment") {
        space_type copy;
        auto pcopy = &(copy = non_default_bs);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.get_basis_set() == bs);
    }

    SECTION("Move Assignment") {
        space_type moved;
        auto pmoved = &(moved = std::move(non_default_bs));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.get_basis_set() == bs);
    }

    SECTION("Check Default Labels") {
       REQUIRE(multi_bs.size() == 3);
       REQUIRE(multi_bs.label(0) == "ao_0");
       REQUIRE(multi_bs.label(1) == "ao_1");
       REQUIRE(multi_bs.label(2) == "ao_2");
    }

    SECTION("Basis set getter") {
        REQUIRE(std::as_const(non_default_bs).get_basis_set() == bs);
    }

    SECTION("Basis set setter") {
        space_type multi_bs2;
	multi_bs2.set_basis_set(bs3); // set the basis set
        REQUIRE(multi_bs2.size() == 3);
        REQUIRE(multi_bs2.label(0) == "ao_0");
        REQUIRE(multi_bs2.label(1) == "ao_1");
        REQUIRE(multi_bs2.label(2) == "ao_2");
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default_bs.size() == 1);
    }

    SECTION("equal") {
        SECTION("LHS == default") {
            SECTION("RHS == defaulted") {
                REQUIRE(defaulted.equal(space_type{}));
            }

            SECTION("RHS == non-default") {
                REQUIRE_FALSE(defaulted.equal(non_default_bs));
            }
        }

        SECTION("LHS == non-default && RHS == non-default") {
            REQUIRE(non_default_bs.equal(space_type{bs}));
        }
    }

    SECTION("comparisons") {
        SECTION("Different types") {
            if constexpr(std::is_same_v<TestType, AOBasisSetD>) {
                AOSpace<AOBasisSetF> other;
                REQUIRE_FALSE(defaulted == other);
                REQUIRE(defaulted != other);
            } else {
                AOSpace<AOBasisSetD> other;
                REQUIRE_FALSE(defaulted == other);
                REQUIRE(defaulted != other);
            }
        }

        SECTION("LHS == default") {
            SECTION("RHS == defaulted") {
                space_type rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("RHS == non-default") {
                REQUIRE(defaulted != non_default_bs);
                REQUIRE_FALSE(defaulted == non_default_bs);
            }
        }

        SECTION("LHS == non-default && RHS == non-default") {
            space_type rhs{bs};
            REQUIRE(non_default_bs == rhs);
            REQUIRE_FALSE(non_default_bs != rhs);
	    // compare labels
            space_type multi_bs2(bs3);
	    multi_bs2.label(0) = "ao_rev_0";
	    REQUIRE_FALSE(multi_bs2 == multi_bs);
	    REQUIRE(multi_bs2 != multi_bs);
        }
    }
}
