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

#include <chemist/fragmenting/fragmented_molecule.hpp>

namespace chemist::fragmenting {
namespace detail_ {

template<typename MoleculeType>
class FragmentedMoleculePIMPL {
private:
    /// Type of *this
    using my_type = FragmentedMoleculePIMPL<MoleculeType>;

public:
    /// The type *this implements
    using parent_type = FragmentedMolecule<MoleculeType>;

    /// Pull in parent's types
    ///@{
    using pimpl_pointer         = typename parent_type::pimpl_pointer;
    using supersystem_reference = typename parent_type::supersystem_reference;
    using const_supersystem_reference =
      typename parent_type::const_supersystem_reference;
    using fragmented_nuclei_type = typename parent_type::fragmented_nuclei_type;
    using charge_type            = typename parent_type::charge_type;
    using charge_container       = typename parent_type::charge_container;
    using multiplicity_type      = typename parent_type::multiplicity_type;
    using multiplicity_container = typename parent_type::multiplicity_container;
    using reference              = typename parent_type::reference;
    using const_reference        = typename parent_type::const_reference;
    using size_type              = typename parent_type::size_type;
    ///@}

    FragmentedMoleculePIMPL(fragmented_nuclei_type frags, charge_type charge,
                            multiplicity_type multiplicity,
                            charge_container charges,
                            multiplicity_container multiplicities) :
      m_frags_(std::move(frags)),
      m_charge_(charge),
      m_multiplicity_(multiplicity),
      m_charges_(std::move(charges)),
      m_multiplicities_(std::move(multiplicities)) {
        // Sanity check that charge/multiplicity work
        auto ss = supersystem();
    }

    supersystem_reference supersystem() {
        return supersystem_reference{m_frags_.supersystem(), &m_charge_,
                                     &m_multiplicity_};
    }

    const_supersystem_reference supersystem() const {
        return const_supersystem_reference{m_frags_.supersystem(), &m_charge_,
                                           &m_multiplicity_};
    }

    reference operator[](size_type i) {
        return reference(m_frags_[i], &m_charges_[i], &m_multiplicities_[i]);
    }

    const_reference operator[](size_type i) const {
        const auto* pcharge = &m_charges_[i];
        const auto* pmult   = &m_multiplicities_[i];
        return const_reference(m_frags_[i], pcharge, pmult);
    }

    size_type size() const noexcept { return m_frags_.size(); }

    bool operator==(const FragmentedMoleculePIMPL& rhs) const noexcept {
        return std::tie(m_frags_, m_charge_, m_multiplicity_, m_charges_,
                        m_multiplicities_) ==
               std::tie(rhs.m_frags_, rhs.m_charge_, rhs.m_multiplicity_,
                        rhs.m_charges_, rhs.m_multiplicities_);
    }

    pimpl_pointer clone() const {
        return std::make_unique<my_type>(m_frags_, m_charge_, m_multiplicity_,
                                         m_charges_, m_multiplicities_);
    }

    auto& frags() { return m_frags_; }

    const auto& frags() const { return m_frags_; }

private:
    fragmented_nuclei_type m_frags_;

    charge_type m_charge_;

    multiplicity_type m_multiplicity_;

    charge_container m_charges_;

    multiplicity_container m_multiplicities_;
};

} // namespace detail_

namespace {

template<typename NucleiType, typename ChargesType>
auto compute_multiplicities(NucleiType&& nuclei, ChargesType&& charges) {
    auto size = nuclei.size();
    typename FragmentedMolecule<Molecule>::multiplicity_container rv(size, 1);
    using atomic_number_type = typename Nucleus::atomic_number_type;
    using charge_type        = typename Nucleus::charge_type;

    for(decltype(size) i = 0; i < size; ++i) {
        atomic_number_type n_electrons = 0;
        for(const auto& ni : nuclei[i]) n_electrons += ni.Z();
        const charge_type charge_i = charges[i];
        if(charge_i < 0) {
            n_electrons += static_cast<atomic_number_type>(-1 * charge_i);
        } else {
            if(charge_i > n_electrons)
                throw std::runtime_error(
                  "Charge is greater than number of electrons");
            n_electrons -= static_cast<atomic_number_type>(charge_i);
        }
        if(n_electrons % 2) rv[i] = 2;
    }
    return rv;
}

} // namespace

#define TPARAMS template<typename MoleculeType>
#define FRAGMENTED_MOLECULE FragmentedMolecule<MoleculeType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, dtor
// -----------------------------------------------------------------------------

TPARAMS
FRAGMENTED_MOLECULE::FragmentedMolecule() noexcept = default;

TPARAMS
FRAGMENTED_MOLECULE::FragmentedMolecule(supersystem_type molecule) :
  FragmentedMolecule(fragmented_nuclei_type(molecule.nuclei()),
                     molecule.charge(), molecule.multiplicity()) {}

TPARAMS FRAGMENTED_MOLECULE::FragmentedMolecule(
  fragmented_nuclei_type nuclear_frags, charge_type charge,
  multiplicity_type multiplicity) {
    charge_container charges(nuclear_frags.size(), 0);
    auto multiplicities = compute_multiplicities(nuclear_frags, charges);
    std::make_unique<pimpl_type>(std::move(nuclear_frags), charge, multiplicity,
                                 std::move(charges), std::move(multiplicities))
      .swap(m_pimpl_);
}

TPARAMS
FRAGMENTED_MOLECULE::FragmentedMolecule(fragmented_nuclei_type nuclear_frags,
                                        charge_type charge,
                                        multiplicity_type multiplicity,
                                        charge_container charges,
                                        multiplicity_container multiplicities) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(nuclear_frags), charge,
                                        multiplicity, std::move(charges),
                                        std::move(multiplicities))) {}

