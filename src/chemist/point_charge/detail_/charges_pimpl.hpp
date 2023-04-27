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
#include <chemist/point/point_set.hpp>
#include <chemist/point_charge/charges.hpp>

namespace chemist::detail_ {

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
    using pimpl_pointer   = typename parent_type::pimpl_pointer;
    using value_type      = typename parent_type::value_type;
    using reference       = typename parent_type::reference;
    using const_reference = typename parent_type::const_reference;
    using size_type       = typename parent_type::size_type;
    ///@}

    using charge_type = typename value_type::charge_type;

    void push_back(value_type q) {
        m_charges_.push_back(q.charge());
        m_points_.push_back(q);
    }

    reference operator[](size_type i) {
        return reference(m_charges_[i], m_points_[i]);
    }

    const_reference operator[](size_type i) const {
        return const_reference(m_charges_[i], m_points_[i]);
    }

    size_type size() const noexcept { return m_charges_.size(); }

    const PointSet<T>& as_point_set() { return m_points_; }

private:
    /// The state associated with being a PointSet<T>
    PointSet<T> m_points_;

    /// The charges of each point charge
    std::vector<charge_type> m_charges_;
};

} // namespace chemist::detail_
