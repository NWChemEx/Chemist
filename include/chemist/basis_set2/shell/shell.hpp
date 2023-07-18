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
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_traits.hpp>
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_view.hpp>
#include <chemist/enums.hpp>

// TODO: Add AO classes and make Shell a container.
// #include <utilities/containers/indexable_container_base.hpp>

namespace chemist::basis_set {
namespace detail_ {
template<typename CGType>
class ShellPIMPL;
} // namespace detail_

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

    /// Types need for working with ContractedGaussians
    using cg_traits = ContractedGaussianTraits<CGType>;

public:
    /// Type of the PIMPL implementing the shell part of the class
    using pimpl_type = detail_::ShellPIMPL<CGType>;

    /// Type of a pointer to this class's PIMPL
    using pimpl_ptr = std::unique_ptr<pimpl_type>;

    /// The type of the contracted Gaussian function *this uses
    using value_type = CGType;

    /// Type of a read/write reference to the contracted Gaussian function
    using reference = ContractedGaussianView<value_type>;

    /// Type of a read-only reference to the contracted Gaussian function
    using const_reference = ContractedGaussianView<const value_type>;

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

    // -------------------------------------------------------------------------
    // -- Ctors/assignment/dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a null Shell instance.
     *
     *  The Shell instance resulting from this ctor
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        PIMPL. Strong throw guarantee.
     */
    Shell() noexcept;

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
    Shell(const my_type& rhs);

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
    Shell(my_type&& rhs) noexcept;

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
    my_type& operator=(const my_type& rhs);

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
    my_type& operator=(my_type&& rhs) noexcept;

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
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    Shell(pure_type pure, angular_momentum_type l, CoefBeginItr&& cbegin,
          CoefEndItr&& cend, ExpBeginItr&& ebegin, ExpEndItr&& eend,
          typename cg_traits::coord_type x, typename cg_traits::coord_type y,
          typename cg_traits::coord_type z);

    /** @brief Creates a new Shell with the provided state.
     *
     *
     *  @param[in] pure True if the shell is a pure shell and false if it is
     *                  Cartesian.
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] coefs The contraction coefficients for the CGTO common to all
     *                   AOs in the shell.
     *  @param[in] exps The exponents of each primitive Gaussian in the common
     *                  CGTO.
     *  @param[in] center The point in Cartesian space where *this will be
     *                    centered.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    Shell(pure_type pure, angular_momentum_type l, CoefBeginItr&& cbegin,
          CoefEndItr&& cend, ExpBeginItr&& ebegin, ExpEndItr&& eend,
          typename cg_traits::center_type center);

    /** @brief Creates a new Shell with the provided state.
     *
     *
     *  @param[in] pure True if the shell is a pure shell and false if it is
     *                  Cartesian.
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] cg The contracted Gaussian function common to all AOs in
     *                this shell.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL. Strong throw guarantee.
     */
    Shell(pure_type pure, angular_momentum_type l, value_type cg);

    /// Default, no-throw dtor
    ~Shell() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters/setters
    // -------------------------------------------------------------------------

    /** @brief Function for determining/setting if the shell is pure.
     *
     *  This member function can be used to determine if the present instance is
     *  a pure shell (eigenfunction of L^2) or if it is a Cartesian shell.
     *  Alternatively this function can be used to modify whether or not the
     *  shell is pure. If *this is a null shell, this method will allocate a
     *  new PIMPL and return the purity of that PIMPL (which is pure by
     *  default).
     *
     *  @return True if the shell is pure and false if the shell is Cartesian.
     *
     *  @throw std::bad_alloc if *this is a null shell and allocating a PIMPL
     *                        fails. Strong throw guarantee.
     */
    pure_reference pure();

    /** @brief Function for determining if the shell is pure.
     *
     *  This member function can be used to determine if the present instance is
     *  a pure shell (eigenfunction of L^2) or if it is a Cartesian shell.
     *
     *  @return True if the shell is pure and false if the shell is Cartesian.
     *
     *  @throw std::runtime_error if *this is a null shell. Strong throw
     *                            guarantee.
     */
    const_pure_reference pure() const;

