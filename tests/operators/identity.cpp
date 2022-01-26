#include "chemist/operators/identity.hpp"
#include "test_operator.hpp"

using namespace chemist;
using namespace chemist::operators;

// Using ManyElectrons for distinguishability in != cases
using type_list = std::tuple<Identity<ManyElectrons>>;

TEMPLATE_LIST_TEST_CASE("Identity", "", type_list) {
    using identity_type  = TestType;
    using particle_types = typename identity_type::particle_type;
    using particle0_type = std::tuple_element_t<0, particle_types>;

    identity_type defaulted;

    auto p0 = testing::non_default_parameter<particle0_type>();
    identity_type non_default(p0);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.template at<0>() == particle0_type{});
        }
        SECTION("Value") { REQUIRE(non_default.template at<0>() == p0); }
        SECTION("Copy") {
            identity_type other(non_default);
            REQUIRE(other.template at<0>() == p0);
        }
        SECTION("Move") {
            identity_type other(std::move(non_default));
            REQUIRE(other.template at<0>() == p0);
        }
        SECTION("Copy assignment") {
            identity_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
        }
        SECTION("Move assignment") {
            identity_type copy;
            auto pcopy = &(copy = std::move(non_default));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
        }
    }

    SECTION("Hash") {
        using chemist::detail_::hash_objects;
        SECTION("LHS == default") {
            auto lhs = hash_objects(defaulted);

            SECTION("LHS == RHS") {
                identity_type rhs;
                REQUIRE(lhs == hash_objects(rhs));
            }

            SECTION("LHS != RHS") { REQUIRE(lhs != hash_objects(non_default)); }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                identity_type rhs;
                REQUIRE(defaulted == rhs);
                REQUIRE_FALSE(defaulted != rhs);
            }

            SECTION("LHS != RHS") {
                REQUIRE(defaulted != non_default);
                REQUIRE_FALSE(defaulted == non_default);
            }
        }
    }

    SECTION("as_string") { REQUIRE(defaulted.as_string() == "Î"); }
}
