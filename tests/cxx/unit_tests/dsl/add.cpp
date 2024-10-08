#include "test_dsl.hpp"
#include <chemist/dsl/add.hpp>

/* Testing Strategy.
 *
 * Add is basically a strong type, we just test it can be constructed with all
 * of the possible const-variations.
 */

TEMPLATE_LIST_TEST_CASE("Add", "", test_chemist::binary_types) {
    using lhs_type = std::tuple_element_t<0, TestType>;
    using rhs_type = std::tuple_element_t<1, TestType>;

    auto values     = test_chemist::binary_values();
    auto [lhs, rhs] = std::get<TestType>(values);

    chemist::dsl::Add<lhs_type, rhs_type> a_xx(lhs, rhs);
    chemist::dsl::Add<const lhs_type, rhs_type> a_cx(lhs, rhs);
    chemist::dsl::Add<lhs_type, const rhs_type> a_xc(lhs, rhs);
    chemist::dsl::Add<const lhs_type, const rhs_type> a_cc(lhs, rhs);

    SECTION("CTors") {
        REQUIRE(a_xx.lhs() == lhs);
        REQUIRE(a_xx.rhs() == rhs);

        REQUIRE(a_cx.lhs() == lhs);
        REQUIRE(a_cx.rhs() == rhs);

        REQUIRE(a_xc.lhs() == lhs);
        REQUIRE(a_xc.rhs() == rhs);

        REQUIRE(a_cc.lhs() == lhs);
        REQUIRE(a_cc.rhs() == rhs);
    }
}