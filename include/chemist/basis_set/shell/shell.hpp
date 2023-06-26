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
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian.hpp>
#include <chemist/enums.hpp>

// TODO: Add AO classes and make Shell a container.
// #include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
template<typename CGType>
class ShellPIMPL;
}

/** @brief Models a set of AOs with a common center, the same total angular
 *         momentum, and the same contracted Gaussian function.
 *
 *  Each Cartesian AO is made up of the product of a contracted Gaussian
 *  function  and a term of the
 *  form: @f$x^ay^bz^c@f$ such that @f$a+b+c = \ell@f$ (with @f$\ell@f$ being
 *  the total angular momentum of the AO. Spherical AOs are then formed by
 *  taking linear combinations of Cartesian AOs (the coefficients of the
 *  transformation being given by the spherical transform). Historically, in
 *  electronic structure theory we implicitly store much of this information in
 *  what is called a shell. A shell is comprised of a contracted Gaussian
 *  function, the total angular momentum, and
 *  whether the AOs in the shell are Cartesian or spherical (often als called
 *  pure). All other information is stored implicitly. At present, this class
 *  adheres to this convention.
 *
 *  @tparam CGType The type of the contracted Gaussian common to all AOs in the
 *                 shell.
 */
template<typename CGType>
class Shell {
private:
    /// Type of this instance
    using my_type = Shell<CGType>;

    /// Type of the IndexableContainerBase base class
    // using container_base = utilities::IndexableContainerBase<my_type>;

public:
    /// Type of the PIMPL implementing the shell part of the class
    using pimpl_t = detail_::ShellPIMPL<T>;

    /// Type of a pointer to this class's PIMPL
    using pimpl_ptr_t = std::unique_ptr<pimpl_t>;

    /// For all intents and purposes the type of CGTOs comprising the shell
    using contracted_gaussian_type = CGType;

    /// Type of a read/write reference to an AO
    using contracted_gaussian_reference =
      ContractedGaussianView<contracted_gaussian_type>;

    /// Type of a read-only reference to an AO
    using const_cg_reference =
      ContractedGaussianView<const contracted_gaussian_type>;

    /// Unsigned integral type used for indexing and offsets
    using size_type = std::size_t;

    /// Unsigned integral type used to represent the total angular momentum
    using angular_momentum_type = size_type;

    /// Type of a mutable reference to the angular momentum
    using angular_momentum_reference = angular_momentum_type&;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference = const angular_momentum_type&;

    /// Type used to determine if a shell is pure or not
    using pure_type = ShellType;

    /// Reference to a mutable object of type pure_type
    using pure_reference = pure_type&;

    /// Reference to a read-only object of type pure_type
    using const_pure_reference = const pure_type&;

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
    Shell(ShellType pure, angular_momentum_type l, std::vector<T> coefs,
          std::vector<T> exps, T x, T y, T z);

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
    pure_reference pure() noexcept;

    /** @brief Function for determining if the shell is pure.
     *
     *  This member function can be used to determine if the present instance is
     *  a pure shell (eigenfunction of L^2) or if it is a Cartesian shell.
     *
     *  @return True if the shell is pure and false if the shell is Cartesian.
     *
     *  @throw none No throw guarantee.
     */
    const_pure_reference pure() const noexcept;

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
    angular_momentum_reference l() noexcept;

    /** @brief Determines the total angular momentum of the shell.
     *
     *  This function is used to inspect the total angular momentum of the
     *  shell.
     *
     *  @return The total angular momentum of the shell in a read-only format.
     *
     *  @throw none No throw guarantee.
     */
    const_angular_momentum_reference l() const noexcept;

    /** @brief The number of AOs in *this.
     *
     *  A Shell object implicitly stores the AO objects which make it up. This
     *  method is used to retrieve the number of such AOs.
     *
     *  @return The number of AOs in *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return size_(); }

    /** @brief The ContractedGauassian *this is based on.
     *
     *  The Shell class assumes the use of a segmented contraction. Thus each
     *  AO in *this is based off of a single contracted Gaussian. This method
     *  can be used to retrieve that contracted Gaussian.
     *
     *  @return A mutable reference to the ContractedGaussian implementing
     *          *this.
     */
    contracted_gaussian_reference contracted_gaussian() { return at_(0); }

    /** @brief Accesses the ContractedGaussian used to define *this.
     *
     *  This method behaves identical to the non-const version except that
     *  the resulting reference is read-only.
     *
     *  @return A read-only reference to the ContractedGaussian *this is
     *          defined in terms of.
     */
    const_cg_reference contracted_gaussian() const { return at_(0); }

    /// Type of a Primitive
    using primitive_type = typename contracted_gaussian_type::value_type;

    /// Type of a read/write reference to a primitive
    using primitive_reference = typename contracted_gaussian_type::reference;

    /// Type of a read-only reference to a primitive
    using const_primitive_reference =
      typename contracted_gaussian_type::const_reference;

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
    // friend container_base;
    /// Implements size()
    size_type size_() const noexcept;
    /// Implements operator[]
    contracted_gaussian_reference at_(size_type index);
    /// Implements operator[]const
    const_cg_reference at_(size_type index) const;
    /// The instance that actually implements this class
    pimpl_ptr_t m_pimpl_;
}; // End class Shell

/** @brief Determines if two Shell instances are the same.
 *
 *  @relates Shell
 *
 *  This function is used to compare two Shell instances for equality. Two
 *  Shell instances are equal if they have the same purity, total angular
 *  momentum, and the same contracted gaussian.
 *
 *  @tparam T The type used for the parameters of the contracted Gaussian.
 *  @param[in] lhs The Shell instance on the left side of the comparison
 *                 operator.
 *  @param[in] rhs The Shell instance on the right side of the comparison
 *                 operator.
 *  @return True if the two instances are equal and false otherwise.
 *  @throw none No throw guarantee.
 */
template<typename T>
bool operator==(const Shell<T>& lhs, const Shell<T>& rhs) noexcept {
    // using base_t = utilities::IndexableContainerBase<Shell<T>>;
    auto lhs_state = std::tie(lhs.l(), lhs.pure(), lhs.contracted_gaussian());
    auto rhs_state = std::tie(rhs.l(), rhs.pure(), rhs.contracted_gaussian());

    return lhs_state == rhs_state;
    // return static_cast<const base_t&>(lhs) == static_cast<const
    // base_t&>(rhs);
}

/** @brief Determines if two Shell instances are different.
 *
 *  @relates Shell
 *
 *  This function is used to compare two Shell instances for inequality. It
 *  simply negates the result of operator==.
 *
 *  @tparam T The type used for the contracted Gaussian's parameters.
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
