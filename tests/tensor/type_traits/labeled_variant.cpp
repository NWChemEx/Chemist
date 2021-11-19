#include "chemist/tensor/type_traits/labeled_variant.hpp"
#include "chemist/types.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using namespace chemist::tensor;

using scalar_traits = backends::TiledArrayTraits<field::Scalar>;
using tot_traits    = backends::TiledArrayTraits<field::Tensor>;

using tensor_types = std::tuple<typename scalar_traits::tensor_type<double>,
                                typename scalar_traits::tensor_type<float>,
                                typename tot_traits::tensor_type<double>,
                                typename tot_traits::tensor_type<float>>;

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
