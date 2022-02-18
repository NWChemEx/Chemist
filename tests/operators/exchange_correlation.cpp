#include "chemist/operators/exchange_correlation.hpp"
#include "test_operator.hpp"

using namespace chemist;
using namespace chemist::operators;

using type_list = std::tuple<ExchangeCorrelation>;

TEMPLATE_LIST_TEST_CASE("ExchangeCorrelation", "", type_list) {
    using xc_type            = TestType;
    using xc_functional_type = ExchangeCorrelationFunctional;
    using particle_types     = typename xc_type::particle_type;
    using functional_type    = std::tuple_element_t<0, particle_types>;
    using particle0_type     = std::tuple_element_t<1, particle_types>;
    using particle1_type     = std::tuple_element_t<2, particle_types>;

    xc_type defaulted;

    auto func = xc_functional_type::PBE0;
    auto p0   = testing::non_default_parameter<particle0_type>();
    auto p1   = testing::non_default_parameter<particle1_type>();
    xc_type non_default(func, p0, p1);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.template at<0>() == xc_functional_type::_INVALID);
            REQUIRE(defaulted.template at<1>() == particle0_type{});
            REQUIRE(defaulted.template at<2>() == particle1_type{});
        }
        SECTION("Value") {
            REQUIRE(non_default.template at<0>() == func);
            REQUIRE(non_default.template at<1>() == p0);
            REQUIRE(non_default.template at<2>() == p1);
        }
        SECTION("Copy") {
            xc_type other(non_default);
            REQUIRE(other.template at<0>() == func);
            REQUIRE(other.template at<1>() == p0);
            REQUIRE(other.template at<2>() == p1);
        }
        SECTION("Move") {
            xc_type other(std::move(non_default));
            REQUIRE(other.template at<0>() == func);
            REQUIRE(other.template at<1>() == p0);
            REQUIRE(other.template at<2>() == p1);
        }
        SECTION("Copy assignment") {
            xc_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == func);
            REQUIRE(copy.template at<1>() == p0);
            REQUIRE(copy.template at<2>() == p1);
        }
        SECTION("Move assignment") {
            xc_type copy;
            auto pcopy = &(copy = std::move(non_default));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == func);
            REQUIRE(copy.template at<1>() == p0);
            REQUIRE(copy.template at<2>() == p1);
        }
    }

    SECTION("Hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(defaulted);

            SECTION("LHS == RHS") {
                xc_type rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("LHS != RHS") { REQUIRE(lhs != hash_objects(non_default)); }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                xc_type rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS w/ default") {
                REQUIRE(defaulted != non_default);
                REQUIRE_FALSE(defaulted == non_default);
            }
            SECTION("LHS != RHS different XCF") {
                xc_type other(xc_functional_type::SVWN3, p0, p1);
                REQUIRE(other != non_default);
                REQUIRE_FALSE(other == non_default);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Ô"); }
}
