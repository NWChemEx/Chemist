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
#include <chemist/point/point.hpp>
#include <memory>

namespace chemist {
namespace detail_ {
template<typename T>
class PrimitivePIMPL;
}

/** @brief Models a Gaussian Primitive by value.
 *
 *  This class is used to model a Gaussian Primitive by value. For our purposes
 *  a Gaussian Primitive is a function @f$g@f$ such that:
 *
 *  @f{
 *  g(\vec{r}; \vec{r_0}, c, \alpha) = c e^{-\alpha (\vec{r}-\vec{r_0})^2},
 *  @f}
 *
 *  where @f$c@f$ is the coefficient or weight of the Gaussian (usually in a
 *  linear combination, but could also be a normalization factor), @f$\alpha@f$
 *  is the exponent, and @f$\vec{r_0}@f$ is the point in Cartesian space that 
 *  the Gaussian is centered on.
 *
 * @tparam T The type used to hold the parameters of the Primitive and the
 *         coordinates of the center. Assumed to be a floating-point POD type.
 */
template<typename T>
class Primitive {
public:
    /// Type of this class's PIMPL
    using pimpl_type = detail_::PrimitivePIMPL<T>;

    /// Type of a pointer to this class's PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// rank 1 tensor-like type used to hold the center
    using center_type = Point<T>;

    /// Type of a mutable reference to the center
    using center_reference = center_type&;

    /// Type of a read-only reference to the center
    using const_center_reference = const center_type&;

    /// Floating-point type used to hold the coefficient
    using coefficient_type = T;

    /// Type of a mutable reference to the coefficient
    using coefficient_reference = coefficient_type&;

    /// Type of a read-only reference to the coefficient
    using const_coefficient_reference = const coefficient_type&;

    /// Floating-point type used to hold the exponent
    using exponent_type = T;

    /// Type of a read/write reference to the exponent
    using exponent_reference = exponent_type&;

    /// Type of a read-only reference to a parameter
    using const_exponent_reference = const exponent_type&;

    /** @brief Creates a new null Primitive instance
     *
     *  This ctor creates a null Primitive. Null primitives have no center,
     *  coefficient, or exponent. Calling the non-const version of `center()`,
     *  `coefficient()`, or `exponent()` will convert the null Primitive into
     *  a zero-initialized Primitive whose state can then be set through
     *  the respective getter. Alternatively, a null Primitive can be converted
     *  to a valued Primitive by assignment.
     *
     *  @throw None No throw guarantee
     */
    Primitive() noexcept;

    /** @brief Creates a new Primitive with the specified parameters.
     *
     *  This ctor can be used to create a Primitive with the provided
     *  parameters.
     *
     * @param[in] coef The contraction coefficient of the primitive.
     * @param[in] exp The exponent of the primitive
     * @param[in] x The x-coordinate of the primitive's center
     * @param[in] y The y-coordinate of the primitive's center
     * @param[in] z The z-coordinate of the primitive's center
     *
     * @throw std::bad_alloc if there is insufficient memory to make the PIMPL.
     *        Strong throw guarantee.
     */
    Primitive(T coef, T exp, T x, T y, T z);

    /** @brief Makes a new Primitive which is a deep copy of @p rhs.
     *
     *  This ctor will create a new Primitive instance by deep copying the
     *  state of @p rhs.
     *
     *  @param[in] rhs The Primitive instance to copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    Primitive(const Primitive<T>& rhs);

    /** @brief Makes a new Primitive by taking the state of another instance.
     *
     *  This ctor will make a new Primitive which will take its state from
     *  another instance. All references to @p rhs's state are valid after this
     *  ctor is called, with the caveat that they now point to this instance's
     *  state.
     *
     *  @param[in] rhs The instance to take the state of. After this operation
     *             @p rhs will not have a PIMPL and will thus be in an invalid
     *             state. @p rhs can be returned to a valid state by move
     *             assignment from a valid Primitive.
     *
     *  @throw none No throw guarantee.
     */
    Primitive(Primitive<T>&& rhs) noexcept;

    /** @brief Changes the current instance's state to a deep copy of another
     *         instance's state.
     *
     * This function is used to set the current instance's state to a deep copy
     * of another instance's state. All references to the current instance's
     * state remain valid, but will now hold copies of @p rhs's state.
     *
     * @param[in] rhs The instance to copy the state from.
     *
     * @return The current instance after setting its state to a deep copy of
     *         @p rhs's state.
     *
     * @throw none No throw guarantee.
     */
    Primitive<T>& operator=(const Primitive<T>& rhs) noexcept;

    /** @brief Transfers another instance's state to the current instance.
     *
     * This function is used to transfer the state of another instance to this
     * instance. All references to @p rhs's state remain valid, but now point to
     * this instance's state. All references to the current instance's previous
     * state are invalidated.
     *
     * @param[in,out] rhs The instance to take the state from. After this
     *                operation @p rhs will not contain a PIMPL and will thus be
     *                in an invalid state. @p rhs can be returned to a valid
     *                state by move assigning to it.
     *
     * @return The current instance, now containing @p rhs's state.
     *
     * @throw none No throw guarantee.
     */
    Primitive<T>& operator=(Primitive<T>&& rhs) noexcept;

    /// Defaulted, no throw dtor
    ~Primitive() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters/Setters
    // -------------------------------------------------------------------------

