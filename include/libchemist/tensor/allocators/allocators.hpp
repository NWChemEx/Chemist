#pragma once
#include "libchemist/tensor/allocators/allocator.hpp"
#include "libchemist/tensor/allocators/one_big_tile.hpp"
#include "libchemist/tensor/allocators/single_element_tiles.hpp"

namespace libchemist::tensor {

/** @brief Wraps the process of creating a defaulted allocator
 *
 *  As more advanced alloctors are added to the library it will make sense to
 *  change the default TensorWrapper allocator. To decouple code from this
 *  choice, classes and functions needing a default_allocator instance are
 *  encouraged to get that allocator from this function.
 *
 *  @return a type-erased, allocator.
 *
 *  @throw std::bad_alloc if allocation fails. Strong throw guarantee.
 */
template<typename VariantType>
typename Allocator<VariantType>::allocator_ptr default_allocator() {
    return std::make_unique<OneBigTile<VariantType>>();
}

} // namespace libchemist::tensor
