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
#include <chemist/point_charge/charges.hpp>
#include <chemist/point/point_set.hpp>
#include <vector>

namespace chemist::detail_ {

/** @brief Implements the Charges class.
 *
 *  Charges is a set of PointCharges. Each PointCharge is a charge and a
 *  point. This class unpacks the charges into a contiguous array, and then
 *  unpacks the point part into a PointSet.
 *
 *  @tparam T The floating point type used to hold the point charge's charge.
 *            Assumed to be either float or double.
 */
template<typename T>
class ChargesPIMPL {
private:
    /// Type of *this
    using my_type = ChargesPIMPL<T>;

public:
    /// Type we are implementing
    using parent_type = Charges<T>;

    /// Reuse parent class's types
    ///@{
    using pimpl_pointer       = typename parent_type::pimpl_pointer;
    using value_type          = typename parent_type::value_type;
    using reference           = typename parent_type::reference;
    using const_reference     = typename parent_type::const_reference;
    using point_set_type      = typename parent_type::point_set_type;
    using point_set_reference = typename parent_type::point_set_reference;
    using const_point_set_reference =
      typename parent_type::const_point_set_reference;
    using charge_pointer       = typename parent_type::charge_pointer;
    using const_charge_pointer = typename parent_type::const_charge_pointer;
    using size_type            = typename parent_type::size_type;
    ///@}

    /// The type used to store the charge
    using charge_type = typename value_type::charge_type;

    /// Creates a PIMPL capable of powering an empty Charges object
    ChargesPIMPL() = default;

    /// Create a Charges object from a set of points and their charges
    ChargesPIMPL(point_set_type points, std::vector<charge_type> charges) :
      m_points_(std::move(points)), m_charges_(std::move(charges)) {}

    /// Implements push_back
    void push_back(value_type q) {
        m_charges_.push_back(q.charge());
        m_points_.push_back(q);
    }

    /// Implements retrieving a reference to a charge
    reference operator[](size_type i) {
        return reference(m_charges_[i], m_points_[i]);
    }

    /// Implements retrieving a read-only reference to a charge
    const_reference operator[](size_type i) const {
        return const_reference(m_charges_[i], m_points_[i]);
    }

    /// Implements determining the number of point charges
    size_type size() const noexcept { return m_charges_.size(); }

    /// Retrieves the (mutable) PointSet part of *this
    point_set_reference as_point_set() {
        return point_set_reference(m_points_);
    }

    /// Retrieves the read-only PointSet part of *this
    const_point_set_reference as_point_set() const {
        return const_point_set_reference(m_points_);
    }

    /// Retrieves a mutable pointer to the first charge
    charge_pointer charge_data() { return m_charges_.data(); }

    /// Retrieves a read-only pointer to the first charge
    const_charge_pointer charge_data() const { return m_charges_.data(); }

    /// Implements comparisons for Charges
    bool operator==(const ChargesPIMPL& rhs) const {
        return std::tie(m_points_, m_charges_) ==
               std::tie(rhs.m_points_, rhs.m_charges_);
    }

private:
    /// The state associated with being a PointSet<T>
    point_set_type m_points_;

    /// The charges of each point charge
    std::vector<charge_type> m_charges_;
};

} // namespace chemist::detail_