    /** @brief Provides mutable access to the the point where *this is centered
     *
     *  This function provides access to the point where *this is centered. The 
     *  returned instance is modifiable. If *this has no
     *  PIMPL, then this method will create a PIMPL and then return a reference
     *  to the newly created PIMPL's center (which will be set to the origin).
     *
     *  @return The coefficient in a modifiable form.
     *
     *  @throw std::bad_alloc if there is no PIMPL and allocating one fails.
     *         Strong throw guarantee.
     */
    center_reference center();

    /** @brief Provides read-only access to the point where *this is centered.
     *
     *  This function provides access to the point where this Primitive is
     *  centered. The returned instance is read-only.
     *
     *  @return The center in a read-only format.
     *
     *  @throw std::runtime_error if *this is in a null state. Strong throw
     *         guarantee.
     */
    const_center_reference center() const;

    /** @brief Provides read/write access to the coefficient
     *
     *  This function provides access to the expansion coefficient of the
     *  Primitive. The returned instance is modifiable. If *this has no
     *  PIMPL, then this method will create a PIMPL and then return a reference
     *  to the newly created PIMPL's coefficient (which will be set to zero).
     *
     *  @return The coefficient in a modifiable form.
     *
     *  @throw std::bad_alloc if there is no PIMPL and allocating one fails.
     *         Strong throw guarantee.
     */
    coefficient_reference coefficient();

    /** @brief Provides read-only access to the coefficient
     *
     *  This function provides access to the expansion coefficient of the
     *  Primitive. The returned instance is read-only.
     *
     *  @return The coefficient in a read-only format.
     *
     *  @throw std::runtime_error if *this is in a null state. Strong throw
     *         guarantee.
     */
    const_coefficient_reference coefficient() const;

    /** @brief Provides read/write access to the exponent
     *
     *  This function provides access to the exponent of the Primitive. The
     *  returned instance is modifiable. If *this has no PIMPL, then this method
     *  will create a PIMPL and return a reference to the newly created PIMPL's
     *  exponent (which will be set to zero).
     *
     *  @return The exponent in a modifiable form.
     *
     *  @throw std::bad_alloc if *this is in a null state and allocating the
     *         PIMPL fails. Strong throw guarantee.
     */
    exponent_reference exponent();

    /** @brief Provides read-only access to the exponent
     *
     *  This function provides access to the exponent of the Primitive. The
     *  returned instance is read-only.
     *
     *  @return The exponent in a read-only format.
     *
     *  @throw std::runtime_error if *this is in a null state. Strong throw
     *         guarantee.
     */
    const_exponent_reference exponent() const;

    // -------------------------------------------------------------------------
    // -- Utility Functions
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p rhs.
     * 
     *  @param[in,out] rhs The instance to exchange state with. After this call
     *                 @p rhs will have the state which previously resided in
     *                 *this.
     * 
     *  @throw None No throw guarantee.
     */
    void swap(Primitive<T>& rhs) noexcept;

    /** @brief Compares two Primitives for equality.
     *
     *  @relates Primitive
     *
     *  Two Primitive instances are defined as equal if they are centered on the
     *  same point, and they have the same parameters (coefficient and exponent).
     *  Equality in all cases is defined as having exactly the same value to 
     *  machine precision.
     *
     *  @tparam T The type used to store the parameters and coordinates of the
     *          Primitive. Assumed to be a floating-point POD type.
     *
     *  @param[in] lhs The Primitive on the left side of the operator.
     *  @param[in] rhs The Primitive on the right side of the operator.
     *
     *  @return True if the Primitives are equal and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator==(const Primitive<T>& rhs) const noexcept;

    /** @brief Serialize Primitive instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const { ar& coefficient() & exponent() & center(); }

    /** @brief Deserialize for Primitive instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) { ar& coefficient() & exponent() & center(); }

private:
    /// Sets state of *this to already created PIMPL
    Primitive(pimpl_pointer pimpl) noexcept;

    /// Raises std::runtime_error if *this does not have a PIMPL
    void assert_pimpl_() const;

    /// Allocates a default-initialized PIMPL on the heap and returns it
    pimpl_pointer make_pimpl_() const;

    /// Returns false if *this is in a null state and true otherwise
    bool has_pimpl_() const noexcept;

    /// The instance in charge of implementing the class
    pimpl_pointer m_pimpl_;
}; // class Primitive

/** @brief Determines if two Primitives are different.
 *
 *  @relates Primitive
 *
 *  Two Primitive instances are defined as equal if they are centered on the
 *  same point, and they have the same parameters (coefficient and exponent).
 *  Equality in all cases is defined as having exactly the same value to machine
 *  precision. Different is defined as not equal.
 *
 *  @tparam T The type used to store the parameters and coordinates of the
 *          Primitive. Assumed to be a floating-point POD type.
 *
 *  @param[in] lhs The Primitive on the left side of the operator.
 *  @param[in] rhs The Primitive on the right side of the operator.
 *
 *  @return False if the Primitives are equal and true otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
bool operator!=(const Primitive<T>& lhs, const Primitive<T>& rhs) noexcept {
    return !(lhs == rhs);
}

extern template class Primitive<double>;
extern template class Primitive<float>;

} // namespace chemist
