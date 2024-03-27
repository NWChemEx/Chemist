/*
 * Copyright 2023 NWChemEx-Project
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

#include "detail_/point_set_pimpl.hpp"
#include <stdexcept>
#include <string>
#include <utility>

namespace chemist {

#define TEMPLATE_PARAMS template<typename PointType>
#define POINT_SET PointSet<PointType>

// -- CTors and DTor -----------------------------------------------------------

TEMPLATE_PARAMS
POINT_SET::PointSet() noexcept = default;

TEMPLATE_PARAMS
POINT_SET::PointSet(std::initializer_list<value_type> points) :
  PointSet(std::make_unique<pimpl_type>()) {
    for(const auto& x : points) m_pimpl_->push_back(x);
}

TEMPLATE_PARAMS
POINT_SET::PointSet(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

TEMPLATE_PARAMS
POINT_SET::PointSet(const PointSet& other) :
  PointSet(other.has_pimpl_() ? std::make_unique<pimpl_type>(*other.m_pimpl_) :
                                nullptr) {}

TEMPLATE_PARAMS
POINT_SET::PointSet(PointSet&& other) noexcept = default;

TEMPLATE_PARAMS
POINT_SET& POINT_SET::operator=(const PointSet& rhs) {
    PointSet(rhs).m_pimpl_.swap(m_pimpl_);
    return *this;
}

TEMPLATE_PARAMS
POINT_SET& POINT_SET::operator=(PointSet&& rhs) noexcept = default;

TEMPLATE_PARAMS POINT_SET::~PointSet() noexcept = default;

// -- Setters ------------------------------------------------------------------

TEMPLATE_PARAMS
void POINT_SET::push_back(value_type r) {
    if(!has_pimpl_()) m_pimpl_ = std::make_unique<pimpl_type>();
    m_pimpl_->push_back(std::move(r));
}

// -- Accessors ----------------------------------------------------------------

TEMPLATE_PARAMS
typename POINT_SET::coord_pointer POINT_SET::x_data() noexcept {
    return has_pimpl_() ? m_pimpl_->x_data() : nullptr;
}

TEMPLATE_PARAMS
typename POINT_SET::const_coord_pointer POINT_SET::x_data() const noexcept {
    return has_pimpl_() ? m_pimpl_->x_data() : nullptr;
}

TEMPLATE_PARAMS
typename POINT_SET::coord_pointer POINT_SET::y_data() noexcept {
    return has_pimpl_() ? m_pimpl_->y_data() : nullptr;
}

TEMPLATE_PARAMS
typename POINT_SET::const_coord_pointer POINT_SET::y_data() const noexcept {
    return has_pimpl_() ? m_pimpl_->y_data() : nullptr;
}

TEMPLATE_PARAMS
typename POINT_SET::coord_pointer POINT_SET::z_data() noexcept {
    return has_pimpl_() ? m_pimpl_->z_data() : nullptr;
}

TEMPLATE_PARAMS
typename POINT_SET::const_coord_pointer POINT_SET::z_data() const noexcept {
    return has_pimpl_() ? m_pimpl_->z_data() : nullptr;
}

// -- Private ------------------------------------------------------------------

TEMPLATE_PARAMS
typename POINT_SET::reference POINT_SET::at_(size_type i) {
    return (*m_pimpl_)[i];
}

TEMPLATE_PARAMS
typename POINT_SET::const_reference POINT_SET::at_(size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

TEMPLATE_PARAMS
typename POINT_SET::size_type POINT_SET::size_() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->size();
}

TEMPLATE_PARAMS
bool POINT_SET::has_pimpl_() const noexcept { return !(m_pimpl_ == nullptr); }

#undef POINT_SET
#undef TEMPLATE_PARAMS

template class PointSet<float>;
template class PointSet<double>;

} // namespace chemist
