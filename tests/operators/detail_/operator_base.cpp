#include "../test_operator.hpp"
#include <iostream>

using namespace chemist::operators;

/* Testing strategy:
 *
 * OperatorBase is an abstract class, so we can't make instances of it. Instead
 * we loop over instances of the derived classes, which get used in practice.
 * For each of these derived classes we cast them down to OperatorBase and test
 * the non-virtual members (virtual members are tested in the derived classes).
 */

TEMPLATE_LIST_TEST_CASE("OperatorBase", "", testing::all_operators) {
    using derived_type = TestType;

    derived_type defaulted;

    SECTION("Non-polymorphic comparison") {
        REQUIRE(derived_type{} == defaulted);
        REQUIRE_FALSE(derived_type{} != defaulted);
    }

    SECTION("coefficient") { REQUIRE(defaulted.coefficient() == 1.0); }
}
