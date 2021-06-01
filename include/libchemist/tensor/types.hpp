#pragma once
#include <tiledarray.h>
#include <variant>

namespace libchemist::tensor {
namespace detail_ {

/// Type of a tile in a tensor of tensors
template<typename T>
using tot_tile_t = TA::Tensor<TA::Tensor<T>>;

} // namespace detail_

/// Type of a tensor which is block-sparse
template<typename T>
using block_sparse_t = TA::DistArray<T, TA::SparsePolicy>;

/// Type of the basic, non-hierarchal, block-sparse tensor
template<typename T>
using tensor_t = block_sparse_t<TA::Tensor<T>>;

/// Typedef of the tensor of tensor class
template<typename T>
using tensor_of_tensors_t = block_sparse_t<detail_::tot_tile_t<T>>;

/// Type of a variant with all possible tensor types in it
using tensor_variant_t =
  std::variant<tensor_t<double>, tensor_of_tensors_t<double>, tensor_t<float>,
               tensor_of_tensors_t<float>>;

} // namespace libchemist::tensor