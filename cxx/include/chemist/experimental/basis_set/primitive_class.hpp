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

#pragma once
#include <chemist/experimental/basis_set/primitive_common.hpp>

namespace chemist::experimental {

/** @brief Models a Gaussian primitive by value.
 *
 *  See PrimitiveCommon for the function *this models and for the shared API.
 *  The parameters are stored in the type-erased wtf::fp::Float class, so the
 *  concrete floating-point type is chosen by whoever creates the primitive and
 *  never appears in the API.
 *
 *  *this owns its coefficient, exponent, angular momentum, and center. See
 *  PrimitiveView for a class with the same API which aliases state owned by
 *  something else.
 */
class Primitive : public PrimitiveCommon<Primitive, Primitive> {
private:
    /// Type *this inherits from
    using base_type = PrimitiveCommon<Primitive, Primitive>;

    /// Lets the CRTP base reach coefficient_, exponent_, l_, and center_
    friend base_type;

    /// Lets a view alias *this's state directly
    template<typename PrimitiveType>
    friend class PrimitiveView;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_reference;
    using typename base_type::angular_momentum_type;
    using typename base_type::center_reference;
    using typename base_type::center_type;
    using typename base_type::coefficient_reference;
    using typename base_type::coefficient_type;
    using typename base_type::const_angular_momentum_reference;
    using typename base_type::const_center_reference;
    using typename base_type::const_coefficient_reference;
    using typename base_type::const_exponent_reference;
    using typename base_type::coord_type;
    using typename base_type::exponent_reference;
    using typename base_type::exponent_type;
    using typename base_type::size_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a null primitive.
     *
     *  The resulting primitive has a zero coefficient, a zero exponent, an
     *  angular momentum of 0, and is centered at the origin. The state can be
     *  changed by calling the `set_*` methods.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    Primitive() = default;

    /** @brief Creates a primitive with the specified parameters.
     *
     *  This is the ctor most callers want. The concrete floating-point type
     *  of the coefficient, exponent, and center is deduced from the arguments
     *  and is what *this will hold from then on.
     *
     *  @tparam T The concrete floating-point type of @p coefficient,
     *           @p exponent, @p x, @p y, and @p z.
     *
     *  @param[in] coefficient The weight of the primitive.
     *  @param[in] exponent The exponent of the primitive.
     *  @param[in] l The total angular momentum of the primitive.
     *  @param[in] x The Cartesian x-coordinate of the primitive's center.
     *  @param[in] y The Cartesian y-coordinate of the primitive's center.
     *  @param[in] z The Cartesian z-coordinate of the primitive's center.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<concepts::FloatingPoint T>
    Primitive(T coefficient, T exponent, angular_momentum_type l, T x, T y,
              T z) :
      m_coefficient_(std::move(coefficient)),
      m_exponent_(std::move(exponent)),
      m_l_(l),
      m_center_(std::move(x), std::move(y), std::move(z)) {}

    /** @brief Creates a primitive from already type-erased parameters.
     *
     *  This ctor is primarily for generic code which has received the
     *  parameters from another type-erased source and thus does not know
     *  their concrete floating-point type.
     *
     *  @param[in] coefficient The weight of the primitive.
     *  @param[in] exponent The exponent of the primitive.
     *  @param[in] l The total angular momentum of the primitive.
     *  @param[in] x The Cartesian x-coordinate of the primitive's center.
     *  @param[in] y The Cartesian y-coordinate of the primitive's center.
     *  @param[in] z The Cartesian z-coordinate of the primitive's center.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    Primitive(coefficient_type coefficient, exponent_type exponent,
              angular_momentum_type l, coord_type x, coord_type y,
              coord_type z) :
      m_coefficient_(std::move(coefficient)),
      m_exponent_(std::move(exponent)),
      m_l_(l),
      m_center_(std::move(x), std::move(y), std::move(z)) {}

    /** @brief Creates a primitive centered on an already existing point.
     *
     *  @param[in] coefficient The weight of the primitive.
     *  @param[in] exponent The exponent of the primitive.
     *  @param[in] l The total angular momentum of the primitive.
     *  @param[in] r0 The point where the primitive is centered.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    Primitive(coefficient_type coefficient, exponent_type exponent,
              angular_momentum_type l, center_type r0) :
      m_coefficient_(std::move(coefficient)),
      m_exponent_(std::move(exponent)),
      m_l_(l),
      m_center_(std::move(r0)) {}

    /** @brief Creates a deep copy of @p other.
     *
     *  @param[in] other The primitive to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    Primitive(const Primitive& other) = default;

    /** @brief Takes ownership of @p other's state.
     *
     *  @param[in,out] other The primitive to take the state from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       undefined, state.
     *
     *  @throw None No throw guarantee.
     */
    Primitive(Primitive&& other) noexcept = default;

    /** @brief Overwrites *this with a deep copy of @p other. */
    Primitive& operator=(const Primitive& other) = default;

    /** @brief Overwrites *this with @p other's state. */
    Primitive& operator=(Primitive&& other) noexcept = default;

    /// Default, no-throw dtor
    ~Primitive() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @param[in,out] other The primitive to exchange state with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(Primitive& other) noexcept;

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what PrimitiveCommon::save wrote. Unlike PrimitiveView,
     *  *this owns its state and so can replace it outright.
     *
     *  @tparam Archive The type of the cereal input archive.
     *
     *  @param[in,out] ar The archive to read from.
     *
     *  @throw std::runtime_error if the archive names a floating-point type
     *                            chemist does not know about or can not
     *                            deserialize. Weak throw guarantee.
     */
    template<typename Archive>
    void load(Archive& ar) {
        m_coefficient_ = detail_::load_float(ar);
        m_exponent_    = detail_::load_float(ar);
        ar(m_l_);
        m_center_.load(ar);
    }

private:
    /** @brief Implements the CRTP base's parameter access. */
    ///@{
    coefficient_reference coefficient_();
    const_coefficient_reference coefficient_() const;
    exponent_reference exponent_();
    const_exponent_reference exponent_() const;
    angular_momentum_reference l_() noexcept { return m_l_; }
    const_angular_momentum_reference l_() const noexcept { return m_l_; }
    center_reference center_() noexcept { return m_center_; }
    const_center_reference center_() const noexcept { return m_center_; }
    ///@}

    /// The weight, or coefficient, of *this
    coefficient_type m_coefficient_ = 0.0;

    /// The exponent of *this
    exponent_type m_exponent_ = 0.0;

    /// The total angular momentum of *this
    angular_momentum_type m_l_ = 0;

    /// The point *this is centered on
    center_type m_center_{};
};

} // namespace chemist::experimental
