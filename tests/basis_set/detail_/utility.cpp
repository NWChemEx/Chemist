#include <catch2/catch.hpp>
#include <libchemist/basis_set/detail_/utility.hpp>

using namespace libchemist::basis_set::detail_;

TEST_CASE("holder") {
    STATIC_REQUIRE(std::is_same_v<holder<double*>, double*>);
    STATIC_REQUIRE(std::is_same_v<holder<double>, std::optional<double>>);
}

struct TestCallMemberX {
    constexpr auto& coefficient() const { return m_c; }
    constexpr auto& exponent(std::size_t) const { return m_a; }
    constexpr auto& center(std::size_t, std::size_t) const { return m_center; }
    constexpr bool has_center(std::size_t, std::size_t, std::size_t) const {
        return true;
    }
    constexpr auto& nprims(std::size_t, std::size_t, std::size_t,
                           std::size_t) const {
        return m_c;
    }
    double m_c      = 1.1;
    double m_a      = 2.2;
    double m_center = 3.3;
};

TEST_CASE("CallMemberX") {
    constexpr TestCallMemberX x;
    constexpr std::optional<TestCallMemberX> opt_x(x);
    constexpr std::array<std::size_t, 0> idx0{};
    constexpr std::array idx1{0ul};
    constexpr std::array idx2{0ul, 0ul};
    constexpr std::array idx3{0ul, 0ul, 0ul};
    constexpr std::array idx4{0ul, 0ul, 0ul, 0ul};
    STATIC_REQUIRE(CallMemberX::coefficient(opt_x, idx0) == x.m_c);
    STATIC_REQUIRE(CallMemberX::exponent(opt_x, idx1) == x.m_a);
    STATIC_REQUIRE(CallMemberX::center(opt_x, idx2) == x.m_center);
    STATIC_REQUIRE(CallMemberX::has_center(opt_x, idx3));
    STATIC_REQUIRE(CallMemberX::nprims(opt_x, idx4));
}
