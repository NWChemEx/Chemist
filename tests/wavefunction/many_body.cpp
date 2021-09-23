#include "libchemist/wavefunction/many_body.hpp"
#include "test_wavefunction.hpp"
using namespace libchemist::wavefunction;

using tuple_type = std::tuple<Reference, CanonicalReference>;

TEMPLATE_LIST_TEST_CASE("ManyBody", "", tuple_type) {
    using reference_t = TestType;
    using basis_t     = typename reference_t::basis_set_type;
    using wf_t        = ManyBody<reference_t, libchemist::type::tensor>;
    using tensor_type = typename wf_t::tensor_type;

    wf_t defaulted;

    reference_t spin1(basis_t{}, 1.0);
    wf_t non_default_ref(spin1);

    tensor_type t1 = testing::make_tensor<tensor_type>();
    tensor_type t2 = testing::make_tensor<tensor_type>(2);
    wf_t non_default_t2(reference_t{}, t2);
    wf_t non_default_t1(reference_t{}, tensor_type{}, t1);

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.reference_wavefunction() == reference_t{});
            REQUIRE(defaulted.t1() == tensor_type{});
            REQUIRE(defaulted.t2() == tensor_type{});
        }

        SECTION("Value") {
            REQUIRE(non_default_ref.reference_wavefunction() == spin1);
            REQUIRE(non_default_ref.t1() == tensor_type{});
            REQUIRE(non_default_ref.t2() == tensor_type{});

            REQUIRE(non_default_t2.reference_wavefunction() == reference_t{});
            REQUIRE(non_default_t2.t1() == tensor_type{});
            REQUIRE(non_default_t2.t2() == t2);

            REQUIRE(non_default_t1.reference_wavefunction() == reference_t{});
            REQUIRE(non_default_t1.t1() == t1);
            REQUIRE(non_default_t1.t2() == tensor_type{});
        }
    }
}
