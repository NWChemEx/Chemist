#include "chemist/basis_set/shell.hpp"
#include "chemist/basis_set/shell/shell_pimpl.hpp"
#include "chemist/point/point_pimpl.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

/* Testing Strategy:
 *
 * By using the Point and IndexableContainerBase mix-ins we already know that
 * most of the API works (assuming we implemented the hooks correctly). We also
 * know by using ShellPIMPL that the implementations for the Shell class work.
 * Thus we need to test:
 *
 * - ctors
 * - accessors/hooks call appropriate implementation
 * - comparison
 */

using namespace chemist;
using vector_t = std::vector<double>;

static inline auto make_shell() {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    ContractedGaussian<double> cg(cs, es, 7.0, 8.0, 9.0);
    Shell<double> s(ShellType::pure, 2, cs, es, 7.0, 8.0, 9.0);
    return std::make_pair(s, cg);
}

template<typename T1, typename T2>
static void check_state(T1&& s, ShellType pure, std::size_t l, T2&& corr) {
    SECTION("Purity") { REQUIRE(s.pure() == pure); }
    SECTION("Angular Momentum") { REQUIRE(s.l() == l); }
    SECTION("CGTO") { REQUIRE(s[0] == corr); }
}

TEST_CASE("Shell : default ctor") {
    Shell<double> s;
    ContractedGaussian<double> cg(vector_t{}, vector_t{}, 0.0, 0.0, 0.0);
    check_state(s, ShellType::cartesian, 0, cg);
}

TEST_CASE("Shell : copy ctor") {
    auto [s, cg] = make_shell();
    Shell<double> s2(s);
    check_state(s2, ShellType::pure, 2, cg);
}

TEST_CASE("Shell : move ctor") {
    auto [s, cg] = make_shell();
    Shell<double> s2(std::move(s));
    check_state(s2, ShellType::pure, 2, cg);
}

TEST_CASE("Shell : copy assignment") {
    auto [s, cg] = make_shell();
    Shell<double> s2;
    auto ps2 = &(s2 = s);
    check_state(s2, ShellType::pure, 2, cg);
    SECTION("Returns this") { REQUIRE(ps2 == &s2); }
}

TEST_CASE("Shell : move assignment") {
    auto [s, cg] = make_shell();
    Shell<double> s2;
    auto ps2 = &(s2 = std::move(s));
    check_state(s2, ShellType::pure, 2, cg);
    SECTION("Returns this") { REQUIRE(ps2 == &s2); }
}

TEST_CASE("Shell : value ctor") {
    auto [s, cg] = make_shell();
    check_state(s, ShellType::pure, 2, cg);
}

TEST_CASE("Shell : PIMPL ctor") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    auto ptr1 = std::make_unique<detail_::ShellPIMPL<double>>(ShellType::pure,
                                                              2, &cs, &es);
    auto ptr2 = std::make_unique<detail_::PointPIMPL<double>>(7.0, 8.0, 9.0);
    Shell<double> s(std::move(ptr1), std::move(ptr2));
    auto [shell2, cg] = make_shell();
    check_state(s, ShellType::pure, 2, cg);
}

TEST_CASE("Shell : pure()") {
    auto [s, cg] = make_shell();
    REQUIRE(s.pure() == ShellType::pure);
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<ShellType&, decltype(s.pure())>);
    }
}

TEST_CASE("Shell : pure() const") {
    const auto [s, cg] = make_shell();
    REQUIRE(s.pure() == ShellType::pure);
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const ShellType&, decltype(s.pure())>);
    }
}

TEST_CASE("Shell : l()") {
    auto [s, cg] = make_shell();
    REQUIRE(s.l() == 2);
    SECTION("Is read-/write-able") {
        STATIC_REQUIRE(std::is_same_v<std::size_t&, decltype(s.l())>);
    }
}

TEST_CASE("Shell : l() const") {
    const auto [s, cg] = make_shell();
    REQUIRE(s.l() == 2);
    SECTION("Is read-only") {
        STATIC_REQUIRE(std::is_same_v<const std::size_t&, decltype(s.l())>);
    }
}

TEST_CASE("Shell : n_unique_primitives") {
    const auto [s, cg] = make_shell();
    REQUIRE(s.n_unique_primitives() == 3);
}

TEST_CASE("Shell : unique_primitive") {
    auto [s, cg] = make_shell();
    REQUIRE(s.unique_primitive(0) == cg[0]);
    REQUIRE(s.unique_primitive(1) == cg[1]);
    REQUIRE(s.unique_primitive(2) == cg[2]);
}

TEST_CASE("Shell : unique_primitive() const") {
    const auto [s, cg] = make_shell();
    REQUIRE(s.unique_primitive(0) == cg[0]);
    REQUIRE(s.unique_primitive(1) == cg[1]);
    REQUIRE(s.unique_primitive(2) == cg[2]);
}

TEST_CASE("Shell : size_()") {
    auto [s, cg] = make_shell();
    REQUIRE(s.size() == 5);
}

TEST_CASE("Shell : at_()") {
    auto [s, cg] = make_shell();
    REQUIRE(s[0] == cg);
}

TEST_CASE("Shell : at_() const") {
    const auto [s, cg] = make_shell();
    REQUIRE(s[0] == cg);
}

TEST_CASE("Shell : comparisons") {
    vector_t cs{1.0, 2.0, 3.0};
    vector_t es{4.0, 5.0, 6.0};
    ContractedGaussian<double> cg(cs, es, 7.0, 8.0, 9.0);
    Shell<double> s(ShellType::pure, 2, cs, es, 7.0, 8.0, 9.0);

    SECTION("Same shell") {
        REQUIRE(s == s);
        REQUIRE_FALSE(s != s);
    }
    SECTION("Different purity") {
        Shell<double> s2(ShellType::cartesian, 2, cs, es, 7.0, 8.0, 9.0);
        REQUIRE_FALSE(s == s2);
        REQUIRE(s != s2);
    }
    SECTION("Different total angular momentum") {
        Shell<double> s2(ShellType::pure, 3, cs, es, 7.0, 8.0, 9.0);
        REQUIRE_FALSE(s == s2);
        REQUIRE(s != s2);
    }
    SECTION("Different contraction coefficients") {
        Shell<double> s2(ShellType::pure, 2, es, es, 7.0, 8.0, 9.0);
        REQUIRE_FALSE(s == s2);
        REQUIRE(s != s2);
    }
    SECTION("Different exponents") {
        Shell<double> s2(ShellType::pure, 2, cs, cs, 7.0, 8.0, 9.0);
        REQUIRE_FALSE(s == s2);
        REQUIRE(s != s2);
    }
    SECTION("Different center") {
        Shell<double> s2(ShellType::pure, 2, cs, es, 8.0, 8.0, 9.0);
        REQUIRE_FALSE(s == s2);
        REQUIRE(s != s2);
    }
}

TEST_CASE("Shell serialization") {
    auto [s, cg] = make_shell();
    Shell<double> s2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(s);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(s2);
    }
    REQUIRE(s == s2);
}
