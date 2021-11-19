#pragma once
#include "chemist/basis_set/primitive_view.hpp"
#include "chemist/point/point.hpp"
#include <utilities/containers/indexable_container_base.hpp>
#include <vector>

namespace chemist {
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
class ContractedGaussian
  : public Point<T>,
    public utilities::IndexableContainerBase<ContractedGaussian<T>> {
private:
    /// The type of this class
    using my_type = ContractedGaussian<T>;
    /// the type of the IndexableContainerBase
    using container_base = utilities::IndexableContainerBase<my_type>;
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
    using value_type = Primitive<T>;

    /// Type of an index/offset
    using size_type = typename container_base::size_type;

    /// Type of a read-/write-able view to a Primitive
    using reference = PrimitiveView<T>;
    /// Type of a read-only view to a Primitive
    using const_reference = PrimitiveView<const T>;

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

    /** @brief Serialize ContractedGaussian instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& this->size() & this->coord(0) & this->coord(1) & this->coord(2);
        for(const auto& p : *this) { ar& p.coefficient() & p.exponent(); }
    }

    /** @brief Deserialize for ContractedGaussian instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type mysize;
        T myx, myy, myz;
        ar& mysize& myx& myy& myz;
        std::vector<T> cs(mysize, 0);
        std::vector<T> es(mysize, 0);
        for(int i = 0; i < mysize; ++i) { ar& cs[i] & es[i]; }
        m_pimpl_.reset(new pimpl_type(cs, es));
        this->coord(0) = myx;
        this->coord(1) = myy;
        this->coord(2) = myz;
    }

private:
    /// Allows the IndexableContainerBase to access the implementations
    friend container_base;
    /// Implements size()
    size_type size_() const noexcept;
    /// Implements operator[]
    reference at_(size_type index);
    /// Impelments operator[]const
    const_reference at_(size_type index) const;

    /// The instance actually implementing the class
    std::unique_ptr<pimpl_type> m_pimpl_;
}; // End class contracted_gaussian

/** @brief Compares two ContractedGaussian instances for equality.
 *
 *  Two ContractedGaussian instances are considered equal if they contain the
 *  same number of primitives and if the i-th primitive of @p lhs is equal to
 *  the i-th primitive of @p rhs for all i in the range `[0, lhs.size())`.
 *
 *  @tparam T The type used to hold the ContractedGaussian's parameters.
 *
 *  @param[in] lhs The instance on the left side of the operator.
 *  @param[in] rhs The instance on the right side of the operator.
 *
 *  @return True if the the two instances are equal and false otherwise.
 *
 *  @throw std::bad_alloc if there is insufficient memory to make the PIMPL for
 *         one of the PrimitiveView instances. Strong throw guarantee.
 */
template<typename T>
bool operator==(const ContractedGaussian<T>& lhs,
                const ContractedGaussian<T>& rhs) {
    using base_t = utilities::IndexableContainerBase<ContractedGaussian<T>>;
    return static_cast<const base_t&>(lhs) == static_cast<const base_t&>(rhs);
}

/** @brief Determines if two ContractedGaussian instances are different.
 *
 *  Two ContractedGaussian instances are considered equal if they contain the
 *  same number of primitives and if the i-th primitive of @p lhs is equal to
 *  the i-th primitive of @p rhs for all i in the range `[0, lhs.size())`.
 *  Different is defined as not equal.
 *
 *  @tparam T The type used to hold the ContractedGaussian's parameters.
 *
 *  @param[in] lhs The instance on the left side of the operator.
 *  @param[in] rhs The instance on the right side of the operator.
 *
 *  @return False if the the two instances are equal and true otherwise.
 *
 *  @throw std::bad_alloc if there is insufficient memory to make the PIMPL for
 *         one of the PrimitiveView instances. Strong throw guarantee.
 */
template<typename T>
bool operator!=(const ContractedGaussian<T>& lhs,
                const ContractedGaussian<T>& rhs) {
    return !(lhs == rhs);
}

extern template class ContractedGaussian<double>;
extern template class ContractedGaussian<float>;

} // namespace chemist
