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

#include <chemist/experimental/basis_set/atomic_basis_set_class.hpp>
#include <chemist/experimental/basis_set/detail_/atomic_basis_set_pimpl.hpp>
#include <stdexcept>

namespace chemist::experimental {
namespace {

using pimpl_pointer = typename AtomicBasisSet::pimpl_pointer;

/// Makes the owning implementation holding shells of type ShellType
template<typename ShellType, typename... Args>
pimpl_pointer make_pimpl(Args&&... args) {
    return std::make_unique<detail_::AtomicBasisSetPIMPL<ShellType>>(
      std::forward<Args>(args)...);
}

/** @brief Selects the implementation for the requested shell type.
 *
 *  This is the one place which maps the runtime description of a shell type
 *  (ShellPurity, AOOrdering) to the compile-time one (the shell class). A new
 *  ordering is supported by adding a case here.
 */
pimpl_pointer make_pimpl(AtomicBasisSet::name_type name,
                         AtomicBasisSet::atomic_number_type atomic_number,
                         AtomicBasisSet::center_type center, ShellPurity purity,
                         AOOrdering ordering) {
    switch(ordering) {
        case AOOrdering::cca:
            if(purity == ShellPurity::pure)
                return make_pimpl<CCAShell<SphericalAO>>(
                  std::move(name), atomic_number, std::move(center));
            return make_pimpl<CCAShell<CartesianAO>>(
              std::move(name), atomic_number, std::move(center));
    }
    throw std::invalid_argument(
      "chemist::experimental::AtomicBasisSet: unknown AO ordering.");
}

} // namespace

AtomicBasisSet::AtomicBasisSet() :
  m_pimpl_(make_pimpl<CCAShell<CartesianAO>>()) {}

AtomicBasisSet::AtomicBasisSet(name_type name, atomic_number_type atomic_number,
                               center_type center, ShellPurity purity,
                               AOOrdering ordering) :
  m_pimpl_(make_pimpl(std::move(name), atomic_number, std::move(center), purity,
                      ordering)) {}

AtomicBasisSet::AtomicBasisSet(const AtomicBasisSet& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

AtomicBasisSet::AtomicBasisSet(AtomicBasisSet&& other) noexcept = default;

AtomicBasisSet& AtomicBasisSet::operator=(const AtomicBasisSet& other) {
    if(this != &other) AtomicBasisSet(other).swap(*this);
    return *this;
}

AtomicBasisSet& AtomicBasisSet::operator=(AtomicBasisSet&& other) noexcept =
  default;

AtomicBasisSet::~AtomicBasisSet() noexcept = default;

void AtomicBasisSet::swap(AtomicBasisSet& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

} // namespace chemist::experimental
