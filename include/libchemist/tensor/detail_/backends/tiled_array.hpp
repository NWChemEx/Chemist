#pragma once
#include "libchemist/tensor/fields.hpp"
#include <tiledarray.h>
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

namespace libchemist::tensor::tiled_array {

template<typename T>
struct TiledArrayTraits;

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

} // namespace libchemist::tensor::tiled_array
