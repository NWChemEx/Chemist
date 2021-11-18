#pragma once
#include "libchemist/tensor/detail_/backends/tiled_array.hpp"
#include "libchemist/tensor/type_traits/labeled_variant.hpp"
#include <utilities/type_traits/variant/add_const.hpp>
#include <utilities/type_traits/variant/has_type.hpp>

namespace libchemist::tensor::detail_ {

/** @brief Class that connects the Field with the backend's types.
 *
 *  The point of this class is make the connection from the field choice to
 *  the backend choice. Basically each backend defines the types needed for a
 *  specific field, and then this class wraps the choice of backend.
 *
 *  @tparam FieldType Type of the field the tensor's components come from.
 *                    Assumed to be a class in the `field` namespace.
 *
 */
template<typename FieldType>
struct FieldTraits {
private:
    /// The backend's traits type
    using traits_type = typename backends::TiledArrayTraits<FieldType>;

    /** @brief Applies `const` to each type in a variant of type @p T
     *
     *  This typedef exists to help fit the const_variant_type typedef on a
     *  single line.
     *
     *  @tparam T Assumed to be a specialization of std::variant
     */
    template<typename T>
    using add_const_t = utilities::type_traits::variant::add_const_t<T>;

    /** @brief True if the type @p T is in the variant of type @p U.
     *
     *  Like add_const_t, this typedef's sole purpose is to help get
     *  is_tensor_type_v to fit on one line. If @p T is in the variant of type
     *  @p U this member will be true, otherwise it will be false.
     *
     *  @tparam T The type we will search @p U for.
     *  @tparam U The variant we are looking for @p T in.
     *
     */
    template<typename T, typename U>
    static constexpr bool has_type_v =
      utilities::type_traits::variant::has_type_v<T, U>;

public:
    /// type of the variant the backend uses
    using variant_type = typename traits_type::variant_type;

    /// A read-only version of the variant that the backend uses
    using const_variant_type = add_const_t<variant_type>;

    /// A variant which results from labeling the wrapped tensor
    using labeled_variant_type = labeled_variant_t<variant_type>;

    /// A variant which results from labeling a read-only tensor
    using const_labeled_variant_type = labeled_variant_t<const_variant_type>;

    /// Helper type that TensorWrapper can use to determine if the PIMPL can
    /// hold a tensor of type @p T.
    template<typename T>
    static constexpr bool is_tensor_type_v = has_type_v<T, variant_type>;
};

} // namespace libchemist::tensor::detail_
