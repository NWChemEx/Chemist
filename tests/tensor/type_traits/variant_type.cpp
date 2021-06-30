#include "libchemist/tensor/type_traits/variant_type.hpp"
#include "../test_tensor.hpp"

using namespace libchemist::tensor;

TEMPLATE_LIST_TEST_CASE("VariantType", "", testing::all_tensor_variant_t) {
    using corr_type = TestType;
    using wrapper_type = TensorWrapper<corr_type>;
    using variant_type = typename detail_::VariantType<wrapper_type>::type;

    STATIC_REQUIRE(std::is_same_v<variant_type, corr_type>);
}

TEMPLATE_LIST_TEST_CASE("variant_type_t", "", testing::all_tensor_variant_t) {
    using corr_type = TestType;
    using wrapper_type = TensorWrapper<corr_type>;
    using variant_type = variant_type_t<wrapper_type>;

    STATIC_REQUIRE(std::is_same_v<variant_type, corr_type>);
}
