#include "libchemist/tensor/type_traits/type_traits.hpp"
#include "libchemist/tensor/types.hpp"
#include "libchemist/types.hpp"
#include <catch2/catch.hpp>

using namespace libchemist::tensor;
using TensorD = libchemist::tensor::type::detail_::tensor<double>;
using TensorF = libchemist::tensor::type::detail_::tensor<float>;
using ToTD    = libchemist::tensor::type::detail_::tensor_of_tensors<double>;
using ToTF    = libchemist::tensor::type::detail_::tensor_of_tensors<float>;

template<typename T>
using labeled_tensor_t = typename TensorTraits<T>::labeled_tensor_type;

TEMPLATE_LIST_TEST_CASE("add_expr_t", "", type::tensor_variant) {
    using lhs_type = labeled_tensor_t<TestType>;
    SECTION("RHS is tensor") {
        using rhs_type = labeled_tensor_t<TensorD>;
        using type     = add_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() + std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
    SECTION("RHS is tensor of tensors") {
        using rhs_type = labeled_tensor_t<ToTD>;
        using type     = add_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() + std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
}

TEMPLATE_LIST_TEST_CASE("subt_expr_t", "", type::tensor_variant) {
    using lhs_type = labeled_tensor_t<TestType>;
    SECTION("RHS is tensor") {
        using rhs_type = labeled_tensor_t<TensorD>;
        using type     = subt_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() - std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
    SECTION("RHS is tensor of tensors") {
        using rhs_type = labeled_tensor_t<ToTD>;
        using type     = subt_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() - std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
}

TEMPLATE_LIST_TEST_CASE("mult_expr_t", "", type::tensor_variant) {
    using lhs_type = labeled_tensor_t<TestType>;
    SECTION("RHS is tensor") {
        using rhs_type = labeled_tensor_t<TensorD>;
        using type     = mult_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() * std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
    SECTION("RHS is tensor of tensors") {
        using rhs_type = labeled_tensor_t<ToTD>;
        using type     = mult_expr_t<lhs_type, rhs_type>;
        using corr =
          decltype(std::declval<lhs_type>() * std::declval<rhs_type>());
        STATIC_REQUIRE(std::is_same_v<type, corr>);
    }
}

TEST_CASE("add_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t1        = labeled_tensor_t<ToTD>;
    using t_variant = std::variant<t0, t1>;
    using t00       = add_expr_t<t0, t0>;
    using t01       = add_expr_t<t0, t1>;
    using t10       = add_expr_t<t1, t0>;
    using t11       = add_expr_t<t1, t1>;
    using corr_type = std::variant<t00, t01, t10, t11>;
    using type      = add_variant_t<t_variant, t_variant>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

TEST_CASE("subt_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t1        = labeled_tensor_t<ToTF>;
    using t_variant = std::variant<t0, t1>;
    using t00       = subt_expr_t<t0, t0>;
    using t01       = subt_expr_t<t0, t1>;
    using t10       = subt_expr_t<t1, t0>;
    using t11       = subt_expr_t<t1, t1>;
    using corr_type = std::variant<t00, t01, t10, t11>;
    using type      = subt_variant_t<t_variant, t_variant>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}

TEST_CASE("mult_variant_t") {
    using t0        = labeled_tensor_t<TensorD>;
    using t1        = labeled_tensor_t<ToTF>;
    using t_variant = std::variant<t0, t1>;
    using t00       = mult_expr_t<t0, t0>;
    using t01       = mult_expr_t<t0, t1>;
    using t10       = mult_expr_t<t1, t0>;
    using t11       = mult_expr_t<t1, t1>;
    using corr_type = std::variant<t00, t01, t10, t11>;
    using type      = mult_variant_t<t_variant, t_variant>;
    STATIC_REQUIRE(std::is_same_v<type, corr_type>);
}
