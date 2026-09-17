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
#include <chemist/experimental/basis_set/primitive_class.hpp>

namespace chemist::experimental {

/** @brief An object which behaves like a Primitive, but aliases its state.
 *
 *  *this is the primitive-level counterpart of PointView, and exists for the
 *  same reason: to give a non-owning handle the exact same API as the owning
 *  class, without duplicating that API's declaration, documentation, or
 *  implementation. *this is what a future SoA-backed primitive set would hand
 *  out when indexed, and it is also useful on its own as a non-owning handle
 *  to a Primitive.
 *
 *  @tparam PrimitiveType A, possibly const-qualified, Primitive.
 *                        PrimitiveView<Primitive> aliases mutable state;
 *                        PrimitiveView<const Primitive> aliases read-only
 *                        state and has no setters at all.
 */
template<typename PrimitiveType>
class PrimitiveView
  : public PrimitiveCommon<PrimitiveView<PrimitiveType>, PrimitiveType> {
private:
    /// Type *this inherits from
    using base_type =
      PrimitiveCommon<PrimitiveView<PrimitiveType>, PrimitiveType>;

    /// Lets the CRTP base reach coefficient_, exponent_, l_, and center_
    friend base_type;

    /// Lets the other const-qualification of *this reach its members
    template<typename OtherPrimitiveType>
    friend class PrimitiveView;

    /// True when @p T is the non-const-qualified primitive type *this models
    template<typename T>
    static constexpr bool is_mutable_primitive_v =
      std::is_same_v<std::decay_t<T>, PrimitiveType>;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<std::is_const_v<PrimitiveType> && !std::is_const_v<T>>;

    /// Type used to alias the angular momentum. This is a
    /// pointer rather than an angular_momentum_reference because the latter
    /// would be a genuine C++ reference, which can be bound but never
    /// rebound, making it impossible to implement swap() or the converting
    /// ctors below.
    using l_pointer =
      std::remove_reference_t<typename base_type::angular_momentum_reference>*;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_reference;
    using typename base_type::center_reference;
    using typename base_type::coefficient_reference;
    using typename base_type::coord_type;
    using typename base_type::exponent_reference;
    using typename base_type::size_type;
    ///@}

    /// Type of a, possibly read-only, reference to the aliased Primitive
    using primitive_type =
      typename ChemistClassTraits<PrimitiveType>::value_type;
    using primitive_reference =
      typename ChemistClassTraits<PrimitiveType>::reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view of @p primitive.
     *
     *  This ctor is deliberately implicit so that a Primitive can be passed
     *  wherever a PrimitiveView is expected. The result aliases @p primitive;
     *  it does not copy it, so writing through *this writes into
     *  @p primitive.
     *
     *  @param[in] primitive The primitive *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PrimitiveView(primitive_reference primitive) :
      PrimitiveView(primitive.m_coefficient_.as_view(),
                    primitive.m_exponent_.as_view(), primitive.m_l_,
                    primitive.m_center_) {}

    /** @brief Creates a view of already-aliased state.
     *
     *  This ctor is what lets *this alias state which is not owned by a
     *  Primitive at all, which is how a future SoA-backed primitive set would
     *  hand out its primitives.
     *
     *  @param[in] coefficient A view aliasing the coefficient.
     *  @param[in] exponent A view aliasing the exponent.
     *  @param[in] l A reference aliasing the angular momentum.
     *  @param[in] center A view aliasing the center.
     *
     *  @throw None No throw guarantee.
     */
    PrimitiveView(coefficient_reference coefficient,
                  exponent_reference exponent, angular_momentum_reference l,
                  center_reference center) :
      m_coefficient_(std::move(coefficient)),
      m_exponent_(std::move(exponent)),
      m_l_(&l),
      m_center_(std::move(center)) {}

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided; a read-only view can not be widened into a mutable one.
     *
     *  @tparam OtherPrimitiveType The primitive type @p other models. This
     *                            ctor only participates in overload
     *                            resolution when @p other is mutable and
     *                            *this is not.
     *
     *  @param[in] other The mutable view to convert.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherPrimitiveType,
             typename = enable_mutable_to_const_t<OtherPrimitiveType>>
    PrimitiveView(const PrimitiveView<OtherPrimitiveType>& other) :
      m_coefficient_(other.m_coefficient_),
      m_exponent_(other.m_exponent_),
      m_l_(other.m_l_),
      m_center_(other.m_center_) {}

    /** @brief Creates a view aliasing the same state as @p other.
     *
     *  The copy is shallow: the resulting view aliases the same state
     *  @p other does.
     *
     *  @param[in] other The view to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    PrimitiveView(const PrimitiveView& other) = default;

    /** @brief Takes the aliased state from @p other. */
    PrimitiveView(PrimitiveView&& other) noexcept = default;

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's values through into whatever *this is already aliasing.
     *        Use swap if you want to rebind *this.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherPrimitive The primitive type @p rhs models.
     *
     *  @param[in] rhs The primitive whose state should be copied into the
     *                 state *this aliases.
     *
     *  @return *this, after overwriting the aliased state.
     *
     *  @throw std::runtime_error if the coefficient or exponent of @p rhs are
     *                            not holding the same concrete
     *                            floating-point types as the ones *this
     *                            aliases. Weak throw guarantee.
     */
    template<typename OtherDerived, typename OtherPrimitive>
    PrimitiveView& operator=(
      const PrimitiveCommon<OtherDerived, OtherPrimitive>& rhs) {
        static_assert(!std::is_const_v<PrimitiveType>,
                      "Can not assign through a read-only PrimitiveView.");
        detail_::assign(m_coefficient_, rhs.get_coefficient());
        detail_::assign(m_exponent_, rhs.get_exponent());
        *m_l_     = rhs.get_l();
        m_center_ = rhs.get_center();
        return *this;
    }

    /** @brief Overwrites the state *this aliases with @p rhs's.
     *
     *  This is the copy assignment operator. Like the templated assignment
     *  above, and unlike the copy ctor, it is a deep assignment: it writes
     *  through to the aliased state rather than rebinding *this. This method
     *  only exists when *this aliases mutable state; instantiating it for a
     *  read-only view is a compilation error.
     */
    PrimitiveView& operator=(const PrimitiveView& rhs) {
        static_assert(!std::is_const_v<PrimitiveType>,
                      "Can not assign through a read-only PrimitiveView.");
        if(this != &rhs) {
            detail_::assign(m_coefficient_, rhs.m_coefficient_);
            detail_::assign(m_exponent_, rhs.m_exponent_);
            *m_l_     = *rhs.m_l_;
            m_center_ = rhs.m_center_;
        }
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~PrimitiveView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a Primitive holding a copy of the aliased state.
     *
     *  @return A Primitive value equal to *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    primitive_type as_primitive() const {
        return primitive_type(detail_::copy(m_coefficient_),
                              detail_::copy(m_exponent_), *m_l_,
                              m_center_.as_point());
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @param[in,out] other The view to exchange aliases with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(PrimitiveView& other) noexcept {
        m_coefficient_.swap(other.m_coefficient_);
        m_exponent_.swap(other.m_exponent_);
        std::swap(m_l_, other.m_l_);
        m_center_.swap(other.m_center_);
    }

private:
    /** @brief Implements the CRTP base's parameter access.
     *
     *  Unlike Primitive, *this only needs a single, const-qualified overload
     *  of each hook: whether the returned reference is itself mutable is
     *  decided by PrimitiveType, via the reference types below, not by the
     *  const-qualification of *this.
     */
    ///@{
    coefficient_reference coefficient_() const { return m_coefficient_; }
    exponent_reference exponent_() const { return m_exponent_; }
    angular_momentum_reference l_() const { return *m_l_; }
    center_reference center_() const { return m_center_; }
    ///@}

    /// Aliases the coefficient
    coefficient_reference m_coefficient_;

    /// Aliases the exponent
    exponent_reference m_exponent_;

    /// Aliases the angular momentum. A pointer, not a reference -- see
    /// l_pointer.
    l_pointer m_l_;

    /// Aliases the center
    center_reference m_center_;
};

/// Type of a view of a mutable Primitive
using primitive_view = PrimitiveView<Primitive>;

/// Type of a view of a read-only Primitive
using const_primitive_view = PrimitiveView<const Primitive>;

/** @note Only the mutable instantiation is declared here, for the same
 *        reason PointView<const Point> is not: explicitly instantiating
 *        PrimitiveView<const Primitive> would force the compiler to
 *        instantiate its copy-assignment operator, which deliberately does
 *        not compile for a read-only view.
 */
extern template class PrimitiveView<Primitive>;

} // namespace chemist::experimental
