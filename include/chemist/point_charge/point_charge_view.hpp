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
#include <chemist/point/point_view2.hpp>
#include <chemist/point_charge/point_charge.hpp>
#include <type_traits>

namespace chemist {

/** @brief Allows viewing data as if it was a PointCharge object.
 *
 *  PointChargeView objects act like references to a PointCharge object. More
 *  specifically, the state inside a PointChargeView is an alias of data owned
 *  by another object (usually a Charges object or related classes).
 *
 *  @tparam ChargeType The type of PointCharge<T> object this class is behaving
 *                     like a view of. Assumed to be either PointCharge<T> or
 *                     const PointCharge<T> (T being either float or double).
 */
template<typename ChargeType>
class PointChargeView
  : public PointView2<typename detail_::ViewTraits<
      ChargeType>::template apply_const<typename ChargeType::point_type>> {
private:
    /// Traits type helping us with TMP
    using traits_type = detail_::ViewTraits<ChargeType>;

    /// Typedef so we don't need "typename" and "template"
    template<typename U>
    using apply_const_ref = typename traits_type::template apply_const_ref<U>;

public:
    // -- Types associated with PointCharge ------------------------------------

    /// Type of an un-qualified PointCharge object
    using point_charge_type = typename traits_type::type;

    /// Type of a PointCharge with parallel const-ness of *this
    using point_charge_reference = apply_const_ref<point_charge_type>;

    /// Type used to store the point charge's charge
    using charge_type = typename point_charge_type::charge_type;

    /// Type used of a reference to the charge
    using charge_reference = apply_const_ref<charge_type>;

    /// Type used for a read-only reference to the charge
    using const_charge_reference =
      typename point_charge_type::const_charge_reference;

    // -- Types associated with Point ------------------------------------------

    /// Type of an un-qualified Point object
    using point_type = typename point_charge_type::point_type;

    /// Type of a view to a Point object with const-ness paralleling *this
    using point_view_type =
      PointView2<typename traits_type::template apply_const<point_type>>;

    /// Type of a read-only view of a Point object
    using const_point_view = PointView2<const point_type>;

    /// Type of a reference to one of the base's coordinates
    using coord_reference = typename point_view_type::coord_reference;

    /** @brief PointCharge ctor
     *
     *  This ctor will alias the state owned by a PointCharge object.
     *
     *  @param[in] q The PointCharge object *this should be a view of.
     *
     *  @throw None No throw guarantee.
     */
    PointChargeView(point_charge_reference q);

    /** @brief Point ctor
     *
     *  This ctor will create a PointChargeView given a reference to the charge
     *  and an already existing PointView object.
     *
     *  @param[in] q The aliased point charge's charge.
     *  @param[in] r A view of the point charge's coordinates.
     *
     *  @throw None No throw guarantee.
     */
    PointChargeView(charge_reference q, point_view_type r);

    /** @brief Value ctor
     *
     *  This ctor takes references to not only the charge, but also the
     *  coordinates where the charge is located.
     *
     *  @param[in] q The point charge's charge.
     *  @param[in] x The x-coordinate of the charge.
     *  @param[in] y The y-coordinate of the charge.
     *  @param[in] z The z-coordiante of the charge.
     *
     *  @throw None No throw guarantee.
     */
    PointChargeView(charge_reference q, coord_reference x, coord_reference y,
                    coord_reference z);

    /** @brief Returns the aliased charge.
     *
     *  This method is used to retrieve the aliased charge. The const-ness of
     *  the returned charge mirrors the const-ness of *this, i.e., if
     *  @p ChargeType is const-qualified the resulting reference will be
     *  read-only, otherwise the result will be read/write.
     *
     *  @return The aliased charge, by reference.
     *
     *  @throw None No throw guarantee.
     */
    charge_reference charge() { return *m_pq_; }

    /** @brief Returns the aliased charge in a read-only state.
     *
     *  @return A read-only reference to the aliased charge.
     *
     *  @throw None No throw guarantee.
     */
    const_charge_reference charge() const { return *m_pq_; }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  This method will check if the charge and coordinates of *this are value
     *  equal to the charge and coordinates of @p rhs.
     *
     *  @param[in] rhs The PointCharge we are comparing to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    bool operator==(const point_charge_type& rhs) const noexcept;

    template<typename T>
    bool operator==(const PointChargeView<T>& rhs) const noexcept;
    ///@}
    /** @brief Determines if *this is different than @p rhs.
     *
     *  This operator defines two PointChargeViews to be different if they
     *  are not value equal. In practice this method simply negates operator==.
     *
     *  @param[in] rhs The PointCharge we are comparing *this to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    ///@{
    bool operator!=(const point_charge_type& rhs) const noexcept;

    template<typename T>
    bool operator!=(const PointChargeView<T>& rhs) const noexcept;
    ///@}

    /** @brief Allows *this to be converted in to a PointCharge.
     *
     *  This will create a new PointCharge object by copying the state aliased
     *  by *this into the
     *
     *  @return A new PointCharge object initialized from the state of *this.
     *
     *  @throw None No throw guarantee.
     */
    point_charge_type as_point_charge() const {
        return point_charge_type(charge(), this->x(), this->y(), this->z());
    }

private:
    /// The type of a pointer used to alias the charge
    using internal_pointer =
      typename traits_type::template apply_const_ptr<charge_type>;

    /// The aliased charge
    internal_pointer m_pq_;
};

/// Same as PointChargeView::operator==, but when a PointCharge is the LHS
template<typename ChargeType, typename ChargeType2>
bool operator==(const PointCharge<ChargeType>& lhs,
                const PointChargeView<ChargeType2>& rhs) {
    return rhs == lhs;
}

/// Same as PointChargeView::operator!=, but when a PointCharge is the LHS
template<typename ChargeType, typename ChargeType2>
bool operator!=(const PointCharge<ChargeType>& lhs,
                const PointChargeView<ChargeType2>& rhs) {
    return rhs != lhs;
}

// -- Inline implementations ---------------------------------------------------

#define POINT_CHARGE_VIEW PointChargeView<ChargeType>

// -- Ctors --------------------------------------------------------------------

template<typename ChargeType>
POINT_CHARGE_VIEW::PointChargeView(point_charge_reference q) :
  PointChargeView(q.charge(), q.x(), q.y(), q.z()) {}

template<typename ChargeType>
POINT_CHARGE_VIEW::PointChargeView(charge_reference q, point_view_type r) :
  point_view_type(r), m_pq_(&q) {}

template<typename ChargeType>
POINT_CHARGE_VIEW::PointChargeView(charge_reference q, coord_reference x,
                                   coord_reference y, coord_reference z) :
  point_view_type(x, y, z), m_pq_(&q) {}

// -- Utility ------------------------------------------------------------------

template<typename ChargeType>
bool POINT_CHARGE_VIEW::operator==(
  const point_charge_type& rhs) const noexcept {
    const point_view_type& plhs = *this;
    const_point_view prhs(rhs);

    return std::tie(charge(), plhs) == std::tie(rhs.charge(), prhs);
}

template<typename ChargeType>
template<typename T>
bool POINT_CHARGE_VIEW::operator==(
  const PointChargeView<T>& rhs) const noexcept {
    const point_view_type& plhs = *this;
    const_point_view prhs(rhs.x(), rhs.y(), rhs.z());

    return std::tie(charge(), plhs) == std::tie(rhs.charge(), prhs);
}

template<typename ChargeType>
bool POINT_CHARGE_VIEW::operator!=(
  const point_charge_type& rhs) const noexcept {
    return !(*this == rhs);
}

template<typename ChargeType>
template<typename T>
bool POINT_CHARGE_VIEW::operator!=(
  const PointChargeView<T>& rhs) const noexcept {
    return !(*this == rhs);
}

#undef POINT_CHARGE_VIEW
} // namespace chemist
