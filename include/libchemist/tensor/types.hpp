#pragma once
#include <tiledarray.h>
#include <utilities/type_traits/variant/cat.hpp>
#include <variant>

namespace libchemist::tensor {
template<typename VariantType>
class TensorWrapper;

namespace detail_ {

/// Type of a tile in a tensor of tensors
template<typename T>
using tot_tile_t = TA::Tensor<TA::Tensor<T>>;

} // namespace detail_

/// Base type of a TA tensor
template<typename TileType, typename PolicyType>
using ta_tensor_t = TA::DistArray<TileType, PolicyType>;

/// Type of a tensor which is block-sparse
template<typename T>
using block_sparse_t = ta_tensor_t<T, TA::SparsePolicy>;

/// Type of the basic, non-hierarchal, block-sparse tensor
template<typename T>
using tensor_t = block_sparse_t<TA::Tensor<T>>;

/// Typedef of the tensor of tensor class
template<typename T>
using tensor_of_tensors_t = block_sparse_t<detail_::tot_tile_t<T>>;

/// Type of a variant with all possible non-hierarchal tensor types in it
using tensor_variant_t = std::variant<tensor_t<double>>; //, tensor_t<float>>;

/// Type of a variant with all possible hierarchal tensor_types in it
using tot_variant_t =
  std::variant<tensor_of_tensors_t<double>>; //, tensor_of_tensors_t<float>>;

/// Type of a variant with all possible tensor types in it
using all_tensor_variant_t =
  utilities::type_traits::variant::cat_t<tensor_variant_t, tot_variant_t>;

using UniversalTensorWrapper = TensorWrapper<all_tensor_variant_t>;

using SparseTensorWrapper = TensorWrapper<tensor_variant_t>;

using ToTWrapper = TensorWrapper<tot_variant_t>;

} // namespace libchemist::tensor