TPARAMS
FRAGMENTED_MOLECULE::FragmentedMolecule(const FragmentedMolecule& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS
FRAGMENTED_MOLECULE::FragmentedMolecule(FragmentedMolecule&& other) noexcept =
  default;

TPARAMS
FRAGMENTED_MOLECULE& FRAGMENTED_MOLECULE::operator=(
  const FragmentedMolecule& rhs) {
    if(this != &rhs) FragmentedMolecule(rhs).swap(*this);
    return *this;
}

TPARAMS
FRAGMENTED_MOLECULE& FRAGMENTED_MOLECULE::operator=(
  FragmentedMolecule&& rhs) noexcept = default;

TPARAMS
FRAGMENTED_MOLECULE::~FragmentedMolecule() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

TPARAMS
typename FRAGMENTED_MOLECULE::fragmented_nuclei_type&
FRAGMENTED_MOLECULE::fragmented_nuclei() {
    assert_pimpl_();
    return m_pimpl_->frags();
}

TPARAMS
const typename FRAGMENTED_MOLECULE::fragmented_nuclei_type&
FRAGMENTED_MOLECULE::fragmented_nuclei() const {
    assert_pimpl_();
    return std::as_const(*m_pimpl_).frags();
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

TPARAMS
void FRAGMENTED_MOLECULE::swap(FragmentedMolecule& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool FRAGMENTED_MOLECULE::operator==(
  const FragmentedMolecule& rhs) const noexcept {
    if(this->supersystem() != rhs.supersystem()) return false;
    // Possible that both fragment empty system and one or both have no PIMPL
    if(!has_pimpl_() || !rhs.has_pimpl_()) return true;
    return (*m_pimpl_) == (*rhs.m_pimpl_);
}

TPARAMS
bool FRAGMENTED_MOLECULE::operator!=(
  const FragmentedMolecule& rhs) const noexcept {
    return !((*this) == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected methods
// -----------------------------------------------------------------------------

TPARAMS typename FRAGMENTED_MOLECULE::reference FRAGMENTED_MOLECULE::at_(
  size_type i) {
    // N.b. base class will bounds check, we don't need to do it again here
    return (*m_pimpl_)[i];
}

TPARAMS
typename FRAGMENTED_MOLECULE::const_reference FRAGMENTED_MOLECULE::at_(
  size_type i) const {
    // N.b. base class will bounds check, we don't need to do it again here
    return std::as_const(*m_pimpl_)[i];
}

TPARAMS
typename FRAGMENTED_MOLECULE::size_type FRAGMENTED_MOLECULE::size_()
  const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

TPARAMS
typename FRAGMENTED_MOLECULE::supersystem_reference
FRAGMENTED_MOLECULE::supersystem_() {
    return has_pimpl_() ? m_pimpl_->supersystem() : supersystem_reference{};
}

TPARAMS
typename FRAGMENTED_MOLECULE::const_supersystem_reference
FRAGMENTED_MOLECULE::supersystem_() const {
    return has_pimpl_() ? std::as_const(*m_pimpl_).supersystem() :
                          const_supersystem_reference{};
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool FRAGMENTED_MOLECULE::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
void FRAGMENTED_MOLECULE::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("FragmentedMolecule object has no PIMPL");
}

#undef FRAGMENTED_MOLECULE
#undef TPARAMS

template class FragmentedMolecule<Molecule>;
template class FragmentedMolecule<const Molecule>;

} // namespace chemist::fragmenting
