//#include "libchemist/orbital_space/base_space.hpp"
//#include "test_orbital_space.hpp"
//#include <catch2/catch.hpp>
//
//using namespace libchemist;
//using namespace libchemist::orbital_space;
//
//// TODO: Test the default instance when/if default tensors are usable
//
//TEMPLATE_PRODUCT_TEST_CASE("BaseSpace", "",
//                           (type::tensor, type::tensor_of_tensors),
//                           (float, double)) {
//    // Determine the types for this unit test
//    using base_space      = BaseSpace_<TestType>;
//    using tensor_type     = typename base_space::overlap_type;
//
//    auto& world = TA::get_default_world();
//    auto S  = test::TensorMaker<tensor_type>::S(world);
//    auto S2 = test::TensorMaker<tensor_type>::S2(world);
//
//    SECTION("Typedefs") {
//        using overlap_type = typename base_space::overlap_type;
//        STATIC_REQUIRE(std::is_same_v<overlap_type, TestType>);
//    }
//
//    SECTION("CTors") {
//        SECTION("Default") {
//            base_space bs;
//            REQUIRE_FALSE(bs.has_overlap());
//        }
//
//        SECTION("Value") {
//            base_space bs(S);
//            REQUIRE(test::compare_tensors(bs.S(), S));
//        }
//
//        SECTION("Copy") {
//            SECTION("Default") {
//                // base_space bs0;
//                // base_space bs1(bs0);
//                // REQUIRE(bs0 == bs1);
//            }
//
//            SECTION("Value") {
//                base_space bs0(S);
//                base_space bs1(bs0);
//                REQUIRE(bs0 == bs1);
//            }
//        }
//
//        SECTION("Move") {
//            SECTION("Default") {
//                // base_space bs0, bs1;
//                // base_space bs2(std::move(bs0));
//                // REQUIRE(bs1 == bs2);
//            }
//
//            SECTION("Value") {
//                base_space bs0(S), bs1(S);
//                base_space bs2(std::move(bs0));
//                REQUIRE(bs1 == bs2);
//            }
//        }
//
//        SECTION("Copy Assignment") {
//            SECTION("Default") {
//                // base_space bs0, bs1;
//                // auto pbs1 = &(bs1 = bs0);
//                // SECTION("Same state") { REQUIRE(bs0 == bs1); }
//                // SECTION("Returns this") { REQUIRE(pbs1 == &bs1); }
//            }
//
//            SECTION("Value") {
//                base_space bs0(S), bs1;
//                auto pbs1 = &(bs1 = bs0);
//                SECTION("Same state") { REQUIRE(bs0 == bs1); }
//                SECTION("Returns this") { REQUIRE(pbs1 == &bs1); }
//            }
//        }
//
//        SECTION("Move") {
//            SECTION("Default") {
//                // base_space bs0, bs1, bs2;
//                // auto pbs2 = &(bs2 = std::move(bs0));
//                // SECTION("Same state") { REQUIRE(bs1 == bs2); }
//                // SECTION("Returns this") { REQUIRE(pbs2 == &bs2); }
//            }
//
//            SECTION("Value") {
//                base_space bs0(S), bs1(S), bs2;
//                auto pbs2 = &(bs2 = std::move(bs0));
//                SECTION("Same state") { REQUIRE(bs1 == bs2); }
//                SECTION("Returns this") { REQUIRE(pbs2 == &bs2); }
//            }
//        }
//    }
//
//    base_space bs, bs_S(S), bs_S2(S2);
//
//    SECTION("S()") {
//        REQUIRE_THROWS_AS(bs.S(), std::bad_optional_access);
//        REQUIRE(test::compare_tensors(bs_S.S(), S));
//    }
//
//    SECTION("S() const"){
//        REQUIRE_THROWS_AS(std::as_const(bs).S(), std::bad_optional_access);
//        REQUIRE(test::compare_tensors(std::as_const(bs_S).S(), S));
//    }
//
//    SECTION("hash"){
//        SECTION("Both default") {
//            auto hash1 = sde::hash_objects(bs);
//            auto hash2 = sde::hash_objects(base_space());
//            REQUIRE(hash1 == hash2);
//        }
//
//        SECTION("One default and one non-default") {
//            auto hash1 = sde::hash_objects(bs);
//            auto hash2 = sde::hash_objects(bs_S);
//            REQUIRE(hash1 != hash2);
//        }
//
//        SECTION("Same overlap matrix") {
//            auto hash1 = sde::hash_objects(bs_S);
//            auto hash2 = sde::hash_objects(base_space(S));
//            REQUIRE(hash1 == hash2);
//        }
//
//        SECTION("Different overlap matrix") {
//            auto hash1 = sde::hash_objects(bs_S);
//            auto hash2 = sde::hash_objects(bs_S2);
//            REQUIRE(hash1 != hash2);
//        }
//    }
//
//    SECTION("Comparison operators") {
//        SECTION("Both default"){
//            REQUIRE(bs == base_space());
//            REQUIRE_FALSE(bs != base_space());
//        }
//
//        SECTION("One default and one non-default") {
//            REQUIRE_FALSE(bs == bs_S);
//            REQUIRE(bs != bs_S);
//        }
//
//        SECTION("Same overlap matrix") {
//            REQUIRE(bs_S == base_space(S));
//            REQUIRE_FALSE(bs_S != base_space(S));
//        }
//
//        SECTION("Different overlap matrix") {
//            REQUIRE_FALSE(bs_S == bs_S2);
//            REQUIRE(bs_S != bs_S2);
//        }
//    }
//}
