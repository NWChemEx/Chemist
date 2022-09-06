/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <tensorwrapper/sparse_map/sparse_map.hpp>
#include <tensorwrapper/tensor/tensor.hpp>

/** @file types.hpp
 *
 * @brief Typedefs of the fundamental types used in chemist.
 *
 * The typedefs in this file are the default types used throughout LibChemist.
 * For example, `chemist::type::tensor<double>` is the default type of a
 * tensor whose elements are of type `double` and `chemist::type::size` is
 * the default type used for indexing and offsets. Components of LibChemist can
 * override the defaults if they need to, but should otherwise just use these
 * typedefs. Types that are primarily used within a component of LibChemist
 * should be defined in that component's `types.hpp` file.
 */
namespace chemist::type {

/// Type used for offsets and indexing
using size = std::size_t;

/// Type used for non-hierarchical tensors
using tensor = tensorwrapper::tensor::ScalarTensorWrapper;

/// Type used for hierarchical (i.e. ToTs) tensors
using tensor_of_tensors = tensorwrapper::tensor::TensorOfTensorsWrapper;

/** @brief Type of the sparse affiliated with dependent spaces.
 *
 *  Sparse maps are stored from elements to elements. Since tiling can vary from
 *  tensor to tensor this allows us to compute the tile to tile sparse map per
 *  tensor.
 */
using sparse_map = tensorwrapper::sparse_map::SparseMap;

} // namespace chemist::type
