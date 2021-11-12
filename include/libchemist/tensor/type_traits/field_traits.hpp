#pragma once
#include "libchemist/tensor/detail_/backends/tiled_array.hpp"
#include "libchemist/tensor/type_traits/labeled_variant.hpp"
#include <utilities/type_traits/variant/add_const.hpp>
#include <utilities/type_traits/variant/has_type.hpp>

namespace libchemist::tensor::detail_ {

template<typename FieldType>
struct FieldTraits {
private:
    using traits_type = typename backends::TiledArrayTraits<FieldType>;

    template<typename T>
    using add_const_t = utilities::type_traits::variant::add_const_t<T>;

    template<typename T, typename U>
    static constexpr bool has_type_v =
      utilities::type_traits::variant::has_type_v<T, U>;

public:
    using variant_type = typename traits_type::variant_type;

    using const_variant_type = add_const_t<variant_type>;

    using labeled_variant_type = labeled_variant_t<variant_type>;

    using const_labeled_variant_type = labeled_variant_t<const_variant_type>;

    /// Helper type that TensorWrapper can use to determine if the PIMPL can
    /// hold a tensor of type @p T.
    template<typename T>
    static constexpr bool is_tensor_type_v = has_type_v<T, variant_type>;
};

} // namespace libchemist::tensor::detail_
