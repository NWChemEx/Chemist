#include "chemist/basis_set/ao_basis_set.hpp"
#include "chemist/detail_/hashing.hpp"
#include <catch2/catch.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>
#include <utilities/iter_tools/enumerate.hpp>

using bs_t     = chemist::AOBasisSet<double>;
using center_t = chemist::AtomicBasisSet<double>;
using param_t  = typename bs_t::value_type::param_set;

static inline auto make_bs() {
    bs_t bs;
    center_t c("", 0, 1.0, 2.0, 3.0);
    param_t ps{1.0};
    c.add_shell(chemist::ShellType::pure, 0, ps, ps);
    c.add_shell(chemist::ShellType::pure, 2, ps, ps);
    bs.add_center(c);
    return std::make_pair(bs, c);
}

// TEST_CASE("AOBasisSet : default ctor") {
//     bs_t bs;
//     chemist::detail_::Hasher h(chemist::detail_::HashType::Hash128);
//     h(bs);
//     REQUIRE(chemist::detail_::hash_to_string(h.finalize()) ==
//             "00000000000000000000000000000000");
// }

TEST_CASE("AOBasisSet : copy ctor") {
    bs_t bs;
    REQUIRE(bs.empty());
    center_t c("", 0, 1.0, 2.0, 3.0);
    bs.add_center(c);
    bs_t bs2(bs);
    SECTION("State") { REQUIRE(bs2 == bs); }
    SECTION("Is deep copy") { REQUIRE(&bs[0].x() != &bs2[0].x()); }
}

TEST_CASE("AOBasisSet : copy assignment") {
    bs_t bs;
    center_t c("", 0, 1.0, 2.0, 3.0);
    bs.add_center(c);
    bs_t bs2;
    auto pbs2 = &(bs2 = bs);
    SECTION("Returns this") { REQUIRE(pbs2 == &bs2); }
    SECTION("State") { REQUIRE(bs2 == bs); }
    SECTION("Is deep copy") { REQUIRE(&bs[0].x() != &bs2[0].x()); }
}

TEST_CASE("AOBasisSet : move ctor") {
    bs_t bs;
    center_t c("", 0, 1.0, 2.0, 3.0);
    bs.add_center(c);
    bs_t bs2(bs);
    bs_t bs3(std::move(bs));
    SECTION("State") { REQUIRE(bs3 == bs2); }
}

TEST_CASE("AOBasisSet : move assignment") {
    bs_t bs;
    center_t c("", 0, 1.0, 2.0, 3.0);
    bs.add_center(c);
    bs_t bs2(bs);
    bs_t bs3;
    auto pbs3 = &(bs3 = std::move(bs));
    SECTION("Returns this") { REQUIRE(pbs3 == &bs3); }
    SECTION("State") { REQUIRE(bs3 == bs2); }
}

TEST_CASE("AOBasisSet : add_center") {
    bs_t bs;
    center_t c("", 0, 1.0, 2.0, 3.0);
    bs.add_center(c);
    REQUIRE(bs.size() == 1);
    REQUIRE(bs[0] == c);

    // chemist::detail_::Hasher h(chemist::detail_::HashType::Hash128);
    // h(bs);
    // REQUIRE(chemist::detail_::hash_to_string(h.finalize()) ==
    //         "c7c65e5af263ca28eb7d099cf993f8af");
}

TEST_CASE("AOBasisSet : max_l") {
    auto [bs, c] = make_bs();
    REQUIRE(bs.max_l() == 2);
}

TEST_CASE("AOBasisSet : n_shells") {
    auto [bs, c] = make_bs();
    REQUIRE(bs.n_shells() == 2);
}

TEST_CASE("AOBasisSet : shell()") {
    auto [bs, c] = make_bs();
    SECTION("Values") {
        REQUIRE(bs.shell(0) == c[0]);
        REQUIRE(bs.shell(1) == c[1]);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(bs.shell(2), std::out_of_range);
    }
}

TEST_CASE("AOBasisSet : shell() const") {
    const auto [bs, c] = make_bs();
    SECTION("Values") {
        REQUIRE(bs.shell(0) == c[0]);
        REQUIRE(bs.shell(1) == c[1]);
    }
    SECTION("Throws if out of range") {
        REQUIRE_THROWS_AS(bs.shell(2), std::out_of_range);
    }
}

TEST_CASE("AOBasisSet : shells()") {
    auto [bs, c] = make_bs();
    std::array corr{c[0], c[1]};
    for(auto&& [i, shell_i] : utilities::Enumerate(bs.shells())) {
        REQUIRE(shell_i == corr[i]);
    }
}

TEST_CASE("AOBasisSet : shells() const") {
    const auto [bs, c] = make_bs();
    std::array corr{c[0], c[1]};
    for(auto&& [i, shell_i] : utilities::Enumerate(bs.shells())) {
        REQUIRE(shell_i == corr[i]);
    }
}

TEST_CASE("AOBasisSet : shell_offsets()") {
    auto [bs, c] = make_bs();
    bs.add_center(c);
    auto shell_offsets = bs.shell_offsets();
    REQUIRE(shell_offsets == std::vector<bs_t::size_type>{0, 2, 4});
}

