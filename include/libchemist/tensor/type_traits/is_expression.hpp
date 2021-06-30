#pragma once
#include <type_traits>

namespace libchemist::tensor {

namespace detail_ {

// Forward declare OpLayer for template meta-programming purposes
template<typename T>
class OpLayer;

} // namespace detail_

/** @brief Compile-time constant indicating whether a type @p T is part of the
 *         expressions layer of the tensor wrapper.
 *
 *  All types which are part of the expression layer are assumed to inherit
 *  from OpLayer via the CRTP. This constant indicates whether @p T inherits
 *  from OpLayer and is thus part of the expression layer. This constant will be
 *  set to true if @p T derives from OpLayer and false otherwise.
 *
 *  @tparam T We are determining whether @p T is part of the expression layer.
 */
template<typename T>
static constexpr bool is_expression_v =
    std::is_base_of_v<detail_::OpLayer<T>, T>;

/** @brief Used to enable a function via SFINAE when @p T is part of the
 *         expression layer.
 *
 *  @tparam T If @p T is part of the expression layer the decorated function
 *            will participate in overload resolution otherwise it will not.
 *  @tparam U The type that the typedef will be set to if @p T is part of the
 *            expression layer.
 */
template<typename T, typename U = void>
using enable_if_expression_t = std::enable_if_t<is_expression_v<T>, U>;

} // namespace libchemist::tensor
