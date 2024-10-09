#include "../catch.hpp"
#include <chemist/dsl/add.hpp>
#include <chemist/dsl/term_traits.hpp>

using namespace chemist;

/* Testing strategy.
 *
 * Our goal here is to verify that we implemented the TMP in the TermTraits
 * class correctly. That really depends on whether or not the template type
 * parameter T:
 *
 * - is const qualified,
 * - is derived from dsl::Term,
 * - is a floating point type
 */

TEST_CASE("TermTraits<char>") {
    using traits = dsl::TermTraits<char>;
    STATIC_REQUIRE_FALSE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, char>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const char&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, char&>);
    STATIC_REQUIRE_FALSE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, char&>);
}

TEST_CASE("TermTraits<const char>") {
    using traits = dsl::TermTraits<const char>;
    STATIC_REQUIRE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, char>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const char&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, const char&>);
    STATIC_REQUIRE_FALSE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, const char&>);
}

TEST_CASE("TermTraits<double>") {
    using traits = dsl::TermTraits<double>;
    STATIC_REQUIRE_FALSE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, double>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const double&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, double&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, double>);
}

TEST_CASE("TermTraits<const double>") {
    using traits = dsl::TermTraits<const double>;
    STATIC_REQUIRE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, double>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const double&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, const double&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, double>);
}

TEST_CASE("TermTraits<Add<int, double>>") {
    using op_t   = dsl::Add<int, double>;
    using traits = dsl::TermTraits<op_t>;
    STATIC_REQUIRE_FALSE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, op_t>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const op_t&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, op_t&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, op_t>);
}

TEST_CASE("TermTraits<const Add<int, double>>") {
    using op_t   = dsl::Add<int, double>;
    using traits = dsl::TermTraits<const op_t>;
    STATIC_REQUIRE(traits::is_const_v);
    STATIC_REQUIRE(std::is_same_v<traits::value_type, op_t>);
    STATIC_REQUIRE(std::is_same_v<traits::const_reference, const op_t&>);
    STATIC_REQUIRE(std::is_same_v<traits::reference, const op_t&>);
    STATIC_REQUIRE(traits::is_dsl_term_v);
    STATIC_REQUIRE(std::is_same_v<traits::holder_type, op_t>);
}