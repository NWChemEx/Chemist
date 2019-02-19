#include <LibChemist/AOBasisSet.hpp>
#include <bphash/Hasher.hpp>
#include <catch2/catch.hpp>
#include <sstream>

using namespace LibChemist;

using prim_vector  = std::vector<AOPrimitive>;
using coord_type   = typename AOShell::coord_type;
using shell_vector = std::vector<AOShell>;

using value_type      = typename AOBasisSet::value_type;
using reference       = typename AOBasisSet::reference;
using const_reference = typename AOBasisSet::const_reference;
using size_type       = typename AOBasisSet::size_type;
using iterator        = typename AOBasisSet::iterator;
using const_iterator  = typename AOBasisSet::const_iterator;

static const std::array<std::string, 3> corr_hash{
  "00000000000000000000000000000000", // Default CTor
  "677572a854c0950b6e4bc78ede97e565", // One shell
  "2fa3b866fefd837da3033cbe8cabb3f3"  // Two shells
};

template<size_type hash>
void check_bs(AOBasisSet& bs, const shell_vector& shells) {
    const AOBasisSet& const_bs = bs;
    SECTION("Check State") {
        REQUIRE(bs.nshells() == shells.size());
        for(size_type i = 0; i < shells.size(); ++i) {
            REQUIRE(bs.at(i) == shells[i]);
            REQUIRE(const_bs.at(i) == shells[i]);
            REQUIRE(bs[i] == shells[i]);
            REQUIRE(const_bs[i] == shells[i]);
        }
        size_type counter = 0;
        for(auto& shelli : bs) {
            REQUIRE(shelli == shells[counter]);
            ++counter;
        }
        counter = 0;
        for(const auto& shelli : const_bs) {
            REQUIRE(shelli == shells[counter]);
            ++counter;
        }
    }

    bphash::Hasher h(bphash::HashType::Hash128);
    h(bs);
    REQUIRE(bphash::hash_to_string(h.finalize()) == corr_hash[hash]);

    if(!bs.nshells()) return; // Done with empty bases

    SECTION("Check contiguousness") {
        double* cstart     = &(bs[0].coef(0));
        double* astart     = &(bs[0].alpha(0));
        double* cart_start = &(bs[0].center()[0]);

        for(size_type i = 0; i < bs.nshells(); ++i) {
            for(size_type j = 0; j < 3; ++j, ++cart_start)
                REQUIRE(cart_start == &(bs[i].center()[j]));
            for(size_type j = 0; j < bs[i].nprims(); ++j, ++cstart, ++astart) {
                REQUIRE(cstart == &(bs[i].coef(j)));
                REQUIRE(astart == &(bs[i].alpha(j)));
            }
        }
    }
}

TEST_CASE("AOBasisSet class") {
    SECTION("Typedefs") {
        using corr_itr       = typename std::vector<AOShell>::iterator;
        using corr_const_itr = typename std::vector<AOShell>::const_iterator;
        REQUIRE(std::is_same_v<value_type, AOShell>);
        REQUIRE(std::is_same_v<reference, AOShell&>);
        REQUIRE(std::is_same_v<const_reference, const AOShell&>);
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<iterator, corr_itr>);
        REQUIRE(std::is_same_v<const_iterator, corr_const_itr>);
    }

    SECTION("Default Ctor") {
        AOBasisSet bs;
        check_bs<0>(bs, shell_vector{});
    }

    prim_vector prims{{1.23, 2.34}, {3.45, 4.56}, {5.67, 6.78}, {7.89, 8.90}};
    coord_type carts{1.0, 2.0, 3.0};
    AOShell shell1(carts, prims[0], prims[1]);
    AOShell shell2(carts, prims[2], prims[3], 3);
    AOBasisSet bs(shell1);

    SECTION("One Shell") { check_bs<1>(bs, shell_vector{shell1}); }

    bs.push_back(shell2);

    SECTION("Two Shells") { check_bs<2>(bs, shell_vector{shell1, shell2}); }

    // Assignment for AOShells is not allowed at the moment
    //    SECTION("Assignment doesn't mess buffers up") {
    //        bs[0] = shell2;
    //        check_bs(bs, shell_vector{shell2, shell2});
    //
    //        REQUIRE_THROWS_AS(bs[0] = AOShell(prims[0], prims[0], prims[0]),
    //                          std::logic_error);
    //    }

    SECTION("Initializer List") {
        AOBasisSet bs2{shell1, shell2};
        check_bs<2>(bs2, shell_vector{shell1, shell2});
    }

    SECTION("Copy Ctor") {
        AOBasisSet bs2(bs);
        check_bs<2>(bs2, shell_vector{shell1, shell2});
    }

    SECTION("Move Ctor") {
        AOBasisSet bs2(std::move(bs));
        check_bs<2>(bs2, shell_vector{shell1, shell2});
    }

    SECTION("Copy Assignment") {
        AOBasisSet bs2;
        AOBasisSet& pbs = (bs2 = bs);
        check_bs<2>(bs2, shell_vector{shell1, shell2});
        REQUIRE(&pbs == &bs2);
    }

    SECTION("Move Assignment") {
        AOBasisSet bs2;
        AOBasisSet& pbs = (bs2 = std::move(bs));
        check_bs<2>(bs2, shell_vector{shell1, shell2});
        REQUIRE(&pbs == &bs2);
    }

    SECTION("Get the maximum angular momentum") {
        auto[l, itr]  = max_l(bs);
        auto corr_itr = bs.begin();
        ++corr_itr;
        REQUIRE(corr_itr == itr);
        REQUIRE(l == 3);
    }

    SECTION("Printing") {
        std::stringstream ss_corr, ss;
        ss_corr << shell1 << shell2;
        ss << bs;
        REQUIRE(ss.str() == ss_corr.str());
    }
}
