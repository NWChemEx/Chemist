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

#include <chemist/integral_factory/detail_/pimpl.hpp>
#include <chemist/integral_factory/integral_factory.hpp>

namespace chemist {

IntegralFactory::IntegralFactory() = default;

IntegralFactory::IntegralFactory(pimpl_ptr_t p) noexcept :
  m_pimpl_(std::move(p)) {}

IntegralFactory::IntegralFactory(const IntegralFactory& other) :
  IntegralFactory(other.m_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

IntegralFactory::IntegralFactory(IntegralFactory&& other) :
  IntegralFactory(std::move(other.m_pimpl_)) {}

IntegralFactory::~IntegralFactory() noexcept = default;

IntegralFactory& IntegralFactory::operator=(const IntegralFactory& other) {
    m_pimpl_ = other.m_pimpl_->clone();
    return *this;
}

IntegralFactory& IntegralFactory::operator=(IntegralFactory&& other) {
    m_pimpl_ = std::move(other.m_pimpl_);
    return *this;
}

bool IntegralFactory::operator==(const IntegralFactory& rhs) const noexcept {
    if(m_pimpl_ && rhs.m_pimpl_) {
        return m_pimpl_->are_equal(*rhs.m_pimpl_);
    } else if(!m_pimpl_ && !rhs.m_pimpl_)
        return true;
    return false;
};

const typename IntegralFactory::buffer_t& IntegralFactory::compute(
  const indices_t& indices) {
    assert_pimpl_();
    return m_pimpl_->compute(indices);
}

void IntegralFactory::assert_pimpl_() const {
    if(!m_pimpl_)
        throw std::runtime_error("Instance does not contain a PIMPL. Did you "
                                 "default construct it or move from it?");
};

} // namespace chemist
