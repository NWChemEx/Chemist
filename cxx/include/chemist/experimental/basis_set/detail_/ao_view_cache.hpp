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
#include <vector>

namespace chemist::experimental::detail_ {

/** @brief Holds the AO views a shell hands out.
 *
 *  A shell hands out its AOs by reference, so it has to store them somewhere.
 *  Each stored view aliases the shell's contracted Gaussian, which makes the
 *  views part of the shell's *derived* state: they are only valid for the
 *  exact object, and the exact angular momentum, they were built for. *this
 *  encodes that:
 *
 *  - Copying or moving *this gives an empty cache, and assigning to *this
 *    empties it. Moving from a cache empties the source too, since its views
 *    alias state which is being moved out. A copied shell therefore never
 *    hands out views of its source, and the shell's own copy/move operations
 *    can stay defaulted (and moves no-throw).
 *  - The views are built on demand, by get, and are rebuilt whenever the
 *    angular momentum they were built for is not the current one. This catches
 *    changes to @f$\ell@f$ made through the contracted Gaussian directly, which
 *    the shell has no way of observing.
 *  - Anything else which rebinds the contracted Gaussian (swap, load) must
 *    call clear.
 *
 *  @note get populates the cache from a const method. As with any lazily
 *        populated cache, populating it is not safe to do from several threads
 *        at once; once populated, concurrent reads are fine.
 *
 *  @tparam ViewType The type of the AO views being stored.
 */
template<typename ViewType>
class AOViewCache {
public:
    /// Type used for sizes and offsets
    using size_type = std::size_t;

    /// Creates an empty cache
    AOViewCache() noexcept = default;

    /// Copying yields an empty cache; see the description of *this.
    AOViewCache(const AOViewCache&) noexcept {}

    /// Moving yields an empty cache, and empties @p other; see the
    /// description of *this.
    AOViewCache(AOViewCache&& other) noexcept { other.clear(); }

    /// Assigning empties the cache; see the description of *this.
    AOViewCache& operator=(const AOViewCache&) noexcept {
        clear();
        return *this;
    }

    /// Assigning empties the cache, and @p other; see the description of
    /// *this.
    AOViewCache& operator=(AOViewCache&& other) noexcept {
        clear();
        other.clear();
        return *this;
    }

    /// Default no-throw dtor
    ~AOViewCache() noexcept = default;

    /** @brief Returns the views, building them first if needed.
     *
     *  @tparam MakeView The type of @p make_view.
     *
     *  @param[in] l The current angular momentum of the shell.
     *  @param[in] n The number of AOs in the shell.
     *  @param[in] make_view Called with each offset in [0, @p n), returns the
     *                       view at that offset.
     *
     *  @return The stored views, valid for @p l.
     *
     *  @throw std::bad_alloc if there is a problem allocating the views.
     *                        Strong throw guarantee.
     */
    template<typename MakeView>
    const std::vector<ViewType>& get(size_type l, size_type n,
                                     MakeView&& make_view) const {
        if(m_views_.empty() || m_l_ != l) {
            std::vector<ViewType> buffer;
            buffer.reserve(n);
            for(size_type i = 0; i < n; ++i) buffer.push_back(make_view(i));
            // swap rather than move-assign: not every view is assignable
            m_views_.swap(buffer);
            m_l_ = l;
        }
        return m_views_;
    }

    /// Forgets the stored views, so the next call to get rebuilds them
    void clear() noexcept { m_views_.clear(); }

private:
    /// The stored views. Empty means "not built", since no shell is empty.
    mutable std::vector<ViewType> m_views_;

    /// The angular momentum m_views_ was built for
    mutable size_type m_l_ = 0;
};

} // namespace chemist::experimental::detail_
