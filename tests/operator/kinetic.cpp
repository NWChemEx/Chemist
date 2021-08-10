#include "libchemist/operator/kinetic.hpp"
#include "test_operator.hpp"

using namespace libchemist;

// Using ManyElectrons for distinguishability in != cases
using type_list = std::tuple<NElectronKinetic>;

TEMPLATE_LIST_TEST_CASE("Kinetic", "", type_list) {
    using delta_type     = TestType;
    using particle_types = typename delta_type::particle_type;
    using particle0_type = std::tuple_element_t<0, particle_types>;

    delta_type defaulted;

    auto p0 = testing::non_default_parameter<particle0_type>();
    delta_type non_default(p0);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.template at<0>() == particle0_type{});
        }
        SECTION("Value") {
            REQUIRE(non_default.template at<0>() == p0);
        }
        SECTION("Copy") {
            delta_type other(non_default);
            REQUIRE(other.template at<0>() == p0);
        }
        SECTION("Move") {
            delta_type other(std::move(non_default));
            REQUIRE(other.template at<0>() == p0);
        }
        SECTION("Copy assignment") {
            delta_type copy;
            auto pcopy = &(copy = non_default);
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
        }
        SECTION("Move assignment") {
            delta_type copy;
            auto pcopy = &(copy = std::move(non_default));
            REQUIRE(pcopy == &copy);
            REQUIRE(copy.template at<0>() == p0);
        }
    }

    SECTION("Hash") {
        SECTION("LHS == default") {
            auto lhs = pluginplay::hash_objects(defaulted);

            SECTION("LHS == RHS") {
                delta_type rhs;
                REQUIRE(lhs == pluginplay::hash_objects(rhs));
            }

            SECTION("LHS != RHS") {
                REQUIRE(lhs != pluginplay::hash_objects(non_default));
            }
        }
    }

    SECTION("comparisons") {
        SECTION("LHS == default") {
            SECTION("LHS == RHS") {
                delta_type rhs;
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
