#include "libchemist/tensor/tensor.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

namespace {
TA::detail::vector_il<double> eval_data{-0.5157294715892564, 0.1709151888271797,
                                        11.3448142827620728};

TA::detail::matrix_il<double> evec_data{
  {0.7369762290995787, 0.5910090485061027, 0.3279852776056817},
  {0.3279852776056812, -0.7369762290995785, 0.5910090485061033},
  {-0.5910090485061031, 0.3279852776056821, 0.7369762290995784}};

} // namespace

TEST_CASE("eigen_solve") {
    using ivector_il = TA::detail::vector_il<int>;
    using imatrix_il = TA::detail::matrix_il<int>;
    using dvector_il = TA::detail::vector_il<double>;
    using dmatrix_il = TA::detail::matrix_il<double>;
    using TWrapper   = ScalarTensorWrapper;
    auto& world      = TA::get_default_world();
    TA::TSpArrayD data(world,
                       imatrix_il{ivector_il{1, 2, 3}, ivector_il{2, 4, 5},
                                  ivector_il{3, 5, 6}});
    TWrapper X(data);

    TWrapper eval_corr(TA::TSpArrayD(world, eval_data));
    TWrapper evec_corr(TA::TSpArrayD(world, evec_data));

    SECTION("No overlap matrix") {
        const auto& [evals, evecs] = eigen_solve(X);
        SECTION("eigen values") { REQUIRE(allclose(eval_corr, evals)); }
        SECTION("eigen vectors") { REQUIRE(abs_allclose(evec_corr, evecs)); }
    }
    SECTION("With overlap") {
        TA::TSpArrayD ovp(world, dmatrix_il{dvector_il{1.0, 0.0, 0.0},
                                            dvector_il{0.0, 1.0, 0.0},
                                            dvector_il{0.0, 0.0, 1.0}});
        TWrapper S(ovp);
        const auto& [evals, evecs] = eigen_solve(X, S);
        SECTION("eigen values") { REQUIRE(allclose(eval_corr, evals)); }
        SECTION("eigen vectors") { REQUIRE(abs_allclose(evec_corr, evecs)); }
    }
}
