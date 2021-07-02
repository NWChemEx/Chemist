#include "libchemist/tensor/type_traits/tensor_traits.hpp"
#include "libchemist/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist;
using namespace libchemist::tensor;

using tensor_types =
  std::tuple<type::tensor<double>, type::tensor<float>,
             type::tensor_of_tensors<double>, type::tensor_of_tensors<float>>;

TEMPLATE_LIST_TEST_CASE("TA TensorTraits", "", tensor_types) {
    using tensor_type = TestType;
    using traits_type = TensorTraits<tensor_type>;

    SECTION("tensor_type") {
        using type = typename traits_type::tensor_type;
        STATIC_REQUIRE(std::is_same_v<type, tensor_type>);
    }

    SECTION("is_tot") {
        using tile_type     = typename tensor_type::value_type;
        constexpr bool corr = TA::detail::is_tensor_of_tensor_v<tile_type>;
        STATIC_REQUIRE(corr == traits_type::is_tot);
    }

    SECTION("labeled_tensor_type") {
        using corr = decltype(std::declval<tensor_type>()("i,j"));
        using type = typename traits_type::labeled_tensor_type;
        STATIC_REQUIRE(std::is_same_v<corr, type>);
    }
}

TEMPLATE_LIST_TEST_CASE("const TA TensorTraits", "", tensor_types) {
    using tensor_type = TestType;
    using traits_type = TensorTraits<const tensor_type>;

    SECTION("tensor_type") {
        using type = typename traits_type::tensor_type;
        STATIC_REQUIRE(std::is_same_v<type, const tensor_type>);
    }

    SECTION("is_tot") {
        using tile_type     = typename tensor_type::value_type;
        constexpr bool corr = TA::detail::is_tensor_of_tensor_v<tile_type>;
        STATIC_REQUIRE(corr == traits_type::is_tot);
    }

    SECTION("labeled_tensor_type") {
        using corr = decltype(std::declval<const tensor_type>()("i,j"));
        using type = typename traits_type::labeled_tensor_type;
        STATIC_REQUIRE(std::is_same_v<corr, type>);
    }
}