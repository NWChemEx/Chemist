#pragma once
#include "chemist/basis_set/primitive.hpp"
#include "chemist/point/point_view.hpp"

namespace chemist {

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
class PrimitiveView : public PointView<T, Primitive<std::remove_cv_t<T>>> {
private:
    using no_cv_t = std::remove_cv_t<T>;
    using my_type = PrimitiveView<T>;

public:
    /// The non-cv qualified type used to hold the parameters
    using value_type = T;
    /// The type used for indexing and offsets
    using size_type = std::size_t;

    using PointView<T, Primitive<no_cv_t>>::PointView;

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
    decltype(auto) coefficient() noexcept;

    /** @brief Returns the coefficient of the aliased Primitive.
     *
     *  This function can be used to access the coefficient of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's coefficient in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) coefficient() const noexcept;

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
    decltype(auto) exponent() noexcept { return this->pimpl().exponent(); }

    /** @brief Returns the exponent of the aliased Primitive.
     *
     *  This function can be used to access the exponent of the aliased
     *  Primitive in a read-only.
     *
     *  @return The aliased Primitive's exponent in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    decltype(auto) exponent() const noexcept;
};

// ------------------------------------Implemenations--------------------------

template<typename T>
PrimitiveView<T>::PrimitiveView() noexcept :
  PrimitiveView(nullptr, nullptr, nullptr, nullptr, nullptr) {}

template<typename T>
decltype(auto) PrimitiveView<T>::coefficient() noexcept {
    return this->pimpl().coefficient();
}

template<typename T>
decltype(auto) PrimitiveView<T>::coefficient() const noexcept {
    return this->pimpl().coefficient();
}

template<typename T>
decltype(auto) PrimitiveView<T>::exponent() const noexcept {
    return this->pimpl().exponent();
}

extern template class PrimitiveView<double>;
extern template class PrimitiveView<const double>;
extern template class PrimitiveView<float>;
extern template class PrimitiveView<const float>;

} // namespace chemist
