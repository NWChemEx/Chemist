/*
 * Copyright 2022 NWChemEx-Project
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
#include <chemist/basis_set2/primitive/primitive.hpp>
#include <chemist/detail_/view/traits.hpp>
#include <chemist/point/point_view2.hpp>
#include <optional>

namespace chemist::basis_set {

/** @brief Models a reference to a Gaussian Primitive.
 *
 *  This class has the same API as Primitive; however, instances of
 *  PrimitiveView do NOT own their state, they alias it.
 *
 * @tparam PrimitiveType Expected to be a, possibly cv-qualified,
 *                       Primitive type.
 */
template<typename PrimitiveType>
class PrimitiveView {
private:
    /// Used to work out view types
    using traits_type = chemist::detail_::ViewTraits<PrimitiveType>;

    /// Is the type of *this acting like it's const?
    static constexpr bool is_const_v = traits_type::is_const_v;

    /// Typedefs so we don't need "typename" and "template" below
    template<typename T>
    using apply_const = typename traits_type::template apply_const<T>;

    template<typename T>
    using apply_const_ref = typename traits_type::template apply_const_ref<T>;

    template<typename T>
    using ptr_type = typename traits_type::template apply_const_ptr<T>;

    /// Is @p OtherType the same type as PrimitiveType<Primitive<T>>?
    template<typename OtherType>
    static constexpr auto other_is_mutable_view =
      std::is_same_v<OtherType, PrimitiveView<typename traits_type::type>>;

    /// Is @p OtherType the same type as PrimitiveType<const Primitive<T>>?
    template<typename OtherType>
    static constexpr auto other_is_const_view =
      std::is_same_v<OtherType,
                     PrimitiveView<const typename traits_type::type>>;

    /// Disables a function if *this is not read-only and @p OtherType isn't
    /// a mutable view
    template<typename OtherType>
    using enable_if_this_const_other_mutable =
      std::enable_if_t<is_const_v && other_is_mutable_view<OtherType>>;

    /// Disables a function if *this is read-only and @p OtherType is
    /// a mutable view
    template<typename OtherType>
    using enable_if_this_mutable_other_const =
      std::enable_if_t<!is_const_v && other_is_const_view<OtherType>>;

public:
    /// The unqualified type *this is a view of
    using primitive_type = typename traits_type::type;

    /// A type acting like a possibly read-only reference to a primitive
    using primitive_reference = PrimitiveView<primitive_type>;

    /// A type acting like a read-only reference to a primitive
    using const_primitive_reference = PrimitiveView<const primitive_type>;

    // -- Types associated with where the primitive is centered

    /// rank 1 tensor-like type used to hold the center
    using center_type = typename primitive_type::center_type;

    /// Type of a possibly mutable reference to the center
    using center_reference = PointView2<apply_const<center_type>>;

    /// Type of a read-only reference to the center
    using const_center_reference = PointView2<const center_type>;

    // -- Types associated with the Primitive's coefficient

    /// Floating-point type used to hold coefficient
    using coefficient_type = typename primitive_type::coefficient_type;

    /// Type of a possibly mutable reference to the coefficient
    using coefficient_reference = apply_const_ref<coefficient_type>;

    /// Type of a read-only reference to the coefficient
    using const_coefficient_reference = const coefficient_type&;

    /// Type of a pointer to a possibly mutable coefficient
    using coefficient_pointer = ptr_type<coefficient_type>;

    /// Type of a read-only pointer to a coefficient
    using const_coefficient_pointer =
      typename primitive_type::const_coefficient_pointer;

    // -- Types associated with the Primitive's exponent

    /// Floating-point type used to hold exponent
    using exponent_type = typename primitive_type::exponent_type;

    /// Type of a possibly mutable reference to the exponent
    using exponent_reference = apply_const_ref<exponent_type>;

    /// Type of a read-only reference to the exponent
    using const_exponent_reference = const exponent_type&;

    /// Type of a pointer to a possibly mutable coefficient
    using exponent_pointer = ptr_type<exponent_type>;

