#include "../../test_tensor.hpp"

using namespace libchemist;

/* Testing Notes:
 *
 * For construction purposes we don't care if the labels make any sense. For
 * example we're fine with `lhs("i0,i1") * rhs("i0;i1")` even though for a
 * tensor times a ToT contraction can only occur over the independent indices.
 * What we are testing for construction purposes is that we've covered all of
 * the possible products.
 *
 * Actual correctness of the contractions are spotted check later. It is
 * assumed that the underlying tensor library can contract correctly so we
 * don't have to check all of the possible contractions here too. What we are
 * mainly interested in is that the TensorWrappers are const-correct and can
 * be used to get the correct result.
 */
TEST_CASE("Tensor = Tensor * Tensor") {
    using result_t       = type::tensor<double>;
    using lhs_t          = type::tensor<double>;
    using rhs_t          = type::tensor<double>;
    using tensor_wrapper = tensor::SparseTensorWrapper;

    auto lhs = testing::get_tensors<lhs_t>().at("vector");
    auto rhs = testing::get_tensors<rhs_t>().at("vector");

    auto& world = TA::get_default_world();
    result_t corr;
    corr("i,j") = lhs("i") * rhs("j");

    SECTION("all non-const") {
        tensor_wrapper wrapped_lhs(lhs);
        tensor_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i,j") = wrapped_lhs("i") * wrapped_rhs("j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }

    SECTION("LHS is const") {
        const tensor_wrapper wrapped_lhs(lhs);
        tensor_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i,j") = wrapped_lhs("i") * wrapped_rhs("j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }

    SECTION("RHS is const") {
        tensor_wrapper wrapped_lhs(lhs);
        const tensor_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i,j") = wrapped_lhs("i") * wrapped_rhs("j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }

    SECTION("All are const") {
        const tensor_wrapper wrapped_lhs(lhs);
        const tensor_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i,j") = wrapped_lhs("i") * wrapped_rhs("j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }
}

TEST_CASE("Tensor = ToT * ToT") {
    using result_t       = type::tensor<double>;
    using lhs_t          = type::tensor_of_tensors<double>;
    using rhs_t          = type::tensor_of_tensors<double>;
    using tensor_wrapper = tensor::SparseTensorWrapper;
    using tot_wrapper    = tensor::ToTWrapper;

    auto lhs = testing::get_tensors<lhs_t>().at("vector-of-vectors");
    auto rhs = testing::get_tensors<rhs_t>().at("vector-of-vectors");

    auto& world = TA::get_default_world();
    result_t corr;
    TA::expressions::einsum(corr("i"), lhs("i;j"), rhs("i;j"));

    SECTION("all non-const") {
        tot_wrapper wrapped_lhs(lhs);
        tot_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i") = wrapped_lhs("i;j") * wrapped_rhs("i;j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }

    SECTION("LHS is const") {
        const tot_wrapper wrapped_lhs(lhs);
        tot_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i") = wrapped_lhs("i;j") * wrapped_rhs("i;j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }

    SECTION("RHS is const") {
        tot_wrapper wrapped_lhs(lhs);
        const tot_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i") = wrapped_lhs("i;j") * wrapped_rhs("i;j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }

    SECTION("All are const") {
        const tot_wrapper wrapped_lhs(lhs);
        const tot_wrapper wrapped_rhs(rhs);
        tensor_wrapper result(result_t{});
        result("i") = wrapped_lhs("i;j") * wrapped_rhs("i;j");
        REQUIRE(ta_helpers::allclose(result.get<result_t>(), corr));
    }
}

