#include <catch2/catch.hpp>
#include <chemist/detail_/aos_facade.hpp>

/*
 * Note that despite the names of the variables in this test, there's nothing
 * actually tying the AoSFacade class to a atomic orbital basis set.  It's just
 * historical that that was the first application of this class.
 */

using namespace chemist::detail_;

// Typedefs only used in this file
using coef_t  = std::vector<double>;
using carts_t = std::array<double, 3>;
using carts_v = std::vector<carts_t>;
using Shell   = AoSElement<double, carts_t>;
using shell_v = std::vector<Shell>;
using AOBasis = AoSFacade<Shell>;

// Forwarding of typedefs from the AoSElement class
using size_type = typename Shell::size_type;
template<size_type FieldI>
using value_type = typename Shell::value_type<FieldI>;
template<size_type FieldI>
using reference = typename Shell::reference<FieldI>;
template<size_type FieldI>
using const_reference = typename Shell::const_reference<FieldI>;
template<size_type FieldI>
using pointer = typename Shell::pointer<FieldI>;
template<size_type FieldI>
using const_pointer = typename Shell::const_pointer<FieldI>;

using value_type2      = typename AOBasis::value_type;
using reference2       = typename AOBasis::reference;
using const_reference2 = typename AOBasis::const_reference;
using pointer2         = typename AOBasis::pointer;
using const_pointer2   = typename AOBasis::const_pointer;
using size_type2       = typename AOBasis::size_type;

void test_AoSElement(Shell& shell_i, const coef_t& cs, const carts_v& carts) {
    const Shell& const_shell = shell_i;
    SECTION("Coefficients") {
        REQUIRE(shell_i.size<0>() == cs.size());
        if(cs.size()) {
            double* pcoef        = shell_i.get<0>();
            const double* pccoef = const_shell.get<0>();
            for(size_type i = 0; i < cs.size(); ++i) {
                REQUIRE(shell_i.at<0>(i) == cs[i]);
                REQUIRE(const_shell.at<0>(i) == cs[i]);
                REQUIRE(pcoef[i] == cs[i]);
                REQUIRE(pccoef[i] == cs[i]);
            }
        }
    }
    SECTION("Coordinates") {
        REQUIRE(shell_i.size<1>() == carts.size());
        if(carts.size()) {
            REQUIRE(shell_i.at<1>() == carts[0]);
            REQUIRE(const_shell.at<1>() == carts[0]);
        }
    }
}

TEST_CASE("AoSElement Class") {
    SECTION("Typedefs and static members") {
        REQUIRE(std::is_same_v<size_type, std::size_t>);
        REQUIRE(std::is_same_v<value_type<0>, double>);
        REQUIRE(std::is_same_v<reference<0>, double&>);
        REQUIRE(std::is_same_v<const_reference<0>, const double&>);
        REQUIRE(std::is_same_v<pointer<0>, double*>);
        REQUIRE(std::is_same_v<const_pointer<0>, const double*>);
        REQUIRE(Shell::n_fields == 2);
    }

    Shell shell_i;
    SECTION("Default Ctor") { test_AoSElement(shell_i, coef_t{}, carts_v{}); }

    coef_t coefs{1.23, 2.34};
    carts_t carts{3.45, 4.56, 5.67};
    shell_i.insert<0>(coefs[0]);
    shell_i.insert<0>(coefs[1]);
    shell_i.insert<1>(carts);

    SECTION("Filled AoSElement") {
        test_AoSElement(shell_i, coefs, carts_v{carts});
    }

    SECTION("Copy Ctor") {
        Shell shell_j(shell_i);
        test_AoSElement(shell_j, coefs, carts_v{carts});

        // Copy Ctor doesn't alias
        REQUIRE(shell_j.get<0>() != shell_i.get<0>());
    }

    SECTION("Move Ctor") {
        Shell shell_j(std::move(shell_i));
        test_AoSElement(shell_j, coefs, carts_v{carts});
    }

    SECTION("Copy assignment") {
        Shell shell_j;
        Shell& pj = (shell_j = shell_i);
        test_AoSElement(shell_j, coefs, carts_v{carts});
        REQUIRE(shell_j.get<0>() != shell_i.get<0>());
        REQUIRE(&pj == &shell_j);
    }

    SECTION("Move assignment") {
        Shell shell_j;
        Shell& pj = (shell_j = std::move(shell_i));
        test_AoSElement(shell_j, coefs, carts_v{carts});
        REQUIRE(&pj == &shell_j);
    }
}

