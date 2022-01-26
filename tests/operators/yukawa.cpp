#include "chemist/operators/yukawa.hpp"
#include "test_operator.hpp"

using namespace chemist::operators;

using type_list = std::tuple<ElectronElectronYukawa>;

TEMPLATE_LIST_TEST_CASE("Yukawa", "", type_list) {
    using yukawa_type    = TestType;
    using particle_types = typename yukawa_type::particle_type;
    using particle0_type = std::tuple_element_t<0, particle_types>;
    using particle1_type = std::tuple_element_t<1, particle_types>;
    using particle2_type = std::tuple_element_t<2, particle_types>;

    yukawa_type defaulted;

    auto p0 = testing::non_default_parameter<particle0_type>();
    auto p1 = testing::non_default_parameter<particle1_type>();
    auto p2 = testing::non_default_parameter<particle2_type>();
    yukawa_type non_default(p0, p1, p2);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.template at<0>() == particle0_type{});
            REQUIRE(defaulted.template at<1>() == particle1_type{});
            REQUIRE(defaulted.template at<2>() == particle2_type{});
        }
        SECTION("Value") {
            REQUIRE(non_default.template at<0>() == p0);
            REQUIRE(non_default.template at<1>() == p1);
            REQUIRE(non_default.template at<2>() == p2);
        }
        SECTION("Copy") {
            yukawa_type other(non_default);
            REQUIRE(other.template at<0>() == p0);
            REQUIRE(other.template at<1>() == p1);
            REQUIRE(other.template at<2>() == p2);
        }
        SECTION("Move") {
            yukawa_type other(std::move(non_default));
            REQUIRE(other.template at<0>() == p0);
            REQUIRE(other.template at<1>() == p1);
            REQUIRE(other.template at<2>() == p2);
        }
        SECTION("Copy assignment") {
            yukawa_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
            REQUIRE(copy.template at<1>() == p1);
            REQUIRE(copy.template at<2>() == p2);
        }
        SECTION("Move assignment") {
            yukawa_type copy;
            auto pcopy = &(copy = std::move(non_default));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
            REQUIRE(copy.template at<1>() == p1);
            REQUIRE(copy.template at<2>() == p2);
        }
    }

    SECTION("Hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(defaulted);

            SECTION("LHS == RHS") {
                yukawa_type rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("LHS != RHS") { REQUIRE(lhs != hash_objects(non_default)); }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                yukawa_type rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != non_default);
                REQUIRE_FALSE(defaulted == non_default);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Ô"); }
}
