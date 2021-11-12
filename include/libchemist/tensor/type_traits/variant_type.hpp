#pragma once
#include "libchemist/tensor/type_traits/field_traits.hpp"

namespace libchemist::tensor {

/// Forward declare TensorWrapper
template<typename VariantType>
class TensorWrapper;

namespace detail_ {

/** @brief Primary template for determining the variant type that a
 *         TensorWrapper is templated on.
 *
 *  The primary template is not defined, leading to a compilation error when
 *  @p T is not a specialization of `TensorWrapper`. A subsequent specialization
 *  of VariantType handles the case when @p T is a specialization of
 *  `TensorWrapper`.
 *
 *  @tparam T The TensorWrapper specialization we are inspecting.
 */
template<typename T>
struct VariantType;

/** @brief Partial specialization of VariantType for when the template type
 *         parameter is a TensorWrapper.
 *
 *  This specialization has a member `type` set to the variant type of the
 *  provided `TensorWrapper` type.
 *
 *  @tparam T The variant type for the `TensorWrapper` specialization.
 */
template<typename T>
struct VariantType<TensorWrapper<T>> {
    /// Typedef of the variant type in the TensorWrapper
    using type = typename FieldTraits<T>::variant_type;
};
} // namespace detail_

/** @brief Public API for determining the variant type of a TensorWrapper.
 *
 *  This typedef is used to get the variant type of a TensorWrapper
 *  specialization. This typedef will fail to compile if @p T is not a
 *  specialization of TensorWrapper.
 *
 *  @tparam T A non-qualified TensorWrapper type.
 */
template<typename T>
using variant_type_t = typename detail_::VariantType<T>::type;

} // namespace libchemist::tensor
