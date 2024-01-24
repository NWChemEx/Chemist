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

#include "detail_/charges_pimpl.hpp"
#include <utility>

namespace chemist {

#define TPARAMS template<typename T>
#define CHARGES Charges<T>

// -- Ctors and Dtor -----------------------------------------------------------

TPARAMS
CHARGES::Charges() noexcept = default;

TPARAMS
CHARGES::Charges(std::initializer_list<value_type> qs) :
  m_pimpl_(std::make_unique<pimpl_type>()) {
    for(const auto& x : qs) push_back(x);
}

TPARAMS
CHARGES::Charges(const Charges& other) :
  m_pimpl_(other.has_pimpl_() ? std::make_unique<pimpl_type>(*other.m_pimpl_) :
                                nullptr) {}

TPARAMS
CHARGES::Charges(Charges&& other) noexcept = default;

TPARAMS
CHARGES& CHARGES::operator=(const Charges& rhs) {
    Charges(rhs).m_pimpl_.swap(m_pimpl_);
    return *this;
}

TPARAMS
CHARGES& CHARGES::operator=(Charges&& rhs) noexcept = default;

TPARAMS
CHARGES::~Charges() noexcept = default;

// -----------------------------------------------------------------------------

TPARAMS
void CHARGES::push_back(value_type q) {
    if(!has_pimpl_()) m_pimpl_ = std::make_unique<pimpl_type>();
    m_pimpl_->push_back(std::move(q));
}

// -- Private methods ---------------------------------------------------------

TPARAMS
typename CHARGES::reference CHARGES::at_(size_type i) { return (*m_pimpl_)[i]; }

TPARAMS
typename CHARGES::const_reference CHARGES::at_(size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

TPARAMS
typename CHARGES::size_type CHARGES::size_() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->size();
}

TPARAMS
bool CHARGES::has_pimpl_() const noexcept { return m_pimpl_ != nullptr; }

#undef CHARGES
#undef TPARAMS

template class Charges<float>;
template class Charges<double>;

} // namespace chemist
