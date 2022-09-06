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
#include <functional>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::detail_ {

template<typename T, typename ConstT>
class FlattenedView
  : public utilities::IndexableContainerBase<FlattenedView<T, ConstT>> {
private:
    using my_type   = FlattenedView<T, ConstT>;
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    using value_type = typename T::aliased_type;
    /// Unsigned integral type used for offsets/indexing
    using size_type = typename base_type::size_type;

    template<typename U, typename V, typename W>
    FlattenedView(U&& size_fxn, V&& at_fxn, W&& const_at_fxn) noexcept;

private:
    friend base_type;
    size_type size_() const noexcept { return m_size_(); }
    T at_(size_type i) { return m_at_(i); }
    ConstT at_(size_type i) const { return m_const_at_(i); }

    std::function<size_type()> m_size_;
    std::function<T(size_type)> m_at_;
    std::function<ConstT(size_type)> m_const_at_;
};

template<typename T, typename ConstT>
template<typename U, typename V, typename W>
FlattenedView<T, ConstT>::FlattenedView(U&& size_fxn, V&& at_fxn,
                                        W&& const_at_fxn) noexcept :
  m_size_(std::forward<U>(size_fxn)),
  m_at_(std::forward<V>(at_fxn)),
  m_const_at_(std::forward<W>(const_at_fxn)) {}

} // namespace chemist::detail_