    /// Type of a read-only pointer to a coefficient
    using const_exponent_pointer =
      typename primitive_type::const_exponent_pointer;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    PrimitiveView() noexcept = default;

    /** @brief Creates a PrimitiveView that aliases the provided Primitive
     *
     *  This ctor is used to create a new PrimitiveView that aliases the
     *  provided Primitive. This ctor is primarily designed to be used when the
     *  Primitive's state is unpacked (such as when it is stored in a
     *  ContractedGaussian instance).
     *
     *  @param[in] coef The expansion coefficient of the Primitive.
     *  @param[in] exp The exponent of the Primitive.
     *  @param[in] x The x-coordinate where the Primitive is centered.
     *  @param[in] y The y-coordinate where the Primitive is centered.
     *  @param[in] z The z-coordinate where the Primitive is centered.
     *
     *  @throw None No throw guarantee.
     */
    PrimitiveView(coefficient_reference coef, exponent_reference exp,
                  typename center_type::coord_reference x,
                  typename center_type::coord_reference y,
                  typename center_type::coord_reference z) noexcept :
      PrimitiveView(coef, exp, center_reference(x, y, z)) {}

    /** @brief Creates a PrimitiveView that aliases the provided Primitive
     *
     *  This ctor is used to create a new PrimitiveView that aliases the
     *  provided Primitive. This ctor is primarily designed to be used when the
     *  Primitive's state is unpacked (such as when it is stored in a
     *  ContractedGaussian instance).
     *
     *  @param[in] coef The expansion coefficient of the Primitive.
     *  @param[in] exp The exponent of the Primitive.
     *  @param[in] center Where the Primitive is centered
     *
     *  @throw None No throw guarantee
     */
    PrimitiveView(coefficient_reference coef, exponent_reference exp,
                  center_reference center) noexcept :
      m_center_(std::move(center)), m_coef_(&coef), m_exp_(&exp) {}

    /** @brief Creates a view of an existing Primitive object.
     *
     *  @note This is NOT the copy constructor!
     *
     *  This method takes a `Primitive<T>` object and creates a view of it.
     *
     *  @param[in] prim The Primitive we are aliasing.
     *
     *  @throw None No throw guarantee
     */
    PrimitiveView(apply_const_ref<primitive_type> prim) noexcept;

    /** @brief Allows assigning Primitives to *this.
     *
     *  @note This is NOT copy assignment.
     *
     *  As long as *this is a view of a mutable Primitive it should be possible
     *  to assign Primitives objects to the aliased Primitive. This method does
     *  NOT alias @p rhs, but instead sets the aliased state to a copy of the
     *  state in @p rhs. If you want to copy @p rhs, use copy assignment
     *  instead.
     *
     *  @tparam PrimType2 The type of the primitive whose state is being copied
     *                    into *this. PrimType2 must be Primitive<T> for this
     *                    function to not be disabled. Defaults to Primitive<T>
     *  @tparam <anonymous> Used to disable this function when the type of
     *                      *this is not PrimitiveView<Primitive<T>> and
     *                      PrimType2 is not Primitive<T>.
     *
     *  @return *this after copying the state of @p rhs into the state aliased
     *          by *this.
     *
     *  @throw std::runtime_error if *this is a view of a null Primitive or if
     *                            @p rhs is a null Primitive.
     */
    template<
      typename PrimType2 = std::decay_t<PrimitiveType>,
      typename = std::enable_if<std::is_same_v<PrimType2, PrimitiveType>>>
    PrimitiveView& operator=(const PrimType2& rhs);

    /** @brief Allows mutable views to be implicitly converted to read-only
     *         views.
     *
     *  This ctor is only enabled with the type of *this is a read-only view and
     *  the type of @p other is a mutable view.
     *
     *  @tparam OtherType The type of the other view. If OtherType is any type
     *                    other than PrimitiveView<Primitive<T>> SFINAE will
     *                    disable this ctor.
     *
     *  @tparam <anonymous> Type used for SFINAE.
     *
     *  @throw None No throw guarantee
     */

