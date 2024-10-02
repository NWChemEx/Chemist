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

#include "detail_/point_set_contiguous.hpp"
#include <utility>

namespace chemist {

#define TPARAMS template<typename PointSetType>
#define POINT_SET_VIEW PointSetView<PointSetType>

// -----------------------------------------------------------------------------
// -- Ctors, Assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
POINT_SET_VIEW::PointSetView() noexcept = default;

TPARAMS
POINT_SET_VIEW::PointSetView(point_set_reference ps) :
  PointSetView(ps.size(), ps.x_data(), ps.y_data(), ps.z_data()) {}

TPARAMS
POINT_SET_VIEW::PointSetView(size_type n_points, coord_pointer px,
                             coord_pointer py, coord_pointer pz) :
  m_pimpl_(std::make_unique<detail_::PointSetContiguous<PointSetType>>(
    n_points, px, py, pz)) {}

TPARAMS
POINT_SET_VIEW::PointSetView(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

TPARAMS
POINT_SET_VIEW::PointSetView(const PointSetView& other) :
  m_pimpl_(other.clone_pimpl_()) {}

TPARAMS
POINT_SET_VIEW& POINT_SET_VIEW::operator=(const PointSetView& rhs) {
    if(this != &rhs) rhs.clone_pimpl_().swap(m_pimpl_);
    return *this;
}

TPARAMS
POINT_SET_VIEW::PointSetView(PointSetView&& other) noexcept = default;

TPARAMS
POINT_SET_VIEW& POINT_SET_VIEW::operator=(PointSetView&& rhs) noexcept =
  default;

TPARAMS
POINT_SET_VIEW::~PointSetView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
void POINT_SET_VIEW::swap(PointSetView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool POINT_SET_VIEW::empty() const noexcept { return size_() == 0; }

TPARAMS
bool POINT_SET_VIEW::operator==(const PointSetView& rhs) const noexcept {
    if(empty() != rhs.empty()) return false;
    if(empty()) return true;
    return m_pimpl_->are_equal(*rhs.m_pimpl_);
}

TPARAMS
bool POINT_SET_VIEW::operator!=(const PointSetView& rhs) const noexcept {
    return !(*this == rhs);
}

TPARAMS
typename POINT_SET_VIEW::point_set_type POINT_SET_VIEW::as_points() const {
    point_set_type rv;
    for(const auto& x : *this) rv.push_back(x.as_point());
    return rv;
}

// -----------------------------------------------------------------------------
// -- Private member functions
// -----------------------------------------------------------------------------

TPARAMS bool POINT_SET_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
typename POINT_SET_VIEW::pimpl_pointer POINT_SET_VIEW::clone_pimpl_() const {
    return has_pimpl_() ? m_pimpl_->clone() : pimpl_pointer{};
}

TPARAMS
typename POINT_SET_VIEW::reference POINT_SET_VIEW::at_(size_type i) {
    return m_pimpl_->operator[](i);
}

TPARAMS
typename POINT_SET_VIEW::const_reference POINT_SET_VIEW::at_(
  size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

TPARAMS
typename POINT_SET_VIEW::size_type POINT_SET_VIEW::size_() const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

#undef POINT_SET_VIEW
#undef TPARAMS

template class PointSetView<PointSet<float>>;
template class PointSetView<const PointSet<float>>;
template class PointSetView<PointSet<double>>;
template class PointSetView<const PointSet<double>>;

} // namespace chemist
