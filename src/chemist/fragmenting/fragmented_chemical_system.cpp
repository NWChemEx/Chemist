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

#include <chemist/fragmenting/fragmented_chemical_system.hpp>

namespace chemist::fragmenting {
namespace detail_ {

template<typename ChemicalSystemType>
class FragmentedChemicalSystemPIMPL {
private:
    /// Type of *this
    using my_type = FragmentedChemicalSystemPIMPL<ChemicalSystemType>;

public:
    /// Type *this implements
    using parent_type = FragmentedChemicalSystem<ChemicalSystemType>;

    /// Pull in types from parent_type
    ///@{
    using fragmented_molecule_type =
      typename parent_type::fragmented_molecule_type;
    using supersystem_reference = typename parent_type::supersystem_reference;
    using const_supersystem_reference =
      typename parent_type::const_supersystem_reference;
    using size_type     = typename parent_type::size_type;
    using pimpl_pointer = typename parent_type::pimpl_pointer;
    ///@}

    FragmentedChemicalSystemPIMPL(fragmented_molecule_type frags) :
      m_frags_(std::move(frags)) {}

    FragmentedChemicalSystemPIMPL(const FragmentedChemicalSystemPIMPL& other) =
      default;

    pimpl_pointer clone() const { return std::make_unique<my_type>(*this); }

    auto operator[](size_type i) { return supersystem_reference(m_frags_[i]); }
    const auto operator[](size_type i) const {
        return const_supersystem_reference(m_frags_[i]);
    }
    size_type size() const noexcept { return m_frags_.size(); }

    auto supersystem() { return supersystem_reference(m_frags_.supersystem()); }
    auto supersystem() const {
        return const_supersystem_reference(m_frags_.supersystem());
    }

    auto& frags() { return m_frags_; }
    const auto& frags() const { return m_frags_; }

private:
    fragmented_molecule_type m_frags_;
};

} // namespace detail_

#define TPARAMS template<typename ChemicalSystemType>
#define FRAGMENTED_CHEMICAL_SYSTEM FragmentedChemicalSystem<ChemicalSystemType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
FRAGMENTED_CHEMICAL_SYSTEM::FragmentedChemicalSystem() noexcept = default;

TPARAMS
FRAGMENTED_CHEMICAL_SYSTEM::FragmentedChemicalSystem(
  fragmented_molecule_type frags) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(frags))) {}

TPARAMS
FRAGMENTED_CHEMICAL_SYSTEM::FragmentedChemicalSystem(
  const FragmentedChemicalSystem& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS
FRAGMENTED_CHEMICAL_SYSTEM::FragmentedChemicalSystem(
  FragmentedChemicalSystem&& other) noexcept = default;

TPARAMS
FRAGMENTED_CHEMICAL_SYSTEM& FRAGMENTED_CHEMICAL_SYSTEM::operator=(
  const FragmentedChemicalSystem& rhs) {
    if(this != &rhs) FragmentedChemicalSystem(rhs).swap(*this);
    return *this;
}

TPARAMS
FRAGMENTED_CHEMICAL_SYSTEM& FRAGMENTED_CHEMICAL_SYSTEM::operator=(
  FragmentedChemicalSystem&& rhs) noexcept = default;

TPARAMS FRAGMENTED_CHEMICAL_SYSTEM::~FragmentedChemicalSystem() noexcept =
  default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

TPARAMS
typename FRAGMENTED_CHEMICAL_SYSTEM::fragmented_molecule_type&
FRAGMENTED_CHEMICAL_SYSTEM::fragmented_molecule() {
    assert_pimpl_();
    return m_pimpl_->frags();
}

TPARAMS
const typename FRAGMENTED_CHEMICAL_SYSTEM::fragmented_molecule_type&
FRAGMENTED_CHEMICAL_SYSTEM::fragmented_molecule() const {
    assert_pimpl_();
    return std::as_const(*m_pimpl_).frags();
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
void FRAGMENTED_CHEMICAL_SYSTEM::swap(
  FragmentedChemicalSystem& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool FRAGMENTED_CHEMICAL_SYSTEM::operator==(
  const FragmentedChemicalSystem& rhs) const noexcept {
    if(this->supersystem() != rhs.supersystem()) return false;
    if(this->size() != rhs.size()) return false;
    if(this->size() == 0) return true; // Both empty and have same super sys
    for(size_type i = 0; i < this->size(); ++i)
        if((*m_pimpl_)[i] != (*rhs.m_pimpl_)[i]) return false;
    return true;
}

TPARAMS
bool FRAGMENTED_CHEMICAL_SYSTEM::operator!=(
  const FragmentedChemicalSystem& rhs) const noexcept {
    return !((*this) == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected methods
// -----------------------------------------------------------------------------

TPARAMS
typename FRAGMENTED_CHEMICAL_SYSTEM::reference FRAGMENTED_CHEMICAL_SYSTEM::at_(
  size_type i) {
    return (*m_pimpl_)[i];
}

TPARAMS
typename FRAGMENTED_CHEMICAL_SYSTEM::const_reference
FRAGMENTED_CHEMICAL_SYSTEM::at_(size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

TPARAMS
typename FRAGMENTED_CHEMICAL_SYSTEM::size_type
FRAGMENTED_CHEMICAL_SYSTEM::size_() const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

TPARAMS
typename FRAGMENTED_CHEMICAL_SYSTEM::supersystem_reference
FRAGMENTED_CHEMICAL_SYSTEM::supersystem_() {
    return has_pimpl_() ? m_pimpl_->supersystem() : supersystem_reference{};
}

TPARAMS
typename FRAGMENTED_CHEMICAL_SYSTEM::const_supersystem_reference
FRAGMENTED_CHEMICAL_SYSTEM::supersystem_() const {
    return has_pimpl_() ? std::as_const(*m_pimpl_).supersystem() :
                          const_supersystem_reference{};
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool FRAGMENTED_CHEMICAL_SYSTEM::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
void FRAGMENTED_CHEMICAL_SYSTEM::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("FragmentedChemicalSystem has no PIMPL");
}

#undef FRAGMENTED_CHEMICAL_SYSTEM
#undef TPARAMS

template class FragmentedChemicalSystem<ChemicalSystem>;
template class FragmentedChemicalSystem<const ChemicalSystem>;

} // namespace chemist::fragmenting