    template<typename OtherType,
             typename = enable_if_this_const_other_mutable<OtherType>>
    PrimitiveView(const OtherType& other) :
      m_center_(other.center()),
      m_coef_(&other.coefficient()),
      m_exp_(&other.exponent()) {}

    // -------------------------------------------------------------------------
    // -- Getters and setters
    // -------------------------------------------------------------------------

    /** @brief Returns a possibly mutable view of the center.
     *
     *  This method is used to get a view of the object representing the point
     *  in space where *this is centered. The resulting object is a modifiable
     *  view if @p PrimitiveType is non-const and read-only otherwise.
     *
     *  @return A view of the primitive's center.
     *
     *  @throw std::runtime_error if *this is a view of a null primitive. Strong
     *                            throw guarantee.
     */
    center_reference center();

    /** @brief Returns a read-only view of the center
     *
     *  This method is the same as the non-const version, except that the
     *  returned center is guarantee to be read-only.
     *
     *  @return A read-only view of the point where the primitive is centered.
     *
     *  @throw std::runtime_error if *this is a view of a null primitive.
     *                            Strong throw guarantee.
     */
    const_center_reference center() const;

    /** @brief Returns the coefficient of the aliased Primitive.
     *
     *  This function can be used to access the coefficient of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p PrimitiveType is non-const.
     *
     *  @return The aliased Primitive's coefficient in a (possibly) read/write
     *          format.
     *
     *  @throw std::runtime_error if *this is a view of a null primitive.
     *                            Strong throw guarantee.
     */
    coefficient_reference coefficient();

    /** @brief Returns the coefficient of the aliased Primitive.
     *
     *  This function can be used to access the coefficient of the aliased
     *  Primitive in a read-only state.
     *
     *  @return The aliased Primitive's coefficient in a read-only format.
     *
     *  @throw std::runtime_error if *this is a view of a null primitive.
     *                            Strong throw guarantee.
     */
    const_coefficient_reference coefficient() const;

    /** @brief Returns the exponent of the aliased Primitive.
     *
     *  This function can be used to access the exponent of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p T is non-const.
     *
     *  @return The aliased Primitive's exponent in a (possibly) read/write
     *          format.
     *
     *  @throw std::runtime_error if *this is a view of a null primitive.
     *                            Strong throw guarantee
     */
    exponent_reference exponent();

    /** @brief Returns the exponent of the aliased Primitive.
     *
     *  This function can be used to access the exponent of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's exponent in a read-only format.
     *
     *  @throw std::runtime_error if *this is a view of a null primitive.
     *                            Strong throw guarantee.
     */
    const_exponent_reference exponent() const;

    // -------------------------------------------------------------------------
    // -- Utility functions
    // -------------------------------------------------------------------------

    /** @brief Exchanges the Primitives aliased by *this and @p other.
     *
     *  @param[in,out] other The view to swap state with. After this call
     *                       @p other will alias the Primitive which was
     *                       previously aliased by *this, and *this will alias
     *                       the primitive @p other previously aliased.
     *
     *  @throw None No throw guarantee.
     *
     */
    void swap(PrimitiveView& other) noexcept;

