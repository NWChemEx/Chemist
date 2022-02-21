#include "chemist/orbital_space/ao_space.hpp"
#include <catch2/catch.hpp>

using namespace chemist::orbital_space;

TEMPLATE_TEST_CASE("AOSpace", "", float, double) {
    // Determine the types for this unit test
    using basis_set_type = chemist::AOBasisSet<TestType>;
    using space_type     = AOSpace<basis_set_type>;

    auto& world = TA::get_default_world();
    basis_set_type bs;
    bs.add_center(chemist::AtomicBasisSet<TestType>("", 0, 1.0, 2.0, 3.0));

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
        REQUIRE(defaulted.basis_set() == basis_set_type{});
    }

    space_type non_default_bs(bs);

    SECTION("Value Ctor") {
        REQUIRE(non_default_bs.size() == 1);
        REQUIRE(non_default_bs.basis_set() == bs);
    }

    SECTION("Copy Ctor") {
        space_type copy(non_default_bs);
        REQUIRE(copy.basis_set() == bs);
    }

    SECTION("Move") {
        space_type moved(std::move(non_default_bs));
        REQUIRE(moved.basis_set() == bs);
    }

    SECTION("Copy Assignment") {
        space_type copy;
        auto pcopy = &(copy = non_default_bs);
        REQUIRE(pcopy == &copy);
        REQUIRE(copy.basis_set() == bs);
    }

    SECTION("Move Assignment") {
        space_type moved;
        auto pmoved = &(moved = std::move(non_default_bs));
        REQUIRE(pmoved == &moved);
        REQUIRE(moved.basis_set() == bs);
    }

    SECTION("basis_set") { REQUIRE(non_default_bs.basis_set() == bs); }

    SECTION("basis_set() const") {
        REQUIRE(std::as_const(non_default_bs).basis_set() == bs);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(non_default_bs.size() == 1);
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(defaulted);

            SECTION("RHS == defaulted") {
                auto rhs = hash_objects(space_type{});
                REQUIRE(lhs == rhs);
            }

            SECTION("RHS == non-default") {
                auto rhs = hash_objects(non_default_bs);
                REQUIRE(lhs != rhs);
            }
        }

        SECTION("LHS == non-default && RHS == non-default") {
            auto lhs = hash_objects(non_default_bs);
            auto rhs = hash_objects(space_type{bs});
            REQUIRE(lhs == rhs);
        }
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
            if constexpr(std::is_same_v<TestType, double>) {
                AOSpace<chemist::AOBasisSetF> other;
                REQUIRE_FALSE(defaulted == other);
                REQUIRE(defaulted != other);
            } else {
                AOSpace<chemist::AOBasisSetD> other;
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
        }
    }
}
