#include "libchemist/orbital_space/dependent_space.hpp"
#include "test_orbital_space.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::orbital_space;

TEMPLATE_TEST_CASE("DependentSpace", "", float, double) {
    using base_space  = SparseDependentBase<TestType>;
    using tensor_type = typename base_space::overlap_type;

    auto& world = TA::get_default_world();
    auto S      = libchemist::test::TensorMaker<tensor_type>::S(world);
    auto S2     = libchemist::test::TensorMaker<tensor_type>::S2(world);
    auto sm1    = libchemist::test::sm1;
    auto sm2    = libchemist::test::sm2;

    SECTION("Typedefs") {
        SECTION("Sparse map type") {
            using index_type = libchemist::sparse_map::ElementIndex;
            using corr_type =
              libchemist::sparse_map::SparseMap<index_type, index_type>;
            using sparse_map_type = typename base_space::sparse_map_type;
            STATIC_REQUIRE(std::is_same_v<sparse_map_type, corr_type>);
        }

        SECTION("Overlap type") {
            using overlap_type = typename base_space::overlap_type;
            STATIC_REQUIRE(std::is_same_v<overlap_type, tensor_type>);
        }
    }

    SECTION("CTors") {
        SECTION("Default") {
            base_space bs;
            REQUIRE(bs.sparse_map() == sm1);
        }

        SECTION("Value") {
            base_space bs(sm2, S);
            REQUIRE(bs.sparse_map() == sm2);
            REQUIRE(libchemist::ta_helpers::allclose_tot(bs.S(), S, 2));
        }

        SECTION("Copy") {
            base_space bs(sm2, S);
            REQUIRE(bs == base_space(bs));
        }

        SECTION("Move") {
            base_space bs(sm2, S), bs2(sm2, S);
            base_space bs3(std::move(bs));
            REQUIRE(bs2 == bs3);
        }

        SECTION("Copy Assignment") {
            base_space bs(sm2, S), bs2;
            auto pbs2 = &(bs2 = bs);
            SECTION("Value") { REQUIRE(bs == bs2); }
            SECTION("Returns this") { REQUIRE(pbs2 == &bs2); }
        }

        SECTION("Move Assignment") {
            base_space bs(sm2, S), bs2(sm2, S), bs3;
            auto pbs3 = &(bs3 = std::move(bs));
            SECTION("Value") { REQUIRE(bs2 == bs3); }
            SECTION("Returns this") { REQUIRE(pbs3 == &bs3); }
        }
    }

    base_space bs1(sm1, S), bs2(sm2, S2);

    SECTION("sparse_map()") { REQUIRE(bs1.sparse_map() == sm1); }

    SECTION("sparse_map() const") {
        REQUIRE(std::as_const(bs1).sparse_map() == sm1);
    }

    SECTION("S()") {
        REQUIRE(libchemist::ta_helpers::allclose_tot(bs1.S(), S, 2));
    }

    SECTION("S() const") {
        REQUIRE(
          libchemist::ta_helpers::allclose_tot(std::as_const(bs1).S(), S, 2));
    }

    SECTION("Hash") {
        SECTION("Same state") {
            base_space bs3(sm1, S);
            REQUIRE(runtime::hash_objects(bs1) == runtime::hash_objects(bs3));
        }

        SECTION("Different sparse map") {
            base_space bs3(sm2, S);
            REQUIRE(runtime::hash_objects(bs1) != runtime::hash_objects(bs3));
        }

        SECTION("Different overlap matrix") {
            base_space bs3(sm1, S2);
            REQUIRE(runtime::hash_objects(bs1) != runtime::hash_objects(bs3));
        }
    }

    SECTION("Comparison") {
        SECTION("Same state") {
            base_space bs3(sm1, S);
            REQUIRE(bs1 == bs3);
            REQUIRE_FALSE(bs1 != bs3);
        }

        SECTION("Different sparse map") {
            base_space bs3(sm2, S);
            REQUIRE_FALSE(bs1 == bs3);
            REQUIRE(bs1 != bs3);
        }

        SECTION("Different overlap matrix") {
            base_space bs3(sm1, S2);
            REQUIRE_FALSE(bs1 == bs3);
            REQUIRE(bs1 != bs3);
        }
    }
}
