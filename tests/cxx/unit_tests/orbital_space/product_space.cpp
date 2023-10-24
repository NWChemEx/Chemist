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

#include "chemist/basis_set/basis_set.hpp"
#include "chemist/orbital_space/product_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::orbital_space;

TEST_CASE("ProductSpace") {
    // Work out types we will need
    using scalar_type = double;
    using space_type  = ASOSpaceD;
    using tensor_type = chemist::type::tensor;
    using space_a     = AOSpaceD;
    using base_space  = BaseSpace;
    using vector_il   = TA::detail::vector_il<double>;
    using matrix_il   = TA::detail::matrix_il<double>;

    SECTION("Typedefs") {
        SECTION("space_a_type") {
            using space_a_type = typename space_type::space_type_a;
            STATIC_REQUIRE(std::is_same_v<space_a_type, space_a>);
        }
        SECTION("space_b_type") {
            using space_b_type = typename space_type::space_type_b;
            using corr         = SpinSpace;
            STATIC_REQUIRE(std::is_same_v<space_b_type, corr>);
        }
        SECTION("space_ptr_a") {
            using space_a_ptr = typename space_type::space_ptr_a;
            using corr        = std::shared_ptr<const space_a>;
            STATIC_REQUIRE(std::is_same_v<space_a_ptr, corr>);
        }
        SECTION("space_ptr_b") {
            using space_b_ptr = typename space_type::space_ptr_b;
            using corr        = std::shared_ptr<const SpinSpace>;
            STATIC_REQUIRE(std::is_same_v<space_b_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr      = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }

    SpinSpace spin_space;
    space_type default_aso(space_a{}, spin_space);
    AOBasisSetD bs;
    bs.add_center(chemist::AtomicBasisSet<scalar_type>("", 0, 1.0, 2.0, 3.0));
    space_a aos(std::move(bs));
    space_type non_default_aso(aos, spin_space);

    SECTION("CTors/Assignment") {
        SECTION("Default Ctor") {
            space_type s;
            REQUIRE(s.size() == 0);
        }

        SECTION("Value Ctor") {
            REQUIRE(default_aso.SpaceRefA() == space_a{});
            REQUIRE(default_aso.SpaceRefB() == spin_space);
            REQUIRE(non_default_aso.SpaceRefA() == aos);
            REQUIRE(non_default_aso.SpaceRefB() == spin_space);
        }

        SECTION("Aliasing Ctor") {
            auto p_spin_space = std::make_shared<SpinSpace>(spin_space);
            space_type alias(default_aso.space_data_a(), p_spin_space);
            REQUIRE(alias.SpaceRefA() == space_a{});
            REQUIRE(alias.SpaceRefB() == spin_space);
        }

        SECTION("Copy Ctor") {
            space_type copy(non_default_aso);
            REQUIRE(copy.SpaceRefA() == aos);
            REQUIRE(copy.space_data_a() == non_default_aso.space_data_a());
        }

        SECTION("Move Ctor") {
            space_type moved(std::move(non_default_aso));
            REQUIRE(moved.SpaceRefA() == aos);
            REQUIRE(moved.SpaceRefB() == spin_space);
        }

        SECTION("Copy assignment") {
            space_type copy;
            auto pcopy = &(copy = non_default_aso);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.SpaceRefA() == aos);
            REQUIRE(copy.space_data_a() == non_default_aso.space_data_a());
        }

        SECTION("Move assignment") {
            space_type moved;
            auto pmoved = &(moved = std::move(non_default_aso));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.SpaceRefA() == aos);
            REQUIRE(moved.SpaceRefB() == spin_space);
        }
    }

    SECTION("Accessors") {
        SECTION("SpaceRefA()") {
            REQUIRE(non_default_aso.SpaceRefA() == aos);

            SECTION("Throws if no transformation") {
                space_type s;
                REQUIRE_THROWS_AS(s.SpaceRefA(), std::runtime_error);
            }
        }

        SECTION("SpaceRefB()") {
            REQUIRE(non_default_aso.SpaceRefB() == spin_space);

            SECTION("Throws if no space") {
                space_type s;
                REQUIRE_THROWS_AS(s.SpaceRefB(), std::runtime_error);
            }
        }

        SECTION("space_a_data") {
            auto pSpaceA = &(non_default_aso.SpaceRefA());
            REQUIRE(non_default_aso.space_data_a().get() == pSpaceA);
        }

        SECTION("space_b_data()") {
            auto p_spin_space = &(non_default_aso.SpaceRefB());
            REQUIRE(non_default_aso.space_data_b().get() == p_spin_space);
        }

        SECTION("size") {
            REQUIRE(space_type{}.size() == 0);
            REQUIRE(non_default_aso.SpaceRefA().size() == 1);
            REQUIRE(non_default_aso.SpaceRefB().size() == 2);
            REQUIRE(non_default_aso.size() == 2);
        }
    }

    SECTION("comparisons") {
        const space_type lhs;
        SECTION("LHS == RHS") {
            const space_type rhs;
            REQUIRE(lhs == rhs);
            REQUIRE_FALSE(lhs != rhs);
        }

        SECTION("LHS != default") {
            REQUIRE_FALSE(lhs == default_aso);
            REQUIRE(lhs != default_aso);
        }

        SECTION("RHS == default") {
            const space_type rhs(space_a{}, spin_space);
            REQUIRE(default_aso == rhs);
            REQUIRE_FALSE(default_aso != rhs);
        }
    }
}