TEST_CASE("ToT = Tensor * ToT") {
    using result_t       = type::tensor_of_tensors<double>;
    using lhs_t          = type::tensor<double>;
    using rhs_t          = type::tensor_of_tensors<double>;
    using wrapped_result_t = tensor::ToTWrapper;
    using wrapped_lhs_t    = tensor::SparseTensorWrapper;
    using wrapped_rhs_t    = tensor::ToTWrapper;

    auto lhs = testing::get_tensors<lhs_t>().at("matrix");
    auto rhs = testing::get_tensors<rhs_t>().at("matrix-of-vectors");
    const auto result_idx = "i,j;k";
    const auto lhs_idx    = "i,j";
    const auto rhs_idx    = "i,j;k";

    auto& world = TA::get_default_world();
    result_t corr;
    TA::expressions::einsum(corr(result_idx), lhs(lhs_idx), rhs(rhs_idx));

    SECTION("all non-const") {
        wrapped_lhs_t wrapped_lhs(lhs);
        wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("LHS is const") {
        const wrapped_lhs_t wrapped_lhs(lhs);
        wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("RHS is const") {
        wrapped_lhs_t wrapped_lhs(lhs);
        const wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("all const") {
        const wrapped_lhs_t wrapped_lhs(lhs);
        const wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

}

TEST_CASE("ToT = ToT * Tensor") {
    using result_t       = type::tensor_of_tensors<double>;
    using lhs_t          = type::tensor_of_tensors<double>;
    using rhs_t          = type::tensor<double>;
    using wrapped_result_t = tensor::ToTWrapper;
    using wrapped_lhs_t    = tensor::ToTWrapper;
    using wrapped_rhs_t    = tensor::SparseTensorWrapper;

    auto lhs = testing::get_tensors<lhs_t>().at("matrix-of-vectors");
    auto rhs = testing::get_tensors<rhs_t>().at("matrix");
    const auto result_idx = "i,j;k";
    const auto lhs_idx    = "i,j;k";
    const auto rhs_idx    = "i,j";

    auto& world = TA::get_default_world();
    result_t corr;
    TA::expressions::einsum(corr(result_idx), rhs(rhs_idx), lhs(lhs_idx));

    SECTION("all non-const") {
        wrapped_lhs_t wrapped_lhs(lhs);
        wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("LHS const") {
        const wrapped_lhs_t wrapped_lhs(lhs);
        wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("RHS const") {
        wrapped_lhs_t wrapped_lhs(lhs);
        const wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("all const") {
        const wrapped_lhs_t wrapped_lhs(lhs);
        const wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }
}

TEST_CASE("ToT = ToT * ToT") {
    using result_t       = type::tensor_of_tensors<double>;
    using lhs_t          = type::tensor_of_tensors<double>;
    using rhs_t          = type::tensor_of_tensors<double>;
    using wrapped_result_t = tensor::ToTWrapper;
    using wrapped_lhs_t    = tensor::ToTWrapper;
    using wrapped_rhs_t    = tensor::ToTWrapper;

    auto lhs = testing::get_tensors<lhs_t>().at("matrix-of-vectors");
    auto rhs = testing::get_tensors<rhs_t>().at("matrix-of-vectors");
    const auto result_idx = "i,j;k";
    const auto lhs_idx    = "i,j;k";
    const auto rhs_idx    = "i,j;k";

    auto& world = TA::get_default_world();
    result_t corr;
    TA::expressions::einsum(corr(result_idx), lhs(lhs_idx), rhs(rhs_idx));

    SECTION("all non-const") {
        wrapped_lhs_t wrapped_lhs(lhs);
        wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("LHS const") {
        const wrapped_lhs_t wrapped_lhs(lhs);
        wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("RHS const") {
        wrapped_lhs_t wrapped_lhs(lhs);
        const wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }

    SECTION("all const") {
        const wrapped_lhs_t wrapped_lhs(lhs);
        const wrapped_rhs_t wrapped_rhs(rhs);
        wrapped_result_t result(result_t{});
        result(result_idx) = wrapped_lhs(lhs_idx) * wrapped_rhs(rhs_idx);
        REQUIRE(ta_helpers::allclose_tot(result.get<result_t>(), corr, 1));
    }
}
