#pragma once
#include "chemist/tensor/allocators/allocator.hpp"
#include "chemist/tensor/allocators/one_big_tile.hpp"
#include "chemist/tensor/allocators/single_element_tiles.hpp"

namespace chemist::tensor {

/** @brief Wraps the process of creating a defaulted allocator
 *
 *  As more advanced alloctors are added to the library it will make sense to
 *  change the default TensorWrapper allocator. To decouple code from this
 *  choice, classes and functions needing a default_allocator instance are
 *  encouraged to get that allocator from this function.
 *
 *  @tparam FieldType The type of the field the tensor is over. Assumed to be
 *                    either field::Scalar or field::Tensor.
 *
 *  @return a type-erased, allocator.
 *
 *  @throw std::bad_alloc if allocation fails. Strong throw guarantee.
 */
template<typename FieldType>
typename Allocator<FieldType>::allocator_ptr default_allocator() {
    return std::make_unique<OneBigTile<FieldType>>();
}

} // namespace chemist::tensor