TEST_CASE("AOBasisSet : n_aos()") {
    auto [bs, c] = make_bs();
    REQUIRE(bs.n_aos() == 6);
}

TEST_CASE("AOBasisSet : ao()") {
    auto [bs, c] = make_bs();
    SECTION("Values") {
        REQUIRE(bs.ao(0) == c[0][0]);
        REQUIRE(bs.ao(1) == c[1][0]);
        REQUIRE(bs.ao(2) == c[1][1]);
        REQUIRE(bs.ao(3) == c[1][2]);
        REQUIRE(bs.ao(4) == c[1][3]);
        REQUIRE(bs.ao(5) == c[1][4]);
    }
    SECTION("Throws out of range") {
        REQUIRE_THROWS_AS(bs.ao(6), std::out_of_range);
    }
}

TEST_CASE("AOBasisSet : ao() const") {
    const auto [bs, c] = make_bs();
    SECTION("Values") {
        REQUIRE(bs.ao(0) == c[0][0]);
        REQUIRE(bs.ao(1) == c[1][0]);
        REQUIRE(bs.ao(2) == c[1][1]);
        REQUIRE(bs.ao(3) == c[1][2]);
        REQUIRE(bs.ao(4) == c[1][3]);
        REQUIRE(bs.ao(5) == c[1][4]);
    }
    SECTION("Throws out of range") {
        REQUIRE_THROWS_AS(bs.ao(6), std::out_of_range);
    }
}

TEST_CASE("AOBasisSet : aos()") {
    auto [bs, c] = make_bs();
    std::array corr{c[0][0], c[1][0], c[1][1], c[1][2], c[1][3], c[1][4]};
    for(auto&& [i, ao_i] : utilities::Enumerate(bs.aos())) {
        REQUIRE(ao_i == corr[i]);
    }
}

TEST_CASE("AOBasisSet : aos() const") {
    const auto [bs, c] = make_bs();
    std::array corr{c[0][0], c[1][0], c[1][1], c[1][2], c[1][3], c[1][4]};
    for(auto&& [i, ao_i] : utilities::Enumerate(bs.aos())) {
        REQUIRE(ao_i == corr[i]);
    }
}

TEST_CASE("AOBasisSet : ao_offsets()") {
    auto [bs, c] = make_bs();
    bs.add_center(c);
    auto ao_offsets = bs.ao_offsets();
    REQUIRE(ao_offsets == std::vector<bs_t::size_type>{0, 6, 12});
}

TEST_CASE("AOBasisSet : n_unique_primitives()") {
    auto [bs, c] = make_bs();
    REQUIRE(bs.n_unique_primitives() == 2);
}

TEST_CASE("AOBasisSet : unique_primitive()") {
    auto [bs, c] = make_bs();
    SECTION("Values") {
        REQUIRE(bs.unique_primitive(0) == c[0][0][0]);
        REQUIRE(bs.unique_primitive(1) == c[1][0][0]);
    }
}

TEST_CASE("AOBasisSet : unique_primitive() const") {
    const auto [bs, c] = make_bs();
    SECTION("Values") {
        REQUIRE(bs.unique_primitive(0) == c[0][0][0]);
        REQUIRE(bs.unique_primitive(1) == c[1][0][0]);
    }
}

TEST_CASE("AOBasisSet : unique_primitives()") {
    auto [bs, c] = make_bs();
    std::array corr{c[0][0][0], c[1][0][0]};
    for(auto&& [i, prim_i] : utilities::Enumerate(bs.unique_primitives())) {
        REQUIRE(prim_i == corr[i]);
    }
}

TEST_CASE("AOBasisSet : unique_primitives() const") {
    const auto [bs, c] = make_bs();
    std::array corr{c[0][0][0], c[1][0][0]};
    for(auto&& [i, prim_i] : utilities::Enumerate(bs.unique_primitives())) {
        REQUIRE(prim_i == corr[i]);
    }
}

TEST_CASE("AOBasisSet : operator+=") {
    auto [bs, c] = make_bs();
    bs_t rhs;
    center_t c2("", 0, 4.0, 5.0, 6.0);
    rhs.add_center(c2);
    auto pbs = &(bs += rhs);
    REQUIRE(pbs == &bs);
    REQUIRE(bs.size() == 2);
    REQUIRE(bs[0] == c);
    REQUIRE(bs[1] == c2);
}

TEST_CASE("AOBasisSet : operator+") {
    const auto [bs, c] = make_bs();
    bs_t rhs;
    center_t c2("", 0, 4.0, 5.0, 6.0);
    rhs.add_center(c2);
    auto rv = bs + rhs;
    REQUIRE(rv.size() == 2);
    REQUIRE(rv[0] == c);
    REQUIRE(rv[1] == c2);
}

TEST_CASE("AOBasisSet serialization") {
    const auto [bs, c] = make_bs();
    bs_t bs2;
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(bs);
    }
    {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(bs2);
    }
    REQUIRE(bs == bs2);
}
