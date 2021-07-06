#include "libchemist/tensor/type_traits/labeled_variant.hpp"
#include "libchemist/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;
using namespace libchemist::tensor;

using tensor_types =
    std::tuple<type::tensor<double>, type::tensor<float>,
               type::tensor_of_tensors<double>, type::tensor_of_tensors<float>>;

TEMPLATE_LIST_TEST_CASE("LabeledVariant", "", tensor_types) {
    using input  = std::variant<TestType>;
    using type   = typename detail_::LabeledVariant<input>::type;
    using traits = TensorTraits<TestType>;
    using corr   = std::variant<typename traits::labeled_tensor_type>;
    STATIC_REQUIRE(std::is_same_v<type, corr>);
}

TEMPLATE_LIST_TEST_CASE("labeled_variant_t", "", tensor_types) {
    using input  = std::variant<TestType>;
    using type   = labeled_variant_t<input>;
    using traits = TensorTraits<TestType>;
    using corr   = std::variant<typename traits::labeled_tensor_type>;
    STATIC_REQUIRE(std::is_same_v<type, corr>);
}
