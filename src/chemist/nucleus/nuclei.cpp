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

#include "detail_/nuclei_pimpl.hpp"
#include <utility>

namespace chemist {

// -- Ctors and Dtor -----------------------------------------------------------

Nuclei::Nuclei() noexcept = default;

Nuclei::Nuclei(std::initializer_list<value_type> qs) :
  m_pimpl_(std::make_unique<pimpl_type>()) {
    for(const auto& x : qs) push_back(x);
}

Nuclei::Nuclei(const Nuclei& other) :
  m_pimpl_(other.has_pimpl_() ? std::make_unique<pimpl_type>(*other.m_pimpl_) :
                                nullptr) {}

Nuclei::Nuclei(Nuclei&& other) noexcept = default;

Nuclei& Nuclei::operator=(const Nuclei& rhs) {
    Nuclei(rhs).m_pimpl_.swap(m_pimpl_);
    return *this;
}

Nuclei& Nuclei::operator=(Nuclei&& rhs) noexcept = default;

Nuclei::~Nuclei() noexcept = default;

// -----------------------------------------------------------------------------

void Nuclei::push_back(value_type q) {
    if(!has_pimpl_()) m_pimpl_ = std::make_unique<pimpl_type>();
    m_pimpl_->push_back(std::move(q));
}

// -- Private methods ---------------------------------------------------------

typename Nuclei::reference Nuclei::at_(size_type i) { return (*m_pimpl_)[i]; }

typename Nuclei::const_reference Nuclei::at_(size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

typename Nuclei::size_type Nuclei::size_() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->size();
}

bool Nuclei::has_pimpl_() const noexcept { return m_pimpl_ != nullptr; }

} // namespace chemist
