#include "chemist/basis_set/atomic_basis_set.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

using namespace chemist;

static inline auto make_center() {
    AtomicBasisSet<double> c("custom", 1000, 1.0, 2.0, 3.0);
    std::vector<double> cs{4.0};
    std::vector<double> es{5.0};
    c.add_shell(ShellType::pure, 2, cs, es);
    c.add_shell(ShellType::pure, 2, cs, es);
    Shell<double> s(ShellType::pure, 2, cs, es, 1.0, 2.0, 3.0);
    return std::make_pair(c, s);
}

TEST_CASE("AtomicBasisSet : default ctor") {
    AtomicBasisSet<double> c;
    SECTION("Size") { REQUIRE(c.size() == 0); }
    SECTION("Coordinates") {
        REQUIRE(c.x() == 0.0);
        REQUIRE(c.y() == 0.0);
        REQUIRE(c.z() == 0.0);
    }
    SECTION("Basis Set Name") { REQUIRE(c.basis_set_name() == ""); }
    SECTION("Atomic Number") { REQUIRE(c.atomic_number() == 0); }
}

TEST_CASE("AtomicBasisSet : all values ctor") {
    AtomicBasisSet<double> c("custom", 1000, 1.0, 2.0, 3.0);
    SECTION("State") {
        SECTION("Size") { REQUIRE(c.size() == 0); }
        SECTION("Coordinates") {
            REQUIRE(c.x() == 1.0);
            REQUIRE(c.y() == 2.0);
            REQUIRE(c.z() == 3.0);
        }
        SECTION("Basis Set Name") { REQUIRE(c.basis_set_name() == "custom"); }
        SECTION("Atomic Number") { REQUIRE(c.atomic_number() == 1000); }
    }
}

TEST_CASE("AtomicBasisSet : x, y, z ctor") {
    AtomicBasisSet<double> c(1.0, 2.0, 3.0);
    SECTION("State") {
        SECTION("Size") { REQUIRE(c.size() == 0); }
        SECTION("Coordinates") {
            REQUIRE(c.x() == 1.0);
            REQUIRE(c.y() == 2.0);
            REQUIRE(c.z() == 3.0);
        }
        SECTION("Basis Set Name") { REQUIRE(c.basis_set_name() == ""); }
        SECTION("Atomic Number") { REQUIRE(c.atomic_number() == 0); }
    }
}

TEST_CASE("AtomicBasisSet : name and atomic number ctor") {
    AtomicBasisSet<double> c("custom", 1000);
    SECTION("State") {
        SECTION("Size") { REQUIRE(c.size() == 0); }
        SECTION("Coordinates") {
            REQUIRE(c.x() == 0.0);
            REQUIRE(c.y() == 0.0);
            REQUIRE(c.z() == 0.0);
        }
        SECTION("Basis Set Name") { REQUIRE(c.basis_set_name() == "custom"); }
        SECTION("Atomic Number") { REQUIRE(c.atomic_number() == 1000); }
    }
}

TEST_CASE("AtomicBasisSet : Copy ctor") {
    auto [c, s] = make_center();
    AtomicBasisSet<double> c2(c);
    REQUIRE(c2 == c);
    SECTION("Is a deep-copy") { REQUIRE(&c2[0].pure() != &c[0].pure()); }
}

TEST_CASE("AtomicBasisSet : Move ctor") {
    auto [c, s] = make_center();
    AtomicBasisSet<double> c2(c);
    AtomicBasisSet<double> c3(std::move(c));
    REQUIRE(c2 == c3);
}

TEST_CASE("AtomicBasisSet : Copy assignment") {
    auto [c, s] = make_center();
    AtomicBasisSet<double> c2;
    auto pc2 = &(c2 = c);
    REQUIRE(c2 == c);
    SECTION("Returns this") { REQUIRE(pc2 == &c2); }
    SECTION("Is a deep-copy") { REQUIRE(&c2[0].pure() != &c[0].pure()); }
}

TEST_CASE("AtomicBasisSet : Move assignment") {
    auto [c, s] = make_center();
    AtomicBasisSet<double> c2(c);
    AtomicBasisSet<double> c3;
    auto pc3 = &(c3 = std::move(c));
    SECTION("Returns this") { REQUIRE(pc3 == &c3); }
    REQUIRE(c2 == c3);
}

TEST_CASE("AtomicBasisSet : add_shell") {
    auto [c, s] = make_center();
    SECTION("State") {
        SECTION("Size") { REQUIRE(c.size() == 2); }
        SECTION("Shells") { REQUIRE(c[0] == s); }
    }
}

TEST_CASE("AtomicBasisSet : n_aos()") {
    auto [c, s] = make_center();
    auto corr   = 2 * s.size();
    REQUIRE(c.n_aos() == corr);
}

TEST_CASE("AtomicBasisSet : ao()") {
    auto [c, s] = make_center();
    for(std::size_t i = 0; i < c.n_aos(); ++i) { REQUIRE(c.ao(i) == s[0]); }
}

TEST_CASE("AtomicBasisSet : ao() const") {
    const auto [c, s] = make_center();
    for(std::size_t i = 0; i < c.n_aos(); ++i) { REQUIRE(c.ao(i) == s[0]); }
}

TEST_CASE("AtomicBasisSet : n_unique_primitives") {
    const auto [c, s] = make_center();
    auto corr         = 2 * s.n_unique_primitives();
    REQUIRE(c.n_unique_primitives() == corr);
}

TEST_CASE("AtomicBasisSet : unique_primitive()") {
    auto [c, s] = make_center();
    for(std::size_t i = 0; i < c.n_unique_primitives(); ++i) {
        auto si = i % s.n_unique_primitives();
        REQUIRE(c.unique_primitive(i) == s.unique_primitive(si));
    }
}

TEST_CASE("AtomicBasisSet : unique_primitive() const") {
    auto [c, s] = make_center();
    for(std::size_t i = 0; i < c.n_unique_primitives(); ++i) {
        auto si = i % s.n_unique_primitives();
        REQUIRE(c.unique_primitive(i) == s.unique_primitive(si));
    }
}

TEST_CASE("AtomicBasisSet : size()") {
    auto [c, s] = make_center();
    REQUIRE(c.size() == 2);
}

TEST_CASE("AtomicBasisSet : at()") {
    auto [c, s] = make_center();
    REQUIRE(c[0] == s);
    REQUIRE(c[1] == s);
}

TEST_CASE("AtomicBasisSet : at() const") {
    const auto [c, s] = make_center();
    REQUIRE(c[0] == s);
    REQUIRE(c[1] == s);
}

TEST_CASE("AtomicBasisSet serialization") {
    auto [c, s] = make_center();
    AtomicBasisSet<double> c2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(c);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(c2);
    }
    REQUIRE(c == c2);
}
