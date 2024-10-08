#include "test_dsl.hpp"
#include <chemist/dsl/add.hpp>

/* Testing Strategy.
 *
 * The classes which derive from BinaryOp are strong types. We thus only need
 * to test the BinaryOp infrastructure for one derived class (we must test
 * through the derived class because of the CRTP usage).
 */

TEMPLATE_LIST_TEST_CASE("BinaryOp", "", test_chemist::binary_types) {
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

    SECTION("lhs()") {
        REQUIRE(a_xx.lhs() == lhs);
        REQUIRE(a_cx.lhs() == lhs);
        REQUIRE(a_xc.lhs() == lhs);
        REQUIRE(a_cc.lhs() == lhs);
    }

    SECTION("lhs() const") {
        REQUIRE(std::as_const(a_xx).lhs() == lhs);
        REQUIRE(std::as_const(a_cx).lhs() == lhs);
        REQUIRE(std::as_const(a_xc).lhs() == lhs);
        REQUIRE(std::as_const(a_cc).lhs() == lhs);
    }

    SECTION("rhs()") {
        REQUIRE(a_xx.rhs() == rhs);
        REQUIRE(a_cx.rhs() == rhs);
        REQUIRE(a_xc.rhs() == rhs);
        REQUIRE(a_cc.rhs() == rhs);
    }

    SECTION("rhs() const") {
        REQUIRE(std::as_const(a_xx).rhs() == rhs);
        REQUIRE(std::as_const(a_cx).rhs() == rhs);
        REQUIRE(std::as_const(a_xc).rhs() == rhs);
        REQUIRE(std::as_const(a_cc).rhs() == rhs);
    }
}