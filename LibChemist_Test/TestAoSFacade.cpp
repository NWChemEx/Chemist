#include <LibChemist/Implementations/AoSFacade.hpp>
#include <catch/catch.hpp>

/*
 * Note that despite the names of the variables in this test, there's nothing
 * actually tying the AoSFacade class to a atomic orbital basis set.  It's just
 * historical that that was the first application of this class.
 */

using namespace LibChemist::detail_;

//Typedefs only used in this file
using coef_t = std::vector<double>;
using carts_t = std::array<double,3>;
using carts_v = std::vector<carts_t>;
using Shell = AoSElement<double, carts_t>;
using shell_v = std::vector<Shell>;
using AOBasis = AoSFacade<Shell>;

//Forwarding of typedefs from the AoSElement class
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


void test_AoSElement(Shell& shell_i, const coef_t& cs, const carts_v& carts) {
    const Shell& const_shell = shell_i;
    SECTION("Coefficients") {
        REQUIRE(shell_i.size<0>() == cs.size());
        if(cs.size()) {
            double* pcoef = shell_i.get<0>();
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
    SECTION("Default Ctor") {
        test_AoSElement(shell_i, coef_t{}, carts_v{});
    }

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

        //Copy Ctor doesn't alias
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


void test_AoSFacade(AOBasis& bs, const shell_v& shells){
    const AOBasis& const_bs = bs;
    REQUIRE(bs.size() == shells.size());
    for(size_type i=0; i < shells.size(); ++i ){
        REQUIRE(bs.at(i).size<0>() == shells[i].size<0>());
        for(size_type j=0; j < bs.at(i).size<0>(); ++j){

        }
    }
}

TEST_CASE("AoSFacade Class") {
    SECTION("Typedefs") {
        REQUIRE(std::is_same_v<typename AOBasis::value_type, Shell>);
        REQUIRE(std::is_same_v<typename AOBasis::size_type, size_type>);
    }

    AOBasis bs;
    SECTION("Default CTor") {
        test_AoSFacade(bs, shell_v{});
    }

    shell_v shells(2);
    coef_t coefs{1.23, 2.34, 3.45, 4.56};
    carts_v carts{{5.67, 6.78, 7.89}, {8.90, 9.01, 0.12}};
    for(size_t i=0; i<2; ++i) {
        shells[i].insert<0>(coefs[i*2]);
        shells[i].insert<0>(coefs[i*2 + 1]);
        shells[i].insert<1>(carts[i]);
    }

    bs.insert(shells[0]);
    bs.insert(shells[1]);

    SECTION("Filled instance") {
        test_AoSFacade(bs, shells);
    }

}
