#include "../../test_tensor.hpp"

using namespace libchemist::tensor;
using vector_il = TA::detail::vector_il<double>;
using matrix_il = TA::detail::matrix_il<double>;
using tensor_il = TA::detail::tensor3_il<double>;

using scalar_traits  = backends::TiledArrayTraits<field::Scalar>;
using scalar_variant = typename scalar_traits::variant_type;

TEMPLATE_LIST_TEST_CASE("ScaleOp", "", scalar_variant) {
    auto& world    = TA::get_default_world();
    using TWrapper = ScalarTensorWrapper;
    using t_type   = TestType;

    // Declare the TA tensors
    t_type ta_vec(world, vector_il{1.0, 2.0, 3.0});
    t_type ta_matrix(world,
                     matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}});
    t_type ta_tensor(
      world, tensor_il{matrix_il{vector_il{1.0, 2.0}, vector_il{3.0, 4.0}},
                       matrix_il{vector_il{5.0, 6.0}, vector_il{7.0, 8.0}}});

    // Declare non-const wrappers
    TWrapper vec(ta_vec);
    TWrapper mat(ta_matrix);
    TWrapper t3(ta_tensor);

    // Declare const wrappers
    const TWrapper const_vec(ta_vec);
    const TWrapper const_mat(ta_matrix);
    const TWrapper const_t3(ta_tensor);

    // Get LabeledTensorWrappers
    auto lvec       = vec("i");
    auto const_lvec = const_vec("i");
    auto lmat       = mat("i,j");
    auto const_lmat = const_mat("i,j");
    auto lt3        = t3("i,j,k");
    auto const_lt3  = const_t3("i,j,k");

    TWrapper result(t_type{});
    SECTION("operator*(double)") {
        SECTION("vector") {
            result("i") = lvec * 3.0;
            t_type corr(world, vector_il{3.0, 6.0, 9.0});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            result("i,j") = lmat * 3.0;
            t_type corr(world,
                        matrix_il{vector_il{3.0, 6.0}, vector_il{9.0, 12.0}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            result("i,j,k") = lt3 * 3.0;
            t_type corr(world, tensor_il{matrix_il{vector_il{3.0, 6.0},
                                                   vector_il{9.0, 12.0}},
                                         matrix_il{vector_il{15.0, 18.0},
                                                   vector_il{21.0, 24.0}}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }

    SECTION("double * OpWrapper") {
        SECTION("vector") {
            result("i") = 3.0 * lvec;
            t_type corr(world, vector_il{3.0, 6.0, 9.0});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("matrix") {
            result("i,j") = 3.0 * lmat;
            t_type corr(world,
                        matrix_il{vector_il{3.0, 6.0}, vector_il{9.0, 12.0}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
        SECTION("rank-3 tensor") {
            result("i,j,k") = 3.0 * lt3;
            t_type corr(world, tensor_il{matrix_il{vector_il{3.0, 6.0},
                                                   vector_il{9.0, 12.0}},
                                         matrix_il{vector_il{15.0, 18.0},
                                                   vector_il{21.0, 24.0}}});
            auto& rv = result.get<t_type>();
            REQUIRE(libchemist::ta_helpers::allclose(rv, corr));
        }
    }
}
