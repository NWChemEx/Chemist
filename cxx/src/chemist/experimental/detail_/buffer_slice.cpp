/*
 * Copyright 2026 NWChemEx-Project
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

#include <chemist/experimental/detail_/buffer_slice.hpp>
#include <chemist/types/floating_point.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace chemist::experimental::detail_ {
namespace {

/// The concrete types a buffer may hold; see PointSet::fp_types
using fp_types = chemist::types::floating_point_types;

/// Throws std::out_of_range if [begin, begin + n) is not in [0, size)
void check_range(std::size_t size, std::size_t begin, std::size_t n) {
    if(begin <= size && n <= size - begin) return;
    throw std::out_of_range(
      "chemist::experimental::detail_::slice_buffer: the range [" +
      std::to_string(begin) + ", " + std::to_string(begin + n) +
      ") is not contained in a buffer of size " + std::to_string(size) + ".");
}

/// Implements both overloads of slice_buffer
template<typename FloatType>
wtf::buffer::BufferView<FloatType> slice_buffer_(
  wtf::buffer::BufferView<FloatType> buffer, std::size_t begin, std::size_t n) {
    check_range(buffer.size(), begin, n);
    // A null view can not be visited, and an empty slice of a non-null one
    // would need a pointer which may be one-past-the-end.
    if(n == 0) return {};
    // wtf::buffer::visit_contiguous_buffer_view instantiates the visitor for
    // read-only spans even when visiting a mutable view, so it has to compile
    // for them; only the matching const-qualification is reachable.
    auto visitor =
      [begin, n]<typename T>(
        std::span<T> values) -> wtf::buffer::BufferView<FloatType> {
        if constexpr(std::is_const_v<T> && !std::is_const_v<FloatType>) {
            throw std::logic_error(
              "chemist::experimental::detail_::slice_buffer: a mutable buffer "
              "was visited as a read-only one.");
        } else {
            return wtf::buffer::BufferView<FloatType>(values.data() + begin, n);
        }
    };
    return wtf::buffer::visit_contiguous_buffer_view<fp_types>(visitor, buffer);
}

} // namespace

wtf::buffer::BufferView<wtf::fp::Float> slice_buffer(
  wtf::buffer::BufferView<wtf::fp::Float> buffer, std::size_t begin,
  std::size_t n) {
    return slice_buffer_(std::move(buffer), begin, n);
}

wtf::buffer::BufferView<const wtf::fp::Float> slice_buffer(
  wtf::buffer::BufferView<const wtf::fp::Float> buffer, std::size_t begin,
  std::size_t n) {
    return slice_buffer_(std::move(buffer), begin, n);
}

} // namespace chemist::experimental::detail_
