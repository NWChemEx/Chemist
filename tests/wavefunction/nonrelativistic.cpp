#include "libchemist/wavefunction/nonrelativistic.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::wavefunction;

using tuple_type = std::tuple<Reference, CanonicalReference, LocalReference,
                              CanonicalLocalReference>;

TEMPLATE_LIST_TEST_CASE("Nonrelativistic", "", tuple_type) {
    using wf_t = TestType;
    using basis_set_t = typename wf_t::basis_set_type;

    wf_t defaulted;

    SECTION("CTors"){
        SECTION("Default") {
            REQUIRE(defaulted.spin() == 0);
            REQUIRE(defaulted.basis_set() == basis_set_t{});
        }
    }

}