    /** @brief Determines/sets the total angular momentum of the shell.
     *
     *  This function is used to set and/or inspect the total angular momentum
     *  of the shell. If *this is a null shell this function will allocate a
     *  new PIMPL and then return a reference to the angular momentum in that
     *  PIMPL (which defaults to 0).
     *
     *  @return A reference to the total angular momentum of the shell.
     *
     *  @throw std::bad_alloc if *this is a null shell and allocating the PIMPL
     *                        fails. Strong throw guarantee.
     */
    angular_momentum_reference l();

    /** @brief Determines the total angular momentum of the shell.
     *
     *  This function is used to inspect the total angular momentum of the
     *  shell.
     *
     *  @return The total angular momentum of the shell in a read-only format.
     *
     *  @throw std::runtime_error if *this is a null shell. Strong throw
     *                            guarantee.
     */
    const_angular_momentum_reference l() const;

    /** @brief The ContractedGaussian *this is based on.
     *
     *  The Shell class assumes the use of a segmented contraction. Thus each
     *  AO in *this is based off of a single contracted Gaussian. This method
     *  can be used to retrieve that contracted Gaussian. If *this is a null
     *  shell, this method will allocate a new PIMPL and return a reference to
     *  the contracted Gaussian in that PIMPL (which will be a null contracted
     *  Gaussian).
     *
     *  @return A mutable reference to the ContractedGaussian in *this.
     *
     *  @throw std::bad_alloc if *this is a null shell and allocating the PIMPL
     *                        fails. Strong throw guarantee.
     */
    reference contracted_gaussian();

    /** @brief Accesses the ContractedGaussian *this is defined in terms of.
     *
     *  This method behaves identical to the non-const version except that
     *  the resulting reference is read-only.
     *
     *  @return A read-only reference to the ContractedGaussian *this is
     *          defined in terms of.
     *
     *  @throw std::runtime_error if *this is a null shell. Strong throw
     *                            guarantee.
     */
    const_reference contracted_gaussian() const;

    /** @brief Returns the number of primitives in the contracted Gaussian.
     *
     *  Each AO in this shell is defined in terms of a single contracted
     *  Gaussian function. This method can be used to determine the number of
     *  primitives in that contracted Gaussian function. This a convenience
     *  function for calling `contracted_gaussian().size()`. If *this is a
     *  null shell, this method will return 0.
     *
     *  @return The number of primitives in the contracted Gaussian function.
     *
     *  @throw None No throw guarantee
     *
     *  Complexity: constant
     */
    size_type n_primitives() const noexcept;

    /** @brief Returns the @p i-th primitive in the contracted Gaussian.
     *
     *  This is a convenience function for calling
     *  `contracted_gaussian().at(i)`.
     *
     *  @param[in] i The index of the requested unique primitive. Must be in the
     *               range [0, n_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive.
     *
     *  @throw std::out_of_range if @p i is not in the range [0,
     *                           n_unique_primitives()). Strong throw guarantee.
     *
     *  Complexity: Constant.
     */
    typename cg_traits::primitive_reference primitive(size_type i);

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
    typename cg_traits::const_primitive_reference primitive(size_type i) const;

    /** @brief The number of AOs in *this.
     *
     *  A Shell object implicitly stores the AO objects which make it up. This
     *  method is used to retrieve the number of such AOs.
     *
     *  @return The number of AOs in *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept;

    // -------------------------------------------------------------------------
    // -- Utility functions
    // -------------------------------------------------------------------------

    /** @brief Exchanges the contents of *this with @p other.
     *
     *  @param[in,out] other The instance to exchange state with. After this
     *                       call @p other will contain the state which was
     *                       previously in *this and *this will contain the
     *                       state which was previously in @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(Shell& other) noexcept;

    /** @brief Is *this a null shell?
     *
     *  Null shells are Shell objects which have been default constructed and/or
     *  have been moved from. This method can be used to determine if *this is
     *  a null shell.
     *
     *  @return True if *this is a null shell and false otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool is_null() const noexcept;

    /** @brief Determines if *this and @p rhs are value equal
     *
     *  This function is used to compare two Shell instances for equality. Two
     *  Shell instances are equal if they have the same purity, total angular
     *  momentum, and the same contracted gaussian.
     *
     *  @param[in] rhs The instance being compared to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *  @throw none No throw guarantee.
     */

