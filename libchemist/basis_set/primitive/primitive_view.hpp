#pragma once
#include "libchemist/basis_set/primitive/primitive.hpp"

namespace libchemist {

/** @brief Models a Gaussian Primitive by reference.
 *
 *  This class is used to model a Gaussian Primitive by reference. For our
 *  purposes a Gaussian Primitive is a function @f$g(\vec{r};c, \alpha)@f$ such
 *  that:
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
class PrimitiveView {
public:
    /// The non-cv qualified type used to hold the parameters
    using value_type = std::remove_const_t<T>;
    /// The type of a (possibly) read/write reference to a parameter
    using reference = T&;
    /// The type of a read-only reference to a parameter
    using const_reference = const T&;
    /// The type used for indexing and offsets
    using size_type = std::size_t;

    /** @brief Creates a view that is not aliasing any Primitive
     *
     *  The PrimitiveView resulting from this ctor is more-or-less a placeholder
     *  instance. Attempting to dereference a parameter is undefined behavior
     *  and likely to result in a segfault. Assigning a valid view to the
     *  resulting view will make it valid.
     *
     *  @throw none No throw guarantee.
     */
    PrimitiveView() noexcept;

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
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPLs. Strong throw guarantee.
     */
    PrimitiveView(T* coef, T* exp, T* x, T* y, T* z);

    /** @brief Makes a new PrimitiveView that aliases the same Primitive as
     *         another instance.
     *
     *  This ctor will create a PrimitiveView that aliases the same Primitive as
     *  @p rhs; however, it will not be coupled to @p rhs. In other words,
     *  changing the Primitive that @p rhs aliases, after calling this function
     *  will not affect the PrimitiveView that results from this ctor.
     *
     *  @param[in] rhs The PrimitiveView whose aliasing is being copied.
     *
     *  @throw std::bad_alloc if there is insufficient memory to make the PIMPL.
     *         Strong throw guarantee.
     */
    PrimitiveView(const PrimitiveView<T>& rhs);

    /** @brief Transfers a PrimitiveView's state to a new PrimitiveView.
     *
     *  This ctor will create a new PrimitiveView by transferring ownership of
     *  an existing PrimitiveView's state to the resulting instance. After this
     *  ctor is called all references to the Primitive @p rhs previously aliased
     *  remain valid, except that they are now stored in the current instance.
     *
     *  @param[in,out] rhs The PrimitiveView whose state is being transferred.
     *                 After this operation @p rhs is in an invalid state with
     *                 no PIMPL. @p rhs can be restored to a valid state by move
     *                 assignment.
     *
     *  @throw none No throw guarantee.
     */
    PrimitiveView(PrimitiveView<T>&& rhs) noexcept = default;

    /** @brief Makes the current PrimitiveView alias the same Primitive as
     *         another instance.
     *
     *  This function will create a PrimitiveView that aliases the same
     *  Primitive as @p rhs; however, it will not be coupled to @p rhs. In other
     *  words, changing which Primitive @p rhs aliases, after calling this
     *  function will not affect this Primitive. All references to the Primitive
     *  previously aliased by this PrimitiveView remain valid as long as the
     *  aliased Primitive remains valid.
     *
     *  @param[in] rhs The PrimitiveView whose aliasing is being copied.
     *
     *  @return The current instance now aliasing the same Primitive as @p rhs.
     *
     *  @throw none No throw guarantee.
     */
    PrimitiveView<T>& operator=(const PrimitiveView<T>& rhs) noexcept;

    /** @brief Transfers a PrimitiveView's state to this PrimitiveView.
     *
     *  This ctor function will cause the current PrimitiveView to alias a new
     *  Primitive by taking ownership of an existing PrimitiveView's state.
     *  After this ctor is called all references to the Primitive @p rhs
     *  previously aliased remain valid, except that they are now stored in the
     *  current instance. References to the state that this PrimitiveView
     *  previously aliased remain valid as long as the aliased Primitive remains
     *  valid.
     *
     *  @param[in,out] rhs The PrimitiveView whose state is being transferred.
     *                 After this operation @p rhs is in an invalid state with
     *                 no PIMPL. @p rhs can be restored to a valid state by move
     *                 assignment.
     *
     *  @return The current instance, but now containing @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    PrimitiveView<T>& operator=(PrimitiveView<T>&& rhs) noexcept = default;

    /** @brief Returns the coefficient of the aliased Primitive.
     *
     *  This function can be used to access the coefficient of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p T is non-const.
     *
     *  @return The aliased Primitive's coefficient in a (possibly) read/write
     *          format.
     *  @throw none No throw guarantee.
     */
    reference coefficient() noexcept { return m_pimpl_.coefficient(); }