    /** @brief Is *this a view of a null primitive?
     *
     *  Primitives may be in a null state. Therefore it is also possible for a
     *  view of a primitive to be aliasing a null primitive. This is subtly
     *  different than a null view in that a PrimitiveView of a null primitive
     *  is still a view of a primitive, whereas a null view (if it were
     *  possible) would not be associated with a primitive.
     *
     *  @return True if *this is view of a null primitive and false otherwise.
     *
     *  @throw None No throw guarantee.
     *
     */
    bool is_null() const noexcept;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  This method value compares the center, coefficient, and exponent
     *  aliased by *this to the respective center, coefficient, and exponent
     *  aliased by @p rhs. If each respective comparison is true, then *this is
     *  value equal to @p rhs, otherwise they are different. Note that this
     *  is a value comparison, and not a determination of whether or not
     *  *this "points to" the same primitive as @p rhs.
     *
     *  @param[in] rhs The view we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const PrimitiveView& rhs) const noexcept;

    /** @brief Allows comparing a mutable view to a read-only view.
     *
     *  This works the same as the other operator==(const PrimitiveView&)
     *  method except it allows the rhs to be a view of a read-only primitive.
     *
     *  @note The reverse comparison, read-only view to a mutable view works
     *        already because mutable views are implicitly convertible to
     *        read-only views.
     *
     *  @tparam OtherType Must be PrimitiveView<const Primitive<T>> to not be
     *                    disabled by SFINAE.
     *
     *  @tparam <anonymous> Parameter used for SFINAE.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherType,
             typename = enable_if_this_mutable_other_const<OtherType>>
    bool operator==(const OtherType& rhs) const noexcept {
        return rhs == *this;
    }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  This method is similar to the comparison between two PrimitiveView
     *  objects, except @p rhs owns its value (and does not alias it).
     *
     *  @param[in] rhs The primitive we are comparing to.
     *
     *  @return True if the state aliased by *this is value equal to the state
     *          owned by @p rhs and false otherwise.
     *
     * @throw None No throw guarantee.
     */
    bool operator==(const primitive_type& rhs) const noexcept;

    /** @brief Determines if *this is different from @p rhs.
     *
     *  This method simply defines different as not value equal. See the
     *  description for operator== for the definition of value equal.
     *
     *  @param[in] rhs The PrimitiveView we are comparing to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const PrimitiveView& rhs) const noexcept {
        return !(*this == rhs);
    }

    /** @brief Allows comparing a mutable view to a read-only view.
     *
     *  This works the same as the other operator!=(const PrimitiveView&)
     *  method except it allows the rhs to be a view of a read-only primitive.
     *
     *  @note The reverse comparison, read-only view to a mutable view works
     *        already because mutable views are implicitly convertible to
     *        read-only views.
     *
     *  @tparam OtherType Must be PrimitiveView<const Primitive<T>> to not be
     *                    disabled by SFINAE.
     *
     *  @tparam <anonymous> Parameter used for SFINAE.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename OtherType,
             typename = enable_if_this_mutable_other_const<OtherType>>
    bool operator!=(const OtherType& rhs) const noexcept {
        return rhs != *this;
    }

    /** @brief Determines if *this is different from @p rhs.
     *
     *  This method simply defines different as not value equal. See the
     *  description for operator== for the definition of value equal.
     *
     *  @param[in] rhs The Primitive we are comparing to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const primitive_type& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Throws std::runtime_error if *this is a view of a null primitive
    void assert_non_null_() const;

    /// The point in space where *this is centered
    std::optional<center_reference> m_center_;

    /// The scale factor on *this
    ptr_type<coefficient_type> m_coef_ = nullptr;

    /// The exponent on *ths
    ptr_type<exponent_type> m_exp_ = nullptr;
};

template<typename PrimitiveType>
template<typename PrimType2, typename>
PrimitiveView<PrimitiveType>& PrimitiveView<PrimitiveType>::operator=(
  const PrimType2& rhs) {
    assert_non_null_();
    if(rhs.is_null())
        throw std::runtime_error(
          "Assigning a null Primitive to a PrimitiveView is currently not "
          "supported.");
    center().x()  = rhs.center().x();
    center().y()  = rhs.center().y();
    center().z()  = rhs.center().z();
    coefficient() = rhs.coefficient();
    exponent()    = rhs.exponent();
    return *this;
}

// Ensures PrimitiveType can appear on the left side of operator==
template<typename PrimitiveType>
bool operator==(const std::decay_t<PrimitiveType>& lhs,
                const PrimitiveView<PrimitiveType>& rhs) {
    return rhs == lhs;
}

// Ensures PrimitiveType can appear on the left side of operator!=
template<typename PrimitiveType>
bool operator!=(const std::decay_t<PrimitiveType>& lhs,
                const PrimitiveView<PrimitiveType>& rhs) {
    return rhs != lhs;
}

extern template class PrimitiveView<PrimitiveD>;
extern template class PrimitiveView<const PrimitiveD>;
extern template class PrimitiveView<PrimitiveF>;
extern template class PrimitiveView<const PrimitiveF>;

} // namespace chemist::basis_set
