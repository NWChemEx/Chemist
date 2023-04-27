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

#pragma once
#include <chemist/detail_/view/traits.hpp>
#include <chemist/point_charge/point_charge_view.hpp>

namespace chemist {

template<typename NucleusType>
class NucleusView
  : public PointChargeView<typename detail_::ViewTraits<
      NucleusType>::template apply_const<typename NucleusType::charge_type>> {
private:
    /// Traits type helping out with TMP
    using traits_type = detail_::ViewTraits<NucleusType>;

    /// Typedef so we don't need "typename" and "template" for references
    template<typename U>
    using apply_const_ref = typename traits_type::template apply_const_ref<U>;

    /// Typedef so we don't need "typename" and "template" for pointers
    template<typename U>
    using ptr_type = typename traits_type::template apply_const_ptr<U>;

public:
    // -- Nucleus types --------------------------------------------------------

    /// Type of an un-qualified Nucleus object
    using nucleus_type = typename traits_type::type;

    /// Type of a Nucleus reference with const-ness paralleling @p NucleusType
    using nucleus_reference = apply_const_ref<nucleus_type>;

    /// Type of a read-only reference to a Nucleus
    using const_nucleus_reference = const nucleus_type&;

    /// Type of the atomic number
    using atomic_number_type = typename nucleus_type::atomic_number_type;

    /// Reference to atomic number with const-ness paralleling @p NucleusType
    using atomic_number_reference = apply_const_ref<atomic_number_type>;

    /// Type of a read-only reference to the atomic number
    using const_atomic_number_reference =
      typename nucleus_type::const_atomic_number_reference;

    /// Type of the nucleus's mass
    using mass_type = typename nucleus_type::mass_type;

    /// Reference to mass with const-ness paralleling @p NucleusType
    using mass_reference = apply_const_ref<mass_type>;

    /// Type of a read-only reference to the mass
    using const_mass_reference = typename nucleus_type::const_mass_reference;

    // -- PointCharge types ----------------------------------------------------

    /// Type of PointCharge an object of type @p nucleus_type inherits from
    using point_charge_type = typename nucleus_type::point_charge_type;

    /// PointChargeView with const-ness paralleling @p NucleusType
    using charge_view_type = PointChargeView<
      typename traits_type::template apply_const<point_charge_type>>;

    /// Type of a read-only PointChargeView
    using const_charge_view = PointChargeView<const point_charge_type>;

    // -- CTors ----------------------------------------------------------------

    NucleusView(nucleus_reference nuke);
    NucleusView(atomic_number_reference Z, mass_reference m,
                charge_view_type q);

    // -- Accessors ------------------------------------------------------------

    atomic_number_reference Z() noexcept { return *m_pZ_; }
    const_atomic_number_reference Z() const noexcept { return *m_pZ_; }

    mass_reference mass() noexcept { return *m_pmass_; }
    const_mass_reference mass() const noexcept { return *m_pmass_; }

    bool operator==(const_nucleus_reference rhs) const noexcept;

    bool operator!=(const_nucleus_reference rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Pointer to the aliased atomic number
    ptr_type<atomic_number_type> m_pZ_;

    /// Pointer to the aliased mass
    ptr_type<mass_type> m_pmass_;
};

// -- Inline implementations ---------------------------------------------------

template<typename NucleusType>
NucleusView<NucleusType>::NucleusView(nucleus_reference nuke) :
  NucleusView(nuke.Z(), nuke.mass()) {}

template<typename NucleusType>
NucleusView<NucleusType>::NucleusView(atomic_number_reference Z,
                                      mass_reference m, charge_view_type q) :
  charge_view_type(q), m_pZ_(&Z), m_pmass_(&m) {}

template<typename NucleusType>
bool NucleusView<NucleusType>::operator==(
  const_nucleus_reference rhs) const noexcept {
    const charge_view_type& plhs = *this;
    const_charge_view_type prhs(rhs);

    return std::tie(Z(), mass(), plhs) == std::tie(rhs.Z(), rhs.mass(), prhs);
}

} // namespace chemist