    bool operator==(const Shell& rhs) const noexcept;

private:
    /** @brief Creates a new Shell instance with the provided PIMPLs.
     *
     *  This function is primarily intended for use by the AtomicBasisSetPIMPL
     * class to create ShellView instances.
     *
     * @param[in] my_pimpl The PIMPL implementing the non-Point part of the
     *                     instance.
     *
     * @thrown None No throw guarantee
     */
    Shell(pimpl_ptr my_pimpl) noexcept;

    /// True if *this has a PIMPL and false otherwise
    bool has_pimpl_() const noexcept;

    /// raises std::runtime_error if *this has no PIMPL
    void assert_pimpl_() const;

    /// Allows the IndexableContainerBase to access the implementations
    // friend container_base;

    /// Implements size()
    size_type size_() const noexcept;

    /// Implements non-const version of operator[]/at
    // contracted_gaussian_reference at_(size_type index);

    /// Implements const-version of operator[]/at
    // const_cg_reference at_(size_type index) const;

    /// The instance that actually implements this class
    pimpl_ptr m_pimpl_;
}; // End class Shell

// -- Inline implementations

template<typename CGType>
template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
         typename ExpEndItr>
Shell<CGType>::Shell(pure_type pure, angular_momentum_type l,
                     CoefBeginItr&& cbegin, CoefEndItr&& cend,
                     ExpBeginItr&& ebegin, ExpEndItr&& eend,
                     typename cg_traits::coord_type x,
                     typename cg_traits::coord_type y,
                     typename cg_traits::coord_type z) :
  Shell(pure, l, std::forward<CoefBeginItr>(cbegin),
        std::forward<CoefEndItr>(cend), std::forward<ExpBeginItr>(ebegin),
        std::forward<ExpEndItr>(eend),
        typename cg_traits::center_type(x, y, z)) {}

template<typename CGType>
template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
         typename ExpEndItr>
Shell<CGType>::Shell(pure_type pure, angular_momentum_type l,
                     CoefBeginItr&& cbegin, CoefEndItr&& cend,
                     ExpBeginItr&& ebegin, ExpEndItr&& eend,
                     typename cg_traits::center_type center) :
  Shell(pure, l,
        value_type(std::forward<CoefBeginItr>(cbegin),
                   std::forward<CoefEndItr>(cend),
                   std::forward<ExpBeginItr>(ebegin),
                   std::forward<ExpEndItr>(eend), std::move(center))) {}

/** @brief Determines if two Shell instances are different.
 *
 *  @relates Shell
 *
 *  This function is used to compare two Shell instances for inequality. It
 *  simply negates the result of operator==.
 *
 *  @tparam CGType The type of the contracted Gaussian function
 *
 *  @param[in] lhs The Shell instance on the left side of the comparison
 *                 operator.
 *  @param[in] rhs The Shell instance on the right side of the comparison
 *                 operator.
 *  @return False if the two instances are equal and true otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename CGType>
bool operator!=(const Shell<CGType>& lhs, const Shell<CGType>& rhs) {
    return !(lhs == rhs);
}

/// Shell where all parameters use doubles
using ShellD = Shell<ContractedGaussianD>;

/// Shell where all parameters use floats
using ShellF = Shell<ContractedGaussianF>;

extern template class Shell<ContractedGaussianD>;
extern template class Shell<ContractedGaussianF>;

} // namespace chemist::basis_set