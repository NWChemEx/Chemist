#include "../../sparse_map/test_sparse_map.hpp"
#include "libchemist/tensor/detail_/sparse_map_to_shape.hpp"
#include "libchemist/tensor/tensor.hpp"

using namespace libchemist::tensor::detail_;

namespace {

// Code factorization for converting the correct values into a shape object
auto il2shape(const TA::TiledRange& tr, std::initializer_list<float> il) {
    using scalar_type = float;
    using tensor_type = TA::Tensor<scalar_type>;
    using shape_type  = TA::SparseShape<scalar_type>;

    tensor_type corr_tensor(tr.elements_range(), il.begin());
    return shape_type(corr_tensor, tr);
}

} // namespace

TEST_CASE("sparse_map_to_shape") {
    using index_type      = libchemist::sparse_map::ElementIndex;
    using sparse_map_type = libchemist::sparse_map::SparseMapEE;
    using tr1_type        = TA::TiledRange1;
    using tr_type         = TA::TiledRange;

    auto [i0, i1, i2, i3, i4] = testing::make_indices<index_type, 1>();
    auto max                  = std::numeric_limits<float>::max();

    SECTION("Matrix") {
        sparse_map_type sm{
          {i0, {i2, i3}}, {i1, {i1}}, {i2, {i0, i3}}, {i3, {i0, i1}}};
        tr_type tr{tr1_type{0, 1, 2, 3, 4}, tr1_type{0, 1, 2, 3, 4}};

        SECTION("Same order") {
            idx2mode_type idx2mode{0, 1};
            auto shape = sparse_map_to_shape(sm, idx2mode, tr);
            auto corr  = il2shape(tr, {0.0, 0.0, max, max, 0.0, max, 0.0, 0.0,
                                      max, 0.0, 0.0, max, max, max, 0.0, 0.0});
            REQUIRE(shape == corr);
        }

        SECTION("Transposed") {
            idx2mode_type idx2mode{1, 0};
            auto shape = sparse_map_to_shape(sm, idx2mode, tr);
            auto corr  = il2shape(tr, {0.0, 0.0, max, max, 0.0, max, 0.0, max,
                                      max, 0.0, 0.0, 0.0, max, 0.0, max, 0.0});
            REQUIRE(shape == corr);
        }
    }

    SECTION("Tensor") {
        auto [i00, i01, i10, i11] = testing::make_indices<index_type, 2>();

        SECTION("Ind rank 1, Dep rank 2") {
            sparse_map_type sm{{i0, {i00, i10}}, {i1, {i00, i01}}, {i2, {i11}}};
            tr1_type tr03{0, 1, 2, 3}, tr02{0, 1, 2};
            tr_type tr{tr03, tr02, tr02};

            SECTION("Same order") {
                idx2mode_type idx2mode{0, 1, 2};
                auto shape = sparse_map_to_shape(sm, idx2mode, tr);
                auto corr  = il2shape(tr, {max, 0.0, max, 0.0, max, max, 0.0,
                                          0.0, 0.0, 0.0, 0.0, max});
                REQUIRE(shape == corr);
            }

            SECTION("Different order") {
                idx2mode_type idx2mode{0, 2, 1};
                auto shape = sparse_map_to_shape(sm, idx2mode, tr);
                auto corr  = il2shape(tr, {max, max, 0.0, 0.0, max, 0.0, max,
                                          0.0, 0.0, 0.0, 0.0, max});
                REQUIRE(shape == corr);
            }
        }

        SECTION("Ind rank 2, Dep rank 1") {
            sparse_map_type sm{
              {i00, {i1, i2}}, {i01, {i0, i3}}, {i10, {i1}}, {i11, {i0, i3}}};
            tr1_type tr02{0, 1, 2}, tr04{0, 1, 2, 3, 4};
            tr_type tr{tr02, tr02, tr04};

            SECTION("Same order") {
                idx2mode_type idx2mode{0, 1, 2};
                auto shape = sparse_map_to_shape(sm, idx2mode, tr);
                auto corr =
                  il2shape(tr, {0.0, max, max, 0.0, max, 0.0, 0.0, max, 0.0,
                                max, 0.0, 0.0, max, 0.0, 0.0, max});
                REQUIRE(shape == corr);
            }

            SECTION("Different order") {
                idx2mode_type idx2mode{1, 0, 2};
                auto shape = sparse_map_to_shape(sm, idx2mode, tr);
                auto corr =
                  il2shape(tr, {0.0, max, max, 0.0, 0.0, max, 0.0, 0.0, max,
                                0.0, 0.0, max, max, 0.0, 0.0, max});
                REQUIRE(shape == corr);
            }
        }
    }

    SECTION("Throws if SparseMap is inconsistent with mode map") {
        sparse_map_type sm{{i0, {i1}}};
        tr_type tr{tr1_type{0, 4}, tr1_type{0, 6}};
        idx2mode_type idx2mode{0, 1, 3};
        REQUIRE_THROWS_AS(sparse_map_to_shape(sm, idx2mode, tr),
                          std::runtime_error);
    }

    SECTION("Throws if SparseMap is inconsistent with tiled range") {
        sparse_map_type sm{{i0, {i1}}};
        tr_type tr{tr1_type{0, 4}, tr1_type{0, 6}, tr1_type{0, 7}};
        idx2mode_type idx2mode{0, 1};
        REQUIRE_THROWS_AS(sparse_map_to_shape(sm, idx2mode, tr),
                          std::runtime_error);
    }

    SECTION("Throws if idx2mode maps to a mode out of range") {
        sparse_map_type sm{{i0, {i1}}};
        tr_type tr{tr1_type{0, 4}, tr1_type{0, 6}};
        idx2mode_type idx2mode{0, 3};
        REQUIRE_THROWS_AS(sparse_map_to_shape(sm, idx2mode, tr),
                          std::runtime_error);
    }
}