    /** @brief Returns the coefficient of the aliased Primitive.
     *
     *  This function can be used to access the coefficient of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's coefficient in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference coefficient() const noexcept;

    /** @brief Returns the exponent of the aliased Primitive.
     *
     *  This function can be used to access the exponent of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p T is non-const.
     *
     *  @return The aliased Primitive's exponent in a (possibly) read/write
     *          format.
     *  @throw none No throw guarantee.
     */
    reference exponent() noexcept { return m_pimpl_.exponent(); }

    /** @brief Returns the exponent of the aliased Primitive.
     *
     *  This function can be used to access the exponent of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's exponent in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference exponent() const noexcept { return m_pimpl_.exponent(); }

    /** @brief Returns a coordinate of the aliased Primitive's center.
     *
     *  This function can be used to access a coordinate of the aliased
     *  Primitive's center in a (possibly) read/write format. The resulting
     *  reference is read/write as long as @p T is non-const.
     *
     *  @param[in] i The coordinate (x for i==0, y for i==1, and z for i==2) to
     *             return. @p i must be in the range [0, 3).
     *  @return The aliased Primitive's coordinate in a (possibly) read/write
     *          format.
     *  @throw std::out_of_range if @p i is not in the range [0, 3). Strong
     *         throw guarantee.
     */
    reference coord(size_type i) { return m_pimpl_.coord(i); }

    /** @brief Returns a coordinate of the aliased Primitive's center.
     *
     *  This function can be used to access a coordinate of the aliased
     *  Primitive's center in a read-only format.
     *
     *  @param[in] i The coordinate (x for i==0, y for i==1, and z for i==2) to
     *             return. @p i must be in the range [0, 3).
     *  @return The aliased Primitive's coordinate in a read-only format.
     *
     *  @throw std::out_of_range if @p i is not in the range [0, 3). Strong
     *         throw guarantee.
     */
    const_reference coord(size_type i) const { return m_pimpl_.coord(i); }

    /** @brief Returns the x-coordinate of the aliased Primitive.
     *
     *  This function can be used to access the x-coordinate of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p T is non-const.
     *
     *  @return The aliased Primitive's x-coordinate in a (possibly) read/write
     *          format.
     *  @throw none No throw guarantee.
     */
    reference x() noexcept { return m_pimpl_.x(); }

    /** @brief Returns the x-coordinate of the aliased Primitive.
     *
     *  This function can be used to access the x-coordinate of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's x-coordinate in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference x() const noexcept { return m_pimpl_.x(); }

    /** @brief Returns the y-coordinate of the aliased Primitive.
     *
     *  This function can be used to access the y-coordinate of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p T is non-const.
     *
     *  @return The aliased Primitive's y-coordinate in a (possibly) read/write
     *          format.
     *  @throw none No throw guarantee.
     */
    reference y() noexcept { return m_pimpl_.y(); }

    /** @brief Returns the y-coordinate of the aliased Primitive.
     *
     *  This function can be used to access the y-coordinate of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's y-coordinate in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference y() const noexcept { return m_pimpl_.y(); }

    /** @brief Returns the z-coordinate of the aliased Primitive.
     *
     *  This function can be used to access the z-coordinate of the aliased
     *  Primitive in a (possibly) read/write format. The resulting reference is
     *  read/write as long as @p T is non-const.
     *
     *  @return The aliased Primitive's z-coordinate in a (possibly) read/write
     *          format.
     *  @throw none No throw guarantee.
     */
    reference z() noexcept { return m_pimpl_.z(); }

    /** @brief Returns the z-coordinate of the aliased Primitive.
     *
     *  This function can be used to access the z-coordinate of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's z-coordinate in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_reference z() const noexcept { return m_pimpl_.z(); }

    /** @brief Allows PrimitiveView to be implicitly converted to a read-only
     *         Primitive.
     *
     *  This function is intended to allow users to use a PrimitiveView whenever
     *  a read-only Primitive is needed. It also will trigger a deep copy if
     *  the PrimitiveView is used in a non-read-only situation. Avoiding any
     *  potential aliasing problems.
     *
     *  @return The current instance as a read-only Primitive.
     *
     *  @throw none No throw guarantee.
     */
    operator const Primitive<value_type>&() const noexcept { return m_pimpl_; }

private:
    /// The object actually implemeting the view
    Primitive<value_type> m_pimpl_;
};

