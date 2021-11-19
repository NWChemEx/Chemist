#pragma once
#include "chemist/point/point.hpp"
#include <memory>

namespace chemist {
namespace detail_ {
template<typename T>
class PrimitivePIMPL;
}

/** @brief Models a Gaussian Primitive by value.
 *
 *  This class is used to model a Gaussian Primitive by value. For our purposes
 *  a Gaussian Primitive is a function @f$g(\vec{r};c, \alpha)@f$ such that:
 *
 *  @f{
 *  g(\vec{r}; c, \alpha) = c e^{-\alpha r^2},
 *  @f}
 *
 *  where @f$c@f$ is the coefficient or weight of the Gaussian (usually in a
 *  linear combination, but could also be a normalization factor), @f$\alpha@f$
 *  is the exponent, and @f$\vec{r}@f$ is the point in Cartesian space that the
 *  Gaussian is centered on.
 *
 * @tparam T The type used to hold the parameters of the Primitive and the
 *         coordinates of the center. Assumed to be a floating-point POD type.
 */
template<typename T>
class Primitive : public Point<T> {
private:
    /// Type of this class's PIMPL
    using my_pimpl = detail_::PrimitivePIMPL<T>;

    /// Type of a pointer to this class's PIMPL
    using my_pimpl_ptr = std::unique_ptr<my_pimpl>;

    /// Type of a PIMPL for the base class
    using base_pimpl = detail_::PointPIMPL<T>;

    /// Type of a pointer to Point's PIMPL
    using base_pimpl_ptr = std::unique_ptr<base_pimpl>;

public:
    /// Type of a read/write reference to a parameter
    using reference = T&;
    /// Type of a read-only reference to a parameter
    using const_reference = const T&;

    /** @brief Creates a new Primitive instance with zeroed out parameters.
     *
     *  This ctor can be used to create a Primitive with an expansion
     *  coefficient and exponent of 0.0. It will be centered at the origin.
     *  Parameters and components can be changed by calling the respective
     *  member functions.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    Primitive();

    /** @brief Creates a new Primitive with the specified parameters.
     *
     *  This ctor can be used to create a Primitive with the provided
     *  parameters. The resulting instance will contain copies of all
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

    /** @brief Constructs a Primitive with specified PIMPL instances.
     *
     *  This ctor is primarily meant to be called from classes that are defined
     *  in terms of Primitives in order to create the Primitive for implementing
     *  a PrimitiveView instance.
     *
     * @param[in] my_pimpl The PIMPL for the Primitive part of the resulting
     *                     instance.
     * @param[in] point_pimpl The PIMPL for the Point part of the resulting
     *                     instance.
     *
     * @throw None No throw guarantee.
     *
     * Complexity: Constant
     */
    Primitive(my_pimpl_ptr my_pimpl, base_pimpl_ptr point_pimpl) noexcept;

    /// Defaulted, no throw dtor
    ~Primitive() noexcept override;

    /** @brief Provides read/write access to the coefficient
     *
     *  This function provides access to the expansion coefficient of the
     *  Primitive. The returned instance is modifiable.
     *
     *  @return The coefficient in a modifiable form.
     *
     *  @throw none No throw guarantee.
     */
    reference coefficient() noexcept;

    /** @brief Provides read-only access to the coefficient
     *
     *  This function provides access to the expansion coefficient of the
     *  Primitive. The returned instance is read-only.
     *
     *  @return The coefficient in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference coefficient() const noexcept;

    /** @brief Provides read/write access to the exponent
     *
     *  This function provides access to the exponent of the Primitive. The
     *  returned instance is modifiable.
     *
     *  @return The exponent in a modifiable form.
     *
     *  @throw none No throw guarantee.
     */
    reference exponent() noexcept;

    /** @brief Provides read-only access to the exponent
     *
     *  This function provides access to the exponent of the Primitive. The
     *  returned instance is read-only.
     *
     *  @return The exponent in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference exponent() const noexcept;

    /** @brief Serialize Primitive instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& coefficient() & exponent() & this->coord(0) & this->coord(1) &
          this->coord(2);
    }

    /** @brief Deserialize for Primitive instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        ar& coefficient() & exponent() & this->coord(0) & this->coord(1) &
          this->coord(2);
    }

private:
    /// The instance in charge of implementing the class
    my_pimpl_ptr m_pimpl_;
}; // class Primitive

/** @brief Compares two Primitives for equality.
 *
 *  @relates Primitive
 *
 *  Two Primitive instances are defined as equal if they are centered on the
 *  same point, and they have the same parameters (coefficient and exponent).
 *  Equality in all cases is defined as having exactly the same value to machine
 *  precision.
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
template<typename T>
bool operator==(const Primitive<T>& lhs, const Primitive<T>& rhs) noexcept {
    if(std::tie(lhs.coefficient(), lhs.exponent()) ==
       std::tie(rhs.coefficient(), rhs.exponent()))
        return static_cast<const Point<T>&>(lhs) ==
               static_cast<const Point<T>&>(rhs);
    return false;
}

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
