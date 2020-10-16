//#include "libchemist/orbital_space/base_space.hpp"
//#include "libchemist/orbital_space/derived_space.hpp"
//#include "test_orbital_space.hpp"
//#include <catch2/catch.hpp>
//
//
///* For testing purposes we assume:
// *
// * - The actual identity/state of the from space does not impact the
// *   DerivedSpace_ class aside from needing to have a slot in the ctor and be
// *   accessible through the DerivedSpace_ members. i.e. as long as that type is
// *   tested it'll work here too.
// *
// * - The base class more-or-less passes through the DerivedSpace_ class. i.e.,
// *   if the base class works, it'll work when used with DerivedSpace_ too.
// *
// *   - The caveats are that we need to make sure it's accounted for in the ctor,
// *     hash, and comparison operations.
// *
// *  - We assume that the overlap matrices returned by TensorMaker are suitable
// *    for use as transformation coefficients.
// */
//
//using namespace libchemist;
//using namespace libchemist::orbital_space;
//using namespace libchemist::test;
//
//TEMPLATE_PRODUCT_TEST_CASE("DerivedSpace", "",
//                           (type::tensor, type::tensor_of_tensors),
//                           (float, double)) {
//    using tensor_type = TestType;
//    using from_space  = BaseSpace_<tensor_type>;
//    using base_type   = BaseSpace_<tensor_type>;
//    using space_type  = DerivedSpace_<tensor_type, from_space, base_type>;
//
//
//    auto& world = TA::get_default_world();
//    auto S1 = TensorMaker<tensor_type>::S(world);
//    auto S2 = TensorMaker<tensor_type>::S2(world);
//    auto S3 = TensorMaker<tensor_type>::corr_transformed_S(world);
//    auto pbs1 = std::make_shared<base_type>(S1);
//    base_type bs1(*pbs1), bs2(S2);
//
//
//    SECTION("Typedefs") {
//        SECTION("transform_type") {
//            using transform_type = typename space_type::transform_type;
//            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
//        }
//
//        SECTION("from_space_type") {
//            using the_type = typename space_type::from_space_type;
//            STATIC_REQUIRE(std::is_same_v<the_type, from_space>);
//        }
//
//        SECTION("from_space_ptr"){
//            using corr_t = std::shared_ptr<const from_space>;
//            using the_type = typename space_type::from_space_ptr;
//            STATIC_REQUIRE(std::is_same_v<the_type, corr_t>);
//        }
//    }
//
//    SECTION("CTors") {
//
//        SECTION("Default") {
//          space_type st;
//        }
//
//        SECTION("All value") {
//            space_type st(S2, bs1, S1);
//            SECTION("C") { REQUIRE(compare_tensors(st.C(), S2)); }
//            SECTION("from_space") { REQUIRE(st.from_space() == bs1); }
//        }
//
//        SECTION("Base space by pointer") {
//            space_type st(S2, pbs1, S1);
//            SECTION("C") { REQUIRE(compare_tensors(st.C(), S2)); }
//            SECTION("from_space") { REQUIRE(st.from_space_data() == pbs1); }
//        }
//
//        SECTION("Copy") {
//            space_type st(S2, bs1, S1);
//            space_type st2(st);
//            REQUIRE(st == st2);
//        }
//
//        SECTION("Move") {
//            space_type st(S2, bs1, S1), st2(S2, bs1, S1);
//            space_type st3(std::move(st));
//            REQUIRE(st3 == st2);
//        }
//
//        SECTION("Copy Assignment") {
//            space_type st(S2, bs1, S1), st2;
//            auto pst2 = &(st2 = st);
//            SECTION("Value") { REQUIRE(st == st2); }
//            SECTION("Returns this") { REQUIRE(pst2 == &st2); }
//        }
//
//        SECTION("Move") {
//            space_type st(S2, bs1, S1), st2(S2, bs1, S1), st3;
//            auto pst3 = &(st3 = std::move(st));
//            SECTION("Value") { REQUIRE(st3 == st2); }
//            SECTION("Returns this") { REQUIRE(pst3 == &st3); }
//        }
//    }
//
//    space_type st1(S2, bs1, S1);
//
//    SECTION("C()") { REQUIRE(compare_tensors(st1.C(), S2)); }
//
//    SECTION("C() const") {
//        REQUIRE(compare_tensors(std::as_const(st1).C(), S2));
//    }
//
//    SECTION("from_space") {
//        REQUIRE(st1.from_space() == bs1);
//    }
//
//    SECTION("S()") {
//        space_type st2(S2, bs1);
//        REQUIRE(compare_tensors(st2.S(), S3));
//    }
//
//    SECTION("S() const") {
//        const space_type st2(S2, bs1);
//        REQUIRE(compare_tensors(st2.S(), S3));
//    }
//
//    SECTION("from_space_data") {
//        auto pbs1 = std::make_shared<const from_space>(bs1);
//        space_type st3(S2, pbs1, S1);
//        REQUIRE(st3.from_space_data() == pbs1);
//    }
//
//    SECTION("Hash"){
//
//        auto hash1 = sde::hash_objects(st1);
//
//        SECTION("Same State"){
//
//            SECTION("Same from_space instances") {
//                auto hash2 = sde::hash_objects(space_type(S2, pbs1, S1));
//                auto hash3 = sde::hash_objects(space_type(S2, pbs1, S1));
//                REQUIRE(hash2 == hash3);
//            }
//
//            SECTION("Doesn't need to be same aliased space instances") {
//                auto hash2 = sde::hash_objects(space_type(S2, bs1, S1));
//                REQUIRE(hash1 == hash2);
//            }
//        }
//
//        SECTION("Different transformation") {
//            auto hash2 = sde::hash_objects(space_type(S1, bs1, S1));
//            REQUIRE(hash1 != hash2);
//        }
//
//        SECTION("Different from space") {
//            auto hash2 = sde::hash_objects(space_type(S2, bs2, S1));
//            REQUIRE(hash1 != hash2);
//        }
//
//        SECTION("Different base spaces") {
//            auto hash2 = sde::hash_objects(space_type(S2, bs1, S2));
//            REQUIRE(hash1 != hash2);
//        }
//    }
//
//    SECTION("Comparisons"){
//        SECTION("Same State"){
//
//            SECTION("Same from_space instances") {
//                space_type st2(S2, pbs1, S1), st3(S2, pbs1, S1);
//                REQUIRE(st2 == st3);
//                REQUIRE_FALSE(st2 != st3);
//            }
//
//            SECTION("Doesn't need to be same aliased space instances") {
//                space_type st2(S2, bs1, S1);
//                REQUIRE(st1 == st2);
//                REQUIRE_FALSE(st1 != st2);
//            }
//        }
//
//        SECTION("Different transformation") {
//            space_type st2(S1, bs1, S1);
//            REQUIRE_FALSE(st1 == st2);
//            REQUIRE(st1 != st2);
//        }
//
//        SECTION("Different from space") {
//            space_type st2(S2, bs2, S1);
//            REQUIRE_FALSE(st1 == st2);
//            REQUIRE(st1 != st2);
//        }
//
//        SECTION("Different base spaces") {
//            space_type st2(S2, bs2, S2);
//            REQUIRE_FALSE(st1 == st2);
//            REQUIRE(st1 != st2);
//        }
//    }
//}
