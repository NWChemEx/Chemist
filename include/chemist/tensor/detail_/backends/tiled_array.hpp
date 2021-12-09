#pragma once
#include "chemist/ta_helpers/ta_headers.hpp"
#include "chemist/tensor/fields.hpp"
#include <variant>

/** @file tiled_array.hpp
 *
 *  @brief Contains traits for when TiledArray is used as the backend.
 *
 *  These traits classes basically exist to attempt to isolate the TensorWrapper
 *  library from the guts of TA (to an extent). What really matters in all of
 *  these traits types is the `variant_type` member as that's what is used by
 *  the TensorWrapper library.
 */

namespace chemist::tensor::backends {

/** @brief Primary template for establishing the types associated with
 *         TiledArray.
 *
 *  The primary template is not defined. Instead TiledArrayTraits is specialized
 *  for each field, establishing a mapping from a field to the types Tiled Array
 *  uses for that field.
 *
 *  @tparam T The type of the field the tensor is over.
 *
 */
template<typename T>
struct TiledArrayTraits;

/** @brief Specializes TiledArrayTraits for tensors which have scalar elements.
 *
 *  This specialization is selected when the tensor uses `field::Scalar`. The
 *  elements of such a tensor are floating point values.
 */
template<>
struct TiledArrayTraits<field::Scalar> {
    /// Typedef of the tile for a tensor of scalars
    template<typename T>
    using tensor_tile_type = TA::Tensor<T>;

    /// Typedef of the tensor class
    template<typename T>
    using tensor_type = TA::DistArray<tensor_tile_type<T>, TA::SparsePolicy>;

    /// Type of a variant with all possible non-hierarchal tensor types in it
    using variant_type = std::variant<tensor_type<double>>;
};

/** @brief Specializes TiledArrayTraits for tensors which have tensor elements.
 *
 *  This specialization is selected when the tensor uses `field::Tensor`. The
 *  elements of such a tensor are other tensors.
 */
template<>
struct TiledArrayTraits<field::Tensor> {
    /// Typedef of the tiles in a tensor-of-tensors
    template<typename T>
    using tensor_tile_type = TA::Tensor<TA::Tensor<T>>;

    /// Typedef of the tensor-of-tensors class
    template<typename T>
    using tensor_type = TA::DistArray<tensor_tile_type<T>, TA::SparsePolicy>;

    /// Type of a variant with all possible hierarchal tensor_types in it
    using variant_type = std::variant<tensor_type<double>>;
};

} // namespace chemist::tensor::backends
