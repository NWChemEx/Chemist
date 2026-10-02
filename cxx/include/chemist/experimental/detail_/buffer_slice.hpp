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

#pragma once
#include <cstddef>
#include <wtf/wtf.hpp>

namespace chemist::experimental::detail_ {

/** @brief Returns a view of the elements [@p begin, @p begin + @p n) of
 *         @p buffer.
 *
 *  wtf::buffer::BufferView has no notion of a sub-range, but it can alias any
 *  contiguous run of a concrete floating-point type. This visits @p buffer to
 *  recover the concrete type, and wraps the requested run of it in a new view.
 *  Containers which store the parameters of several objects back to back, e.g.
 *  AtomicBasisSet, use this to hand out a view of one object's parameters.
 *
 *  The result aliases the same memory as @p buffer, so writes through either
 *  are visible through the other, and it is invalidated by anything which
 *  reallocates that memory.
 *
 *  @param[in] buffer The buffer to slice. Must be contiguous.
 *  @param[in] begin The offset of the first element of the slice.
 *  @param[in] n The number of elements in the slice.
 *
 *  @return A view of the requested elements. When @p n is zero this is a
 *          default-constructed (null) view, which behaves like an empty
 *          buffer.
 *
 *  @throw std::out_of_range if [@p begin, @p begin + @p n) is not contained in
 *                           [0, buffer.size()). Strong throw guarantee.
 *  @throw std::bad_alloc if there is a problem allocating the view. Strong
 *                        throw guarantee.
 */
///@{
wtf::buffer::BufferView<wtf::fp::Float> slice_buffer(
  wtf::buffer::BufferView<wtf::fp::Float> buffer, std::size_t begin,
  std::size_t n);

wtf::buffer::BufferView<const wtf::fp::Float> slice_buffer(
  wtf::buffer::BufferView<const wtf::fp::Float> buffer, std::size_t begin,
  std::size_t n);
///@}

} // namespace chemist::experimental::detail_
