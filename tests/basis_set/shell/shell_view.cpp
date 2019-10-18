#include <catch2/catch.hpp>
#include <libchemist/basis_set/shell/shell_view.hpp>

using namespace libchemist;

TEST_CASE("ShellView : default ctor") {
    ShellView<double> s; // Basically just make sure it compiles.
}

TEST_CASE("ShellView<double> : pure()") {
    ShellView<double> s;
    SECTION("Value") { REQUIRE_FALSE(s.pure()); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<bool&, decltype(s.pure())>);
    }
}

TEST_CASE("ShellView<const double> : pure()") {
    ShellView<const double> s;
    SECTION("Value") { REQUIRE_FALSE(s.pure()); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const bool&, decltype(s.pure())>);
    }
}

TEST_CASE("ShellView : pure() const") {
    const ShellView<double> s;
    SECTION("Value") { REQUIRE_FALSE(s.pure()); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const bool&, decltype(s.pure())>);
    }
}

TEST_CASE("ShellView<double> : l()") {
    ShellView<double> s;
    SECTION("Value") { REQUIRE(s.l() == 0); }
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<std::size_t&, decltype(s.l())>);
    }
}

TEST_CASE("ShellView<const double> : l()") {
    ShellView<const double> s;
    SECTION("Value") { REQUIRE(s.l() == 0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(s.l())>);
    }
}

TEST_CASE("ShellView : l() const") {
    const ShellView<double> s;
    SECTION("Value") { REQUIRE(s.l() == 0); }
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(s.l())>);
    }
}
