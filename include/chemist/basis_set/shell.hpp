#pragma once
#include "chemist/basis_set/contracted_gaussian_view.hpp"
#include "chemist/enums.hpp"
#include "chemist/point/point.hpp"
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename T>
class ShellPIMPL;
}

/** @brief Models a set of AOs with a common center, total angular momentum, and
 *         CGTOs.
 *
 *  Each Cartesian AO is made up of the product of a CGTO and a term of the
 *  form: @f$x^ay^bz^c@f$ such that @f$a+b+c = \ell@f$ (with @f$\ell@f$ being
 *  the total angular momentum of the AO. Spherical AOs are then formed by
 *  taking linear combinations of Cartesian AOs (the coefficients of the
 *  transformation being given by the spherical transform). Histroically, in
 *  electronic structure theory we implicitly store much of this information in
 *  what is called a shell. A shell is comprised of a CGTO, @f$\ell@f$, and
 *  whether the AOs in the shell are Cartesian or spherical (often called pure).
 *  All other information is stored implicitly. This class adheres to this
 *  convention.
 *
 *  @tparam T The type used to store parameters associated with the CGTO.
 *            Expected to be a non-cv qualified POD floating-point type such as
 *            `double` or `float`.
 */
template<typename T>
class Shell : public Point<T>,
              public utilities::IndexableContainerBase<Shell<T>> {
private:
    /// Type of this instance
    using my_type = Shell<T>;
    /// Type of the IndexableContainerBase base class
    using container_base = utilities::IndexableContainerBase<my_type>;
    /// Type of the PIMPL implementing the shell part of the class
    using pimpl_t = detail_::ShellPIMPL<T>;
    /// Type of a pointer to this class's PIMPL
    using pimpl_ptr_t = std::unique_ptr<pimpl_t>;
    /// Type of the PIMPL used by the Point<T> class
    using point_pimpl_t = detail_::PointPIMPL<T>;
    /// Type of a pointer to the Point's PIMPL
    using point_pimpl_ptr_t = std::unique_ptr<point_pimpl_t>;

public:
    /// For all intents and purposes the type of CGTOs comprising the shell
    using value_type = ContractedGaussian<T>;

    /// Type of a read/write reference to an AO
    using reference = ContractedGaussianView<T>;

    /// Type of a read-only reference to an AO
    using const_reference = ContractedGaussianView<const T>;

    /// Unsigned integral type used for indexing and offsets
    using size_type = typename container_base::size_type;

    /// Type used to determine if a shell is pure or not
    using pure_type = ShellType;

    /** @brief Creates a new Shell instance.
     *
     *  The Shell instance resulting from this ctor has all internal state
     *  default initialized. In particular this means that the resulting shell
     *  is Cartesian, has a total angular momentum of 0, and has a common
     *  CGTO comprised of no primitives.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        PIMPL. Strong throw guarantee.
     */
    Shell();

    /** @brief Makes a new Shell by deep copying another instance.
     *
     *  This ctor can be used to create a new Shell instance whose state is
     *  obtained by deep copying the state of another Shell instance.
     *
     *  @param[in] rhs The instance whose state is being copied.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs's
     *                        PIMPL. Strong throw guarantee.
     */
    Shell(const Shell<T>& rhs);

    /** @brief Creates a new Shell instance by taking ownership of another Shell
     *         instance's state.
     *
     *  This ctor is used to create a new Shell instance by transferring the
     *  state of another Shell instance to the newly created instance.
     *
     *  @param[in,out] rhs The instance whose state is being transferred to the
     *                     new Shell instance. After the operation @p rhs will
     *                     not contain a PIMPL and thus will not be usable.
     *                     @p rhs can be returned to a valid state by copy/move
     *                     assignment from a valid Shell instance.
     *
     * @throw none No throw guarantee.
     */
    Shell(Shell<T>&& rhs) noexcept;

    /** @brief Converts the current instance's state into a deep copy of another
     *         instance's state.
     *
     *  This function can be used to switch the current instance's state to a
     *  deep copy of @p rhs's state. After the operation this instance's
     *  previous state will be freed and all pointers and references to the
     *  previous state will be invalidated.
     *
     * @param[in] rhs The instance whose state is being copied.
     *
     * @return The current instance after exchanging its state for a deep copy
     *         of @p rhs's state.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the state
     *                       of @p rhs. Strong throw guarantee.
     */
    Shell<T>& operator=(const Shell<T>& rhs);

    /** @brief Transfers another Shell's state to this instance.
     *
     *  This function can be used to transfer ownership of another Shell's state
     *  to the present instance.
     *
     *  @param[in,out] rhs The instance whose state is being transferred to the
     *                     present instance. After the operation @p rhs will not
     *                     contain a PIMPL and will thus not be usable until a
     *                     new PIMPL is assigned to it via move/copy assignment.
     *
     *  @return The current instance after taking ownership of @p rhs's state.
     */
    Shell<T>& operator=(Shell<T>&& rhs) noexcept;

    /** @brief Creates a new Shell with the provided state.
     *
     *  This is the ctor that will be primarily used to create new Shell
     *  instances. Its arguments are the state of the Shell to create.
     *
     *  @param[in] pure True if the shell is a pure shell and false if it is
     *                  Cartesian.
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] coefs The contraction coefficients for the CGTO common to all
     *                   AOs in the shell.
     *  @param[in] exps The exponents of each primitive Gaussian in the common
     *                  CGTO.
     *  @param[in] x The x coordinate this shell is centered on.
     *  @param[in] y The y coordinate this shell is centered on.
     *  @param[in] z The z coordinate this shell is centered on.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    Shell(ShellType pure, int l, std::vector<T> coefs, std::vector<T> exps, T x,
          T y, T z);

    /** @brief Creates a new Shell instance with the provided PIMPLs.
     *
     *  This function is primarily intended for use by the AtomicBasisSetPIMPL
     * class to create ShellView instances.
     *
     * @param[in] my_pimpl The PIMPL implementing the non-Point part of the
     *                     instance.
     * @param[in] base_pimpl The PIMPL implementing the Point part of the
     *                       instance.
     */
    Shell(pimpl_ptr_t my_pimpl, point_pimpl_ptr_t base_pimpl);

    /// Default, no-throw dtor
    ~Shell() noexcept override;

    /** @brief Function for determining/setting if the shell is pure.
     *
     *  This member function can be used to determine if the present instance is
     *  a pure shell (eigenfunction of L^2) or if it is a Cartesian shell.
     *  Alternatively this function can be used to modify whether or not the
     *  shell is pure.
     *
     *  @return True if the shell is pure and false if the shell is Cartesian.
     *
     *  @throw none No throw guarantee.
     */
    pure_type& pure() noexcept;

    /** @brief Function for determining if the shell is pure.
     *
     *  This member function can be used to determine if the present instance is
     *  a pure shell (eigenfunction of L^2) or if it is a Cartesian shell.
     *
     *  @return True if the shell is pure and false if the shell is Cartesian.
     *
     *  @throw none No throw guarantee.
     */
    const pure_type& pure() const noexcept;

    /** @brief Determines/sets the total angular momentum of the shell.
     *
     *  This function is used to set and/or inspect the total angular momentum
     *  of the shell.
     *
     *  @return The total angular momentum of the shell in a read-/write-able
     *          format.
     *
     *  @throw none No throw guarantee.
     */
    size_type& l() noexcept;

    /** @brief Determines the total angular momentum of the shell.
     *
     *  This function is used to inspect the total angular momentum of the
     *  shell.
     *
     *  @return The total angular momentum of the shell in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const size_type& l() const noexcept;

    /// Type of a Primitive
    using primitive_type = typename value_type::value_type;

    /// Type of a read/write reference to a primitive
    using primitive_reference = typename value_type::reference;

    /// Type of a read-only reference to a primitive
    using const_primitive_reference = typename value_type::const_reference;

    /** @brief Returns the number of unique primitives in the Shell.
     *
     *  The backend of this class only stores the primitives for one CGTO. This
     *  function can be used to retrieve the number of primitives in that CGTO.
     *
     *  @return The number of primitives in the one unique CGTO.
     *
     *  @throw None.
     *
     *  Complexity: constant
     */
    size_type n_unique_primitives() const noexcept;

    /** @brief Returns the @p i-th unique primitive in this shell.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_unique_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive.
     *
     *  @throw std::out_of_range if @p i is not in the range [0,
     *                           n_unique_primitives()). Strong throw guarantee.
     *
     *  Complexity: Constant.
     */
    primitive_reference unique_primitive(size_type i);

    /** @brief Returns the @p i-th unique primitive in this shell.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_unique_primitives()).
     *
     *  @return A read-only reference to the requested primitive.
     *
     *  @throw std::out_of_range if @p i is not in the range [0,
     *                           n_unique_primitives()). Strong throw guarantee.
     *
     *  Complexity: Constant.
     */
    const_primitive_reference unique_primitive(size_type i) const;

    /** @brief Serialize Shell instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& n_unique_primitives() & bool(pure()) & l() & this->coord(0) &
          this->coord(1) & this->coord(2);
        for(int i = 0; i < n_unique_primitives(); ++i) {
            ar& unique_primitive(i).coefficient() &
              unique_primitive(i).exponent();
        }
    }

    /** @brief Deserialize for Shell instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type n;
        bool ispure;
        ar& n& ispure& l() & this->coord(0) & this->coord(1) & this->coord(2);
        pure() = pure_type(ispure);
        std::vector<T> cs(n, 0);
        std::vector<T> es(n, 0);
        for(int i = 0; i < n; ++i) { ar& cs[i] & es[i]; }
        m_pimpl_.reset(new pimpl_t(pure(), l(), cs, es));
    }

private:
    /// Allows the IndexableContainerBase to access the implementations
    friend container_base;
    /// Implements size()
    size_type size_() const noexcept;
    /// Implements operator[]
    reference at_(size_type index);
    /// Implements operator[]const
    const_reference at_(size_type index) const;
    /// The instance that actually implements this class
    pimpl_ptr_t m_pimpl_;
}; // End class Shell

/** @brief Determines if two Shell instances are the same.
 *
 *  @relates Shell
 *
 *  This function is used to compare two Shell instances for equality. Two
 *  Shell instances are equal if they have the same purity, total angular
 *  momentum, and CGTO.
 *
 *  @tparam T The type used for the CGTO parameters.
 *  @param[in] lhs The Shell instance on the left side of the comparison
 *                 operator.
 *  @param[in] rhs The Shell instance on the right side of the comparison
 *                 operator.
 *  @return True if the two instances are equal and false otherwise.
 *  @throw none No throw guarantee.
 */
template<typename T>
bool operator==(const Shell<T>& lhs, const Shell<T>& rhs) noexcept {
    using base_t = utilities::IndexableContainerBase<Shell<T>>;
    if(std::tie(lhs.l(), lhs.pure()) != std::tie(rhs.l(), rhs.pure()))
        return false;
    return static_cast<const base_t&>(lhs) == static_cast<const base_t&>(rhs);
}

/** @brief Determines if two Shell instances are different.
 *
 *  @relates Shell
 *
 *  This function is used to compare two Shell instances for inequality. Two
 *  Shell instances are equal if they have the same purity, total angular
 *  momentum, and CGTO. Two instances are defined as different if they are not
 *  equal.
 *
 *  @tparam T The type used for the CGTO parameters.
 *  @param[in] lhs The Shell instance on the left side of the comparison
 *                 operator.
 *  @param[in] rhs The Shell instance on the right side of the comparison
 *                 operator.
 *  @return False if the two instances are equal and true otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
bool operator!=(const Shell<T>& lhs, const Shell<T>& rhs) {
    return !(lhs == rhs);
}

extern template class Shell<double>;
extern template class Shell<float>;

} // namespace chemist
