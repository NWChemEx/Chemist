#pragma once
#include "libchemist/basis_set/primitive/primitive_view.hpp"
#include "libchemist/point/point.hpp"
#include <utilities/iterators/offset_iterator.hpp>
#include <vector>

namespace libchemist {
namespace detail_ {
template<typename T>
class CGTOPIMPL;
} // namespace detail_

/** @brief An atomic orbital resulting from a linear combination of Gaussian
 *         primitives.
 *
 *  A contracted Gaussian type orbital formed from @f$N@f$ primitive
 *  gaussians,
 *  @f$\chi(\vec{r};\vec{c}, \vec{\alpha})@f$, is given by:
 *
 *  @f{
 *  \chi(\vec{r};\vec{c}, \vec{\alpha}) = \sum_{i=1}^N g(\vec{r};c_i, \alpha_i)
 *   *  \chi(\vec{r};\vec{c}, \vec{\alpha}) = \sum_{i=1}^N c_ie^{-\alpha_i r^2}

 *  @f}.
 *  This class models the resulting orbital by value, meaning instances of this
 *  class own the memory associated with their parameters.
 *
 * @tparam T The non-cv qualified type of the primitive's parameters.
 */
template<typename T>
class ContractedGaussian : public Point<T> {
private:
    /// The type of this class
    using my_type = ContractedGaussian<T>;
    /// The type of the PIMPL implementing this class
    using pimpl_type = detail_::CGTOPIMPL<T>;
    /// The type of a pointer to this class's PIMPL
    using pimpl_ptr = std::unique_ptr<pimpl_type>;
    /// The type of the PIMPL implementing the base class
    using point_pimpl_type = detail_::PointPIMPL<T>;
    /// The type of a pointer to the base class's PIMPL.
    using point_pimpl_ptr = std::unique_ptr<point_pimpl_type>;

public:
    /// Type of the parameters in this contraction
    using value_type = T;
    /// Type of a reference to a Primitive stored in this container
    using reference = PrimitiveView<T>;
    /// Type of a read-only reference to a Primitive stored in this container
    using const_reference = PrimitiveView<const T>;
    /// Type used for indexing and offsets
    using size_type = std::size_t;
    /// Type of a read/write iterator over this container
    using iterator = utilities::iterators::OffsetIterator<my_type>;
    /// Type of a read-only iterator over this container
    using const_iterator = utilities::iterators::OffsetIterator<const my_type>;

    /** @brief Makes a ContractedGaussian with no Primitives.
     *
     *  The default ctor makes a ContractedGaussian instance which contains no
     *  Primitives. At the moment it is not possible to add Primitives to the
     *  resulting instance aside from using copy/move assignment.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    ContractedGaussian();

    /** @brief Constructs a ContractedGaussian from the provided parameters.
     *
     *  This ctor is largely intended for constructing a ContractedGaussian when
     *  the parameters are not already stored in Primitives.
     *
     *  @param[in] coefs  An @f$N@f$ element array such that the @f$i@f$-th
     *                    element is the contraction coefficient for the
     *                    @f$i@f$-th primitive.
     *  @param[in] exps An @f$N@f$ element array such that the @f$i@f$-th
     *                  element is the exponent for the @f$i@f$-th primitive.
     *  @param[in] x The x-coordinate where this orbital is centered.
     *  @param[in] y The y-coordinate where this orbital is centered.
     *  @param[in] z The z-coordinate where this orbital is centered.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPLs. Strong throw guarantee.
     */
    ContractedGaussian(std::vector<T> coefs, std::vector<T> exps, T x, T y,
                       T z);

    /** @brief Constructs a new ContractedGaussian instance by wrapping the
     *         provided PIMPLs.
     *
     *  This ctor gives you the ability to micro-manage how the
     *  ContractedGaussian works. This ctor is mainly intended for use by the
     *  ContractedGaussianView class.
     *
     *  @param[in] my_pimpl The PIMPL which stores the coefficients and
     *                      exponents for the ContractedGaussian class.
     *  @param[in] point_pimpl The PIMPL which stores the Cartesian coordinates
     *                         for the base Point<T> class.
     *  @throw none No throw guarantee.
     */
    ContractedGaussian(pimpl_ptr my_pimpl,
                       point_pimpl_ptr point_pimpl) noexcept;

