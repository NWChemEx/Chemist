/*
 * Copyright 2024 NWChemEx-Project
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
#include <chemist/traits/point_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {

template<typename PointSetType>
class PointSetView
  : public utilities::IndexableContainerBase<PointSetView<PointSetType>> {
private:
    /// Type of *this
    using my_type = PointSetView<PointSetType>;

    /// Type this class derives from
    using base_type =
      utilities::IndexableContainerBase<PointSetView<PointSetType>>;

    /// Traits associated with PointSetType
    using traits_type = ChemistClassTraits<PointSetType>;

    /// Traits associated with the Points in *this
    using point_traits_type = typename traits_type::point_traits;

public:
    /// Type *this is behaving like
    using point_set_type = typename traits_type::value_type;

    /// Type of an element in *this
    using value_type = typename point_traits_type::value_type;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Makes a view of an empty PointSet object.
     *
     *  @throw None No throw guarantee
     */
    PointSetView() noexcept;
};

} // namespace chemist
