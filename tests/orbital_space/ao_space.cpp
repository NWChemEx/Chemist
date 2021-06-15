// #include "libchemist/basis_set/basis_set.hpp"
// #include "libchemist/orbital_space/ao_space.hpp"
// #include "libchemist/orbital_space/base_space.hpp"
// #include "libchemist/orbital_space/dependent_space.hpp"
// #include "test_orbital_space.hpp"
// #include <catch2/catch.hpp>

// using namespace libchemist::orbital_space;

// TEMPLATE_TEST_CASE("AOSpace", "", float, double) {
//     // Determine the types for this unit test
//     using base_type      = BaseSpace<TestType>;
//     using basis_set_type = libchemist::AOBasisSet<TestType>;
//     using tensor_type    = typename base_type::overlap_type;
//     using space_type     = AOSpace_<basis_set_type, base_type>;

//     auto& world = TA::get_default_world();
//     auto S      = libchemist::test::TensorMaker<tensor_type>::S(world);
//     auto S2     = libchemist::test::TensorMaker<tensor_type>::S2(world);
//     basis_set_type bs;
//     bs.add_center(libchemist::Center<TestType>(1.0, 2.0, 3.0));

//     SECTION("Typedefs") {
//         SECTION("basis_type") {
//             using basis_t = typename space_type::basis_type;
//             STATIC_REQUIRE(std::is_same_v<basis_t, basis_set_type>);
//         }
//         SECTION("size_type") {
//             using size_t = typename space_type::size_type;
//             using corr_t = typename base_type::size_type;
//             STATIC_REQUIRE(std::is_same_v<size_t, corr_t>);
//         }
//     }

//     SECTION("Ctors") {
//         SECTION("Default") {
//             space_type st;
//             REQUIRE(st.basis_set() == basis_set_type{});
//         }

//         SECTION("Value") {
//             space_type st(bs, S);
//             REQUIRE(st.basis_set() == bs);
//             REQUIRE(libchemist::ta_helpers::allclose(st.S(), S));
//         }

//         SECTION("Copy") {
//             space_type st1(bs, S);
//             space_type st2(st1);
//             REQUIRE(st1 == st2);
//         }

//         SECTION("Move") {
//             space_type st1(bs, S), st2(bs, S);
//             space_type st3(std::move(st2));
//             REQUIRE(st1 == st3);
//         }

//         SECTION("Copy Assignment") {
//             space_type st1(bs, S), st2;
//             auto pst2 = &(st2 = st1);
//             SECTION("Value") { REQUIRE(st1 == st2); }
//             SECTION("Returns this") { REQUIRE(pst2 == &st2); }
//         }

//         SECTION("Move Assignment") {
//             space_type st1(bs, S), st2(bs, S), st3;
//             auto pst3 = &(st3 = std::move(st2));
//             SECTION("Value") { REQUIRE(st1 == st3); }
//             SECTION("Returns this") { REQUIRE(pst3 == &st3); }
//         }
//     }

//     space_type st1(bs, S);

//     SECTION("basis_set()") { REQUIRE(st1.basis_set() == bs); }

//     SECTION("basis_set() const") {
//         REQUIRE(std::as_const(st1).basis_set() == bs);
//     }

//     SECTION("hash") {
//         auto hash1 = sde::hash_objects(st1);

//         SECTION("Same state") {
//             auto hash2 = sde::hash_objects(space_type(bs, S));
//             REQUIRE(hash1 == hash2);
//         }

//         SECTION("Different state") {
//             SECTION("Different basis set") {
//                 auto hash2 = sde::hash_objects(space_type(basis_set_type{},
//                 S)); REQUIRE(hash1 != hash2);
//             }

//             SECTION("Different S") {
//                 auto hash2 = sde::hash_objects(space_type(bs, S2));
//                 REQUIRE(hash1 != hash2);
//             }

//             SECTION("All different") {
//                 auto hash2 =
//                   sde::hash_objects(space_type(basis_set_type{}, S2));
//                 REQUIRE(hash1 != hash2);
//             }
//         }
//     }

//     SECTION("size()") {
//         SECTION("empty") {
//             space_type st;
//             REQUIRE(st.size() == 0);
//         }
//         SECTION("Non-empty") { REQUIRE(st1.size() == bs.n_aos()); }
//     }

//     SECTION("Comparisons") {
//         SECTION("Same state") {
//             REQUIRE(st1 == space_type(bs, S));
//             REQUIRE_FALSE(st1 != space_type(bs, S));
//         }

//         SECTION("Different state") {
//             SECTION("Different basis set") {
//                 REQUIRE_FALSE(st1 == space_type(basis_set_type{}, S));
//                 REQUIRE(st1 != space_type(basis_set_type{}, S));
//             }

//             SECTION("Different S") {
//                 REQUIRE_FALSE(st1 == space_type(bs, S2));
//                 REQUIRE(st1 != space_type(bs, S2));
//             }

//             SECTION("All different") {
//                 space_type st2(basis_set_type{}, S2);
//                 REQUIRE_FALSE(st1 == st2);
//                 REQUIRE(st1 != st2);
//             }
//         }
//     }
// }
