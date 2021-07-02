#pragma once
#include <tiledarray.h>
namespace libchemist::tensor {

/** @brief Primary template for determining the types associated with a tensor
 *         type.
 *
 *  The primary template is not defined. Specializations need to be defined for
 *  each tensor type that is going to work with the TensorWrapper layer.
 *  Specializations are expected to define:
 *  - tensor_type : A typedef of the tensor's type
 *  - is_tot : a constexpr bool indicating whether the type is for a tensor-of-
 *             tensors, or not
 *  - labeled_tensor_t : the type that results from labeling a tensor instance
 *                       of type tensor_type
 *
 *  @tparam TensorType the type that this traits class describes.
 */
template<typename TensorType>
struct TensorTraits;

template<typename TileType, typename PolicyType>
struct TensorTraits<TA::DistArray<TileType, PolicyType>> {
public:
    /// Type of the tensor
    using tensor_type = TA::DistArray<TileType, PolicyType>;

    /** @brief Determines if @p tensor_type is the type for a tensor-of-tensors
     *
     *  This type trait determines if @p tensor_type is a tensor-of-tensors. It
     *  simply wraps the call to the respective type trait in TiledArray.
     */
    static constexpr bool is_tot = TA::detail::is_tensor_of_tensor_v<TileType>;

    /** @brief Type resulting from labeling/annotating a tensor with string
     *         indices.
     *
     *  This type trait is used to get the type resulting from
     *  labeling/annotating the tensor with string indices.
     *
     */
    using labeled_tensor_type = decltype(std::declval<tensor_type>()("i,j"));
}; // TensorTraits

template<typename TileType, typename PolicyType>
struct TensorTraits<const TA::DistArray<TileType, PolicyType>> {
public:
    /// Type of the tensor
    using tensor_type = const TA::DistArray<TileType, PolicyType>;

    /** @brief Determines if @p tensor_type is the type for a tensor-of-tensors
     *
     *  This type trait determines if @p tensor_type is a tensor-of-tensors. It
     *  simply wraps the call to the respective type trait in TiledArray.
     */
    static constexpr bool is_tot = TA::detail::is_tensor_of_tensor_v<TileType>;

    /** @brief Type resulting from labeling/annotating a tensor with string
     *         indices.
     *
     *  This type trait is used to get the type resulting from
     *  labeling/annotating the tensor with string indices.
     *
     */
    using labeled_tensor_type = decltype(std::declval<tensor_type>()("i,j"));
}; // TensorTraits

} // namespace libchemist::tensor