/** @brief Overloads Primitive<T>::operator== for PrimitiveView instances.
 *
 *  @relates PrimitiveView
 *
 *  All equality comparisons involving PrimitiveView instances simply cast all
 *  PrimitiveView instances to Primitive instances and then defer to Primitive's
 *  operator==.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The primitive on the left of the operator.
 * @param[in] rhs The primitive on the right of the operator.
 *
 * @return True if @p lhs aliases a primitive that has the same state as @p rhs
 *         and false otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator==(const PrimitiveView<T>& lhs, const Primitive<U>& rhs) noexcept {
    using value_type = typename PrimitiveView<T>::value_type;
    return static_cast<const Primitive<value_type>&>(lhs) == rhs;
}

/** @brief Overloads Primitive<T>::operator== for PrimitiveView instances.
 *
 *  @relates PrimitiveView
 *
 *  All equality comparisons involving PrimitiveView instances simply cast all
 *  PrimitiveView instances to Primitive instances and then defer to Primitive's
 *  operator==.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The primitive on the left of the operator.
 * @param[in] rhs The primitive on the right of the operator.
 *
 * @return True if @p rhs aliases a primitive that has the same state as @p lhs
 *         and false otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator==(const Primitive<T>& lhs, const PrimitiveView<U>& rhs) noexcept {
    using value_type = typename PrimitiveView<T>::value_type;
    return lhs == static_cast<const Primitive<value_type>&>(rhs);
}

/** @brief Overloads Primitive<T>::operator== for PrimitiveView instances.
 *
 *  @relates PrimitiveView
 *
 *  All equality comparisons involving PrimitiveView instances simply cast all
 *  PrimitiveView instances to Primitive instances and then defer to Primitive's
 *  operator==.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The primitive on the left of the operator.
 * @param[in] rhs The primitive on the right of the operator.
 *
 * @return True if @p lhs aliases a primitive that has the same state as the
 *         primitive @p rhs aliases and false otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator==(const PrimitiveView<T>& lhs,
                const PrimitiveView<U>& rhs) noexcept {
    using value_type = typename PrimitiveView<T>::value_type;
    return static_cast<const Primitive<value_type>&>(lhs) == rhs;
}

/** @brief Overloads Primitive<T>::operator!= for PrimitiveView instances.
 *
 *  @relates PrimitiveView
 *
 *  All inequality operators involving PrimitiveViews simply negate the
 *  corresponding operator== result.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The primitive on the left of the operator.
 * @param[in] rhs The primitive on the right of the operator.
 *
 * @return False if @p lhs aliases a primitive that has the same state as
 *         @p rhs and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator!=(const PrimitiveView<T>& lhs, const Primitive<U>& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Overloads Primitive<T>::operator!= for PrimitiveView instances.
 *
 *  @relates PrimitiveView
 *
 *  All inequality operators involving PrimitiveViews simply negate the
 *  corresponding operator== result.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The primitive on the left of the operator.
 * @param[in] rhs The primitive on the right of the operator.
 *
 * @return False if @p lhs has the same state as the primitive @p rhs aliases
 *         and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator!=(const Primitive<T>& lhs, const PrimitiveView<U>& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Overloads Primitive<T>::operator!= for PrimitiveView instances.
 *
 *  @relates PrimitiveView
 *
 *  All inequality operators involving PrimitiveViews simply negate the
 *  corresponding operator== result.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The primitive on the left of the operator.
 * @param[in] rhs The primitive on the right of the operator.
 *
 * @return False if @p lhs aliases a primitive that has the same state as the
 *         primitive @p rhs aliases and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator!=(const PrimitiveView<T>& lhs,
                const PrimitiveView<U>& rhs) noexcept {
    return !(lhs == rhs);
}

// ------------------------------------Implemenations--------------------------

template<typename T>
PrimitiveView<T>::PrimitiveView() noexcept :
  PrimitiveView(nullptr, nullptr, nullptr, nullptr, nullptr) {}

template<typename T>
PrimitiveView<T>& PrimitiveView<T>::operator=(
  const PrimitiveView<T>& rhs) noexcept {
    return *this = PrimitiveView<T>(rhs);
}

template<typename T>
typename PrimitiveView<T>::const_reference PrimitiveView<T>::coefficient() const
  noexcept {
    return m_pimpl_.coefficient();
}

extern template class PrimitiveView<double>;
extern template class PrimitiveView<const double>;
extern template class PrimitiveView<float>;
extern template class PrimitiveView<const float>;

} // namespace libchemist
