#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/submatrix_functions.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>
#include <libchemist/types.hpp>

using tensor_type = libchemist::type::tensor<double>;
using libchemist::ta_helpers::allclose;
using libchemist::ta_helpers::submatrix;
using libchemist::ta_helpers::expand_submatrix;

TEST_CASE("Submatrix Functions") {
    auto& world = TA::get_default_world();

    // Make inputs and comparison values
    TA::TiledRange1 tr1{0, 1, 2, 3};
    TA::TiledRange1 tr2{0, 1, 2};

    TA::TiledRange trange1{tr1, tr1};
    TA::TiledRange trange2{tr2, tr2};

    TA::Tensor<float> mask(trange1.tiles_range(), 0.0);
    mask(0, 0) = std::numeric_limits<float>::max();
    mask(0, 2) = std::numeric_limits<float>::max();
    mask(2, 0) = std::numeric_limits<float>::max();
    mask(2, 2) = std::numeric_limits<float>::max();
    TA::SparseShape<float> shape(mask, trange1);

    auto full_matrix = TA::diagonal_array<tensor_type>(world, trange1, 1.0);
    auto corr_submat = TA::diagonal_array<tensor_type>(world, trange2, 1.0);

    tensor_type corr_matrix;
    corr_matrix("i, j") = full_matrix("i, j").set_shape(shape);

    // Test functions
    auto submat = submatrix<double>(full_matrix, mask);
    auto matrix = expand_submatrix<double>(corr_submat, trange1, mask);

    REQUIRE(allclose(matrix, corr_matrix));
    REQUIRE(allclose(submat, corr_submat));
}