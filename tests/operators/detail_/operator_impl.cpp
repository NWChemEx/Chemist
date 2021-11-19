#include "../test_operator.hpp"
#include <utilities/type_traits/tuple/count_type.hpp>

/* Testing Strategy:
 *
 * OperatorImpl actually implements the majority of the operators via CRTP.
 * While it is technically possible to create instances of OperatorImpl
 * directly, users are expected to actually create OperatorImpl instances via
 * derived classes. Here we test the non-polymorphic operations of OperatorImpl
 * and leave testing of the polymorphic functions for the derived classes.
 */

using namespace utilities::type_traits;
using namespace chemist::operators;
namespace {

// This is some bogus operator, which has a type different than all operators we
// are actually testing.
template<typename T>
struct BogusOperator
  : chemist::operators::detail_::OperatorImpl<BogusOperator, T> {};

} // namespace

TEMPLATE_LIST_TEST_CASE("OperatorImpl", "", testing::all_operator_impls) {
    using derived_type    = TestType;
    using particle_types  = typename derived_type::particle_type;
    constexpr auto n_body = std::tuple_size<particle_types>();

    derived_type defaulted;

    SECTION("at") {
        particle_types default_particles;
        if constexpr(n_body > 0) {
            const auto& p = defaulted.template at<0>();
            REQUIRE(p == std::get<0>(default_particles));
        }
        if constexpr(n_body > 1) {
            const auto& p = defaulted.template at<1>();
            REQUIRE(p == std::get<1>(default_particles));
        }
        if constexpr(n_body > 2) {
            const auto& p = defaulted.template at<2>();
            REQUIRE(p == std::get<2>(default_particles));
        }
        static_assert(n_body <= 3, "Please increment add more bodies to test");
    }

    SECTION("Non-polymorphic comparisons") {
        // Should technically upcast to the OperatorImpl base, but that's a
        // royal pain...

        SECTION("Types are different") {
            REQUIRE(defaulted != BogusOperator<int>{});
            REQUIRE_FALSE(defaulted == BogusOperator<int>{});
        }

        SECTION("Both default") {
            derived_type rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        // w/o extensive TMP it's hard to test for different instances so will
        // that in the derived class tests
    }
}

TEST_CASE("OperatorImpl : as_string()") {
    BogusOperator<int> bogus;
    REQUIRE(bogus.as_string() == "Ô");
}
