#include "chemist/operators/kinetic.hpp"
#include "chemist/wavefunction/many_body.hpp"
#include "test_wavefunction.hpp"
using namespace chemist::wavefunction;

using tuple_type =
  std::tuple<ManyBodyWf, CanonicalManyBodyWf, SparseManyBodyWf>;

TEMPLATE_LIST_TEST_CASE("ManyBody", "", tuple_type) {
    using wf_t        = TestType;
    using reference_t = typename wf_t::reference_wf_type;
    using basis_t     = typename reference_t::basis_set_type;
    using tensor_type = typename wf_t::amplitude_type;

    wf_t defaulted;

    reference_t spin1(basis_t{}, 1.0);
    wf_t non_default_ref(spin1);

    tensor_type t1 = testing::make_tensor<tensor_type>();
    tensor_type t2 = testing::make_tensor<tensor_type>(2);
    wf_t non_default_t2(reference_t{}, t2);
    wf_t non_default_t1(reference_t{}, tensor_type{}, t1);

    SECTION("CTors") {
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

    SECTION("reference_wavefunction") {
        REQUIRE_THROWS_AS(defaulted.reference_wavefunction(),
                          std::runtime_error);
        REQUIRE(non_default_ref.reference_wavefunction() == spin1);
        REQUIRE(non_default_t2.reference_wavefunction() == reference_t{});
        REQUIRE(non_default_t1.reference_wavefunction() == reference_t{});
    }

    SECTION("amplitudes") {
        using e_t = std::runtime_error;
        REQUIRE_THROWS_AS(defaulted.template amplitudes<0>(), e_t);
        REQUIRE_THROWS_AS(defaulted.template amplitudes<1>(), e_t);
        REQUIRE(non_default_t1.template amplitudes<0>() == t1);
        REQUIRE(non_default_t2.template amplitudes<1>() == t2);
    }

    SECTION("t1") {
        REQUIRE_THROWS_AS(defaulted.t1(), std::runtime_error);
        REQUIRE(non_default_t1.t1() == t1);
    }

    SECTION("t2") {
        REQUIRE_THROWS_AS(defaulted.t2(), std::runtime_error);
        REQUIRE(non_default_t2.t2() == t2);
    }

    SECTION("Comparisons") {
        SECTION("Both default") {
            wf_t rhs;
            REQUIRE(defaulted == rhs);
            REQUIRE_FALSE(defaulted != rhs);
        }

        SECTION("Different reference") {
            wf_t rhs(reference_t{});
            REQUIRE(non_default_ref != rhs);
            REQUIRE_FALSE(non_default_ref == rhs);
        }

        SECTION("Different T1") {
            wf_t rhs(reference_t{});
            REQUIRE(non_default_t1 != rhs);
            REQUIRE_FALSE(non_default_t1 == rhs);
        }

        SECTION("Different T2") {
            wf_t rhs(reference_t{});
            REQUIRE(non_default_t2 != rhs);
            REQUIRE_FALSE(non_default_t2 == rhs);
        }
    }
}

using namespace chemist::orbital_space;

TEST_CASE("ManyBody conversions") {
    auto occ         = testing::make_space<DerivedSpaceD>(1.0);
    auto canon_occ   = testing::make_space<CanonicalSpaceD>(1.0);
    auto sparse_occ  = testing::make_space<CanonicalIndSpace>(1.0);
    auto virt        = testing::make_space<DerivedSpaceD>(2.0);
    auto canon_virt  = testing::make_space<CanonicalSpaceD>(2.0);
    auto sparse_virt = testing::make_space<CanonicalIndSpace>(2.0);
    chemist::operators::Fock fock(chemist::operators::ElectronKinetic{});

    Determinant d(occ, virt, fock);
    CanonicalDeterminant c(canon_occ, canon_virt, fock);
    SparseDeterminant s(sparse_occ, sparse_virt, fock);

    Reference r(d);
    CanonicalReference cr(c);
    SparseReference sr(s);

    ManyBodyWf psi0(r);
    CanonicalManyBodyWf psi1(cr);
    SparseManyBodyWf psi2(sr);

    SECTION("Canonical to Non-Canonical") {
        ManyBodyWf temp(psi1);
        REQUIRE(temp == psi1);
    }

    SECTION("Sparse to Canonical") {
        CanonicalManyBodyWf temp(psi2);
        REQUIRE(temp == psi2);
    }

    SECTION("Sparse to Non-Canonical") {
        ManyBodyWf temp(psi2);
        REQUIRE(temp == psi2);
    }
}