void test_AoSFacade(AOBasis& bs, const shell_v& shells) {
    const AOBasis& const_bs = bs;
    REQUIRE(bs.size() == shells.size());
    if(!bs.size()) return;
    double* pCoef         = bs.at(0).get<0>();
    const double* pcCoef  = const_bs.at(0).get<0>();
    double* pCarts        = &(bs.at(0).at<1>()[0]);
    const double* pcCarts = &(const_bs.at(0).at<1>()[0]);
    for(size_type i = 0, primi = 0, carti = 0; i < shells.size(); ++i) {
        Shell& shell_i          = bs.at(i);
        const Shell& corr_shell = shells[i];
        const Shell& cshell_i   = const_bs.at(i);
        const size_type nprims  = bs.at(i).size<0>();
        REQUIRE(nprims == corr_shell.size<0>());
        for(size_type j = 0; j < nprims; ++j, ++primi) {
            const double ci = corr_shell.at<0>(j);
            REQUIRE(shell_i.at<0>(j) == ci);
            REQUIRE(cshell_i.at<0>(j) == ci);
            REQUIRE(pCoef[primi] == ci);
            REQUIRE(pcCoef[primi] == ci);
        }
        auto corr_carts = corr_shell.at<1>();
        REQUIRE(shell_i.at<1>() == corr_carts);
        REQUIRE(cshell_i.at<1>() == corr_carts);
        for(size_type j = 0; j < 3; ++j, ++carti) {
            REQUIRE(pCarts[carti] == corr_carts[j]);
            REQUIRE(pcCarts[carti] == corr_carts[j]);
        }
    }
}

TEST_CASE("AoSFacade Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<value_type2, Shell>);
        REQUIRE(std::is_same_v<reference2, Shell&>);
        REQUIRE(std::is_same_v<const_reference2, const Shell&>);
        REQUIRE(std::is_same_v<pointer2, std::shared_ptr<Shell>>);
        REQUIRE(std::is_same_v<const_pointer2, std::shared_ptr<const Shell>>);
        REQUIRE(std::is_same_v<size_type2, size_type>);
    }

    AOBasis bs;
    SECTION("Default CTor") { test_AoSFacade(bs, shell_v{}); }

    shell_v shells(2);
    coef_t coefs{1.23, 2.34, 3.45, 4.56};
    carts_v carts{{5.67, 6.78, 7.89}, {8.90, 9.01, 0.12}};
    for(size_t i = 0; i < 2; ++i) {
        shells[i].insert<0>(coefs[i * 2]);
        shells[i].insert<0>(coefs[i * 2 + 1]);
        shells[i].insert<1>(carts[i]);
    }

    bs.insert(shells[0]);
    bs.insert(shells[1]);

    SECTION("Filled instance") { test_AoSFacade(bs, shells); }

    SECTION("Copy CTor") {
        AOBasis bs2(bs);
        test_AoSFacade(bs2, shells);
        REQUIRE(bs2.at(0).get<0>() != bs.at(0).get<0>());
    }

    SECTION("Move CTor") {
        AOBasis bs2(std::move(bs));
        test_AoSFacade(bs2, shells);
    }

    SECTION("Copy Assignment") {
        AOBasis bs2;
        AOBasis& pbs = (bs2 = bs);
        test_AoSFacade(bs2, shells);
        REQUIRE(&pbs == &bs2);
        REQUIRE(bs2.at(0).get<0>() != bs.at(0).get<0>());
    }

    SECTION("Move Assignment") {
        AOBasis bs2;
        AOBasis& pbs = (bs2 = std::move(bs));
        test_AoSFacade(bs2, shells);
        REQUIRE(&pbs == &bs2);
    }
}
