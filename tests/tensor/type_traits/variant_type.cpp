#include "../test_tensor.hpp"
#include "chemist/tensor/type_traits/variant_type.hpp"

using namespace chemist::tensor;

using field_types = std::tuple<field::Scalar, field::Tensor>;

TEMPLATE_LIST_TEST_CASE("VariantType", "", field_types) {
    using traits_type  = backends::TiledArrayTraits<TestType>;
    using corr_type    = typename traits_type::variant_type;
    using wrapper_type = TensorWrapper<TestType>;
    using variant_type = typename detail_::VariantType<wrapper_type>::type;

    STATIC_REQUIRE(std::is_same_v<variant_type, corr_type>);
}

TEMPLATE_LIST_TEST_CASE("variant_type_t", "", field_types) {
    using traits_type  = backends::TiledArrayTraits<TestType>;
    using corr_type    = typename traits_type::variant_type;
    using wrapper_type = TensorWrapper<TestType>;
    using variant_type = variant_type_t<wrapper_type>;

    STATIC_REQUIRE(std::is_same_v<variant_type, corr_type>);
}
