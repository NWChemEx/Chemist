#include <catch2/catch.hpp>
#include <libchemist/basis_set/detail_/ao_basis_set_pimpl.hpp>

using namespace libchemist::basis_set;

TEST_CASE("Default AOBasisSetPIMPL ctor") {
    detail_::AOBasisSetPIMPL<> bs;
    REQUIRE(bs.size() == 0);
}

TEST_CASE("AOBasisSetPIMPL") {
    detail_::AOBasisSetPIMPL<> bs;
    std::array r0{1.1, 2.2, 3.3};
    std::array r1{4.4, 5.5, 6.6};
    std::array c0{1.2, 1.3};
    std::array c1{1.4, 1.5, 1.6};
    std::array a0{2.1, 2.2};
    std::array a1{2.3, 2.4, 2.5};
    std::array l0{1};
    std::array l1{3, 2};
    std::array p0{true};
    std::array p1{true, false};
    std::array pps0{2};
    std::array pps1{1, 2};
    auto center0 = make_gaussian_center(r0, l0, p0, pps0, c0, a0);
    auto center1 = make_gaussian_center(r1, l1, p1, pps1, c1, a1);
    bs.add_shell(r0, 1, std::vector{1.2, 1.3}, std::vector{2.1, 2.3}, true);
    bs.add_shell(r1, 3, std::vector{1.4}, std::vector{2.3}, true);
    bs.add_shell(r1, 2, std::vector{1.5, 1.6}, std::vector{2.4, 2.5}, false);
    REQUIRE(bs.size() == 2);
    REQUIRE(bs[0] == center0);
    REQUIRE(bs[1] == center1);
}
