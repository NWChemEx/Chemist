#include "libchemist/tensor/tensor.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;

TEST_CASE("LabeledTensorWrapper") {
    using tensor         = SparseTensorWrapper;
    using labeled_tensor = detail_::LabeledTensorWrapper<tensor>;

    auto& world = TA::get_default_world();
    libchemist::type::tensor<double> ta_tensor(world, {1, 2, 3});
    tensor t(ta_tensor);
    labeled_tensor lt("i", t);

    SECTION("Typedefs") {
        SECTION("annotation_type") {
            using type = typename labeled_tensor::annotation_type;
            using corr = std::string;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }

        SECTION("tensor_wrapper_type") {
            using type = typename labeled_tensor::tensor_wrapper_type;
            using corr = tensor;
            STATIC_REQUIRE(std::is_same_v<type, corr>);
        }
    }
}