    /** @brief Constructs a new ContractedGaussian instance by copying the state
     *         of @p rhs.
     *
     *  This ctor is used to initialize a ContractedGaussian with a deep copy
     *  of another instance's state.
     *
     *  @param[in] rhs The ContractedGaussian instance whose state is being
     *                 copied.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPLs. Strong throw guarantee.
     */
    ContractedGaussian(const ContractedGaussian<T>& rhs);

    /** @brief Transfers another ContractedGaussian's state into a new instance.
     *
     *  This ctor is used to create a new ContractedGaussian by taking ownership
     *  of another instance's state. After this function is called, all
     *  references to @p rhs's state remain valid with the caveat that they now
     *  reside inside the new instance.
     *
     *  @param[in, out] rhs The instance whose state is being transferred. After
     *                  this operation @p rhs has no PIMPL and is thus in an
     *                  invalid state. @p rhs can be returned to a valid state
     *                  by move assignment.
     *
     *  @throw none No throw guarantee.
     */
    ContractedGaussian(ContractedGaussian<T>&& rhs) noexcept;

    /** @brief Replaces the current ContractedGaussian instance's state with a
     *         deep copy of @p rhs's state.
     *
     *  This function is used to change the current ContractedGaussian's state
     *  to a deep copy of another instance's state. After this operation all
     *  references to the current instance's state are invalidated.
     *
     *  @param[in] rhs The ContractedGaussian instance whose state is being
     *                 copied.
     *
     *  @return The current instance with its state changed to a deep copy of
     *          @p rhs's state.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPLs. Strong throw guarantee.
     */
    ContractedGaussian<T>& operator=(const ContractedGaussian<T>& rhs);

    /** @brief Transfers another ContractedGaussian's state into this instance.
     *
     *  This function is used to create a new ContractedGaussian by transferring
     *  ownership of another instance's state into the current instance. After
     *  this function is called, all references to @p rhs's state remain valid
     *  with the caveat that they now reside inside this instance. References to
     *  this instance's previous state are invalidated.
     *
     *  @param[in, out] rhs The instance whose state is being transferred. After
     *                  this operation @p rhs has no PIMPL and is thus in an
     *                  invalid state. @p rhs can be returned to a valid state
     *                  by move assignment.
     *
     *  @return The current instance after taking ownership of @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    ContractedGaussian<T>& operator=(ContractedGaussian<T>&& rhs) noexcept;

    /// Default dtor
    ~ContractedGaussian() noexcept override;

    /** @brief The number of primitives in this orbital.
     *
     *  This function computes the number of primitives that are in this
     *  orbital.
     *
     *  @return The number of primitives that comprise this orbital.
     *
     *  @throw none No throw guarantee.
     */
    size_type size() const noexcept;

    /** @brief Returns the @p i-th primitive contributing to the orbital.
     *
     *  This function is used to retrieve the @p i -th primitive that is
     *  involved in the contraction forming the present orbital. The order of
     *  the Primitives is the order in which they were inserted into the
     *  present instance.
     *
     *  @param[in] i The index of the requested primitive. Must be in the range
     *             [0, size()).
     *
     *  @return A read/writeable view of the requested primitive.
     *
     *  @throw std::out_of_range if the requested index is not in the range
     *                           [0, size()). Strong throw guarantee.
     */
    reference operator[](size_type i);

    /** @brief Returns the @p i-th primitive contributing to the orbital.
     *
     *  This function is used to retrieve the @p i -th primitive that is
     *  involved in the contraction forming the present orbital. The order of
     *  the Primitives is the order in which they were inserted into the
     *  present instance.
     *
     *  @param[in] i The index of the requested primitive. Must be in the range
     *             [0, size()).
     *
     *  @return A read-only view of the requested primitive.
     *
     *  @throw std::out_of_range if the requested index is not in the range
     *                           [0, size()). Strong throw guarantee.
     */
    const_reference operator[](size_type i) const;

    iterator begin() noexcept { return {0, this}; }
    const_iterator begin() const noexcept { return {0, this}; }
    const_iterator cbegin() const noexcept { return {0, this}; }

    iterator end() noexcept { return {size(), this}; }
    const_iterator end() const noexcept { return {size(), this}; }
    const_iterator cend() const noexcept { return {size(), this}; }

private:
    /// The instance actually implementing the class
    std::unique_ptr<pimpl_type> m_pimpl_;
}; // End class contracted_gaussian

extern template class ContractedGaussian<double>;
extern template class ContractedGaussian<float>;

} // namespace libchemist
