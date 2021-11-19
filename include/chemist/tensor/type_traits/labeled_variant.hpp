#pragma once
#include "chemist/tensor/type_traits/tensor_traits.hpp"
#include <variant>

namespace chemist::tensor {
namespace detail_ {

/** @brief Primary template for deducing the type of a std::variant resulting
 *         from labeling/annotating the tensors in the input variant type.
 *
 *  The primary template is chosen when @p T is not an std::variant and will
 *  lead to a compilation error because it is not defined.
 *
 *  @param T When this template is selected @p T will be a type other than
 *           std::variant.
 */
template<typename T>
struct LabeledVariant;

/** @brief Specializes LabeledVariant for std::variant.
 *
 *  This specialization determines the type that results from
 *  labeling/annotating every tensor in the input variant. The resulting variant
 *  type is stored as the member type `type`.
 *
 *  @tparam Args The types in the std::variant. The types are assumed to be
 *               TA::DistArray instantiations.
 */
template<typename... Args>
struct LabeledVariant<std::variant<Args...>> {
    /// Wraps the call to TensorTraits to get the type of the labeled tensor
    template<typename T>
    using labeled_tensor_type = typename TensorTraits<T>::labeled_tensor_type;
    /// Type of a tuple resulting from labeling all the types in @p Args
    using type = std::variant<labeled_tensor_type<Args>...>;
};

} // namespace detail_

/** @brief Public API for getting a variant of labeled tensors
 *
 *  This is the public API for turning a variant of tensors into a variant of
 *  labeled tensors. For the i-th type in @p TensorVariant the i-th type in the
 *  resulting variant will be the type which results from calling
 *  `operator()(std::string)` on a tensor of that type.
 *
 *  @tparam TensorVariant A variant of tensors.
 */
template<typename TensorVariant>
using labeled_variant_t = typename detail_::LabeledVariant<TensorVariant>::type;

} // namespace chemist::tensor
