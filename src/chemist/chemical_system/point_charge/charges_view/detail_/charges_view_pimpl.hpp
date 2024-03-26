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

#pragma once
#include <chemist/chemical_system/point_charge/charges_view/charges_view.hpp>

namespace chemist::detail_ {

template<typename ChargesType>
class ChargesViewPIMPL {
private:
    /// Type *this implements
    using parent_type = ChargesView<ChargesType>;

public:
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    using reference = typename parent_type::reference;

    using const_reference = typename parent_type::const_reference;

    using charges_traits = typename parent_type::charges_traits;

    using point_charge_traits = typename parent_type::point_charge_traits;

    using size_type = typename parent_type::size_type;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    ChargesViewPIMPL() = default;

    ChargesViewPIMPL(const ChargesViewPIMPL&) = default;

    /// Implemented generically by ChargesView
    ChargesViewPIMPL& operator=(const ChargesViewPIMPL&) = delete;

    /// Implemented generically by ChargesView
    ChargesViewPIMPL(ChargesViewPIMPL&&) = delete;

    /// Implemented generically by ChargesView
    ChargesViewPIMPL& operator=(ChargesViewPIMPL&&) = delete;

    /// No throw, default polymorphic dtor
    virtual ~ChargesViewPIMPL() noexcept = default;

    pimpl_pointer clone() const { return clone_(); }

    // -------------------------------------------------------------------------
    // -- Accessors
    // -------------------------------------------------------------------------

    reference operator[](size_type i) noexcept { return at_(i); }

    const_reference operator[](size_type i) const noexcept { return at_(i); }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    size_type size() const noexcept { return size_(); }

protected:
    virtual pimpl_pointer clone_() const = 0;

    virtual reference at_(size_type i) noexcept = 0;

    virtual const_reference at_(size_type i) const noexcept = 0;

    virtual size_type size_() const noexcept = 0;
};

} // namespace chemist::detail_
