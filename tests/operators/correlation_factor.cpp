#include "chemist/operators/correlation_factor.hpp"
#include "test_operator.hpp"

using namespace chemist::operators;

TEMPLATE_LIST_TEST_CASE("CorrelationFactor", "",
                        testing::correlation_factor_types) {
    using f12_type = TestType;

    f12_type f12;

    chemist::operators::STG stg(1.2, 2.3);
    f12_type non_default(stg);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(f12.template at<0>() == chemist::operators::STG{});
        }
        SECTION("Value") { REQUIRE(non_default.template at<0>() == stg); }
        SECTION("Copy") {
            f12_type other(non_default);
            REQUIRE(other.template at<0>() == stg);
        }
        SECTION("Move") {
            f12_type other(std::move(non_default));
            REQUIRE(other.template at<0>() == stg);
        }
        SECTION("Copy assignment") {
            f12_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == stg);
        }
        SECTION("Move assignment") {
            f12_type copy;
            auto pcopy = &(copy = std::move(non_default));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == stg);
        }
    }

    SECTION("operator*") {
        auto rv   = non_default * non_default;
        auto corr = stg * stg;
        REQUIRE(rv.template at<0>() == corr);
    }

    SECTION("Hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(f12);

            SECTION("LHS == RHS") {
                f12_type rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("LHS != RHS") { REQUIRE(lhs != hash_objects(non_default)); }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                f12_type rhs;
                REQUIRE(f12 == rhs);
                REQUIRE_FALSE(f12 != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(f12 != non_default);
                REQUIRE_FALSE(f12 == non_default);
            }
        }
    }

    SECTION("as_string") { REQUIRE(f12.as_string() == "f̂₁₂"); }
}
