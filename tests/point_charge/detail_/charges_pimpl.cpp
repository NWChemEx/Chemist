#include <catch2/catch.hpp>
#include <chemist/point_charge/detail_/charges_pimpl.hpp>

using namespace chemist::detail_;

TEMPLATE_TEST_CASE("ChargesPIMPL", "", float, double) {
    using pimpl_type = ChargesPIMPL<TestType>;
    using value_type = typename pimpl_type::value_type;

    value_type q0(1.0, 2.0, 3.0, 4.0);
    value_type q1(5.0, 6.0, 7.0, 8.0);

    pimpl_type defaulted;
    pimpl_type qs;
    qs.push_back(q0);
    qs.push_back(q1);
}
