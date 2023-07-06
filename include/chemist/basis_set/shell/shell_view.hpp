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
#include <chemist/basis_set/contracted_gaussian/contracted_gaussian_view.hpp>
#include <chemist/basis_set/shell/shell.hpp>
#include <chemist/detail_/view/traits.hpp>

namespace chemist {
namespace detail_ {

template<typename ShellType>
class ShellViewPIMPL;

}

/** @brief Behaves like a reference to a Shell.
 *
 *  Shell instances own their state. ShellView instances alias their state, but
 *  otherwise have the same API as a Shell instance.
 *
 *  @tparam ShellType The type of Shell this is a view of. @p ShellType is
 *                    expected to be a (possibly) cv-qualified instantiation of
 *                    Shell
 */
template<typename ShellType>
class ShellView {
private:
    /// Works out view types for us
    using traits_type = detail_::ViewTraits<ShellType>;

    /// Typedef so we don't need "typename" and "template" below
    template<typename U>
    using apply_const = typename traits_type::template apply_const<U>;

    template<typename U>
    using apply_const_ref = typename traits_type::template apply_const_ref<U>;

    template<typename U>
    using ptr_type = typename traits_type::template apply_const_ptr<U>;

public:
    /// The type of the PIMPL
    using pimpl_type = detail_::ShellViewPIMPL<ShellType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// The type *this is a view of
    using shell_type = typename traits_type::type;

    /// The type of a mutable reference to a Shell
    using shell_reference = shell_type&;

    /// The type of a read-only reference to a Shell
    using const_shell_reference = const shell_type&;

    /// Unsigned integral type used for indexing and offsets
    using size_type = typename shell_type::size_type;

    /// Type used for storing the angular momentum
    using angular_momentum_type = typename shell_type::angular_momentum_type;

    /// Type of a pointer to the angular momentum
    using angular_momentum_pointer = ptr_type<angular_momentum_type>;

    /// Type of a (possibly) mutable reference to the angular momentum
    using angular_momentum_reference = apply_const_ref<angular_momentum_type>;

    /// Type of a read-only reference to the angular momentum
    using const_angular_momentum_reference =
      typename shell_type::const_angular_momentum_reference;

    /// Type used to return Cartesian vs. spherical
    using pure_type = typename shell_type::pure_type;

    /// Type of a pointer to the purity of the shell
    using pure_pointer = ptr_type<pure_type>;

    /// Type of a (possibly) mutable reference to the purity
    using pure_reference = apply_const_ref<pure_type>;

    /// Type of a read-only reference to the purity
    using const_pure_reference = typename shell_type::const_pure_reference;

    /// The type of the contracted Gaussian common to AOs in *this
    using contracted_gaussian_type =
      typename shell_type::contracted_gaussian_type;

    /// The type of a mutable reference to a contracted Gaussian
    using contracted_gaussian_reference =
      ContractedGaussianView<apply_const<contracted_gaussian_type>>;

    /// The type of a read-only reference to a contracted Gaussian
    using const_cg_reference =
      ContractedGaussianView<const contracted_gaussian_type>;

    /// The types associated with a Contracted Gaussian
    using cg_traits = ContractedGaussianTraits<contracted_gaussian_reference>;

    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a ShellView which is an alias of a null shell.
     *
     *  Null shells have no purity, no angular momentum (different than zero),
     *  and no contracted Gaussian function.
     *
     *  @throw None No throw guarantee.
     */
    ShellView() noexcept;

    /** @brief Creates a ShellView from a Shell.
     *
     *  @note This ctor is intentially not explicit to allow implicit conversion
     *        from a Shell to a ShellView.
     *
     *  @param[in] shell2alias The shell the resulting view will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    ShellView(shell_reference shell2alias);

    /** @brief Creates a ShellView aliasing the provided state.
     *
     *  @param[in] ao_type Whether the shell is spherical or Cartesian.
     *  @param[in] l The total angular momentum of the shell.
     *  @param[in] cg The contracted Gaussian the AOs use.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    ShellView(pure_reference ao_type, angular_momentum_reference l,
              contracted_gaussian_reference cg);

    /** @brief Creates a new view of the same shell.
     *
     *  The copy ctor creates a new ShellView which aliases the same state
     *  as @p other. It is important to note that the resulting instance is a
     *  deep copy in the sense that the new ShellView has its own state, but
     *  that state will start as aliasing the same Shell as @p other.
     *
     *  @param[in] other The view we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL.
     *                        Strong throw guarantee.
     */
    ShellView(const ShellView& other);

    /** @brief Creates a new view by taking the state of @p other.
     *
     *  This method creates a new ShellView by taking the state of @p other.
     *
     *  @param[in,out] other The view to take the state from. After this call
     *                       @p other will be in a state consistent with being
     *                       a view of a null shell.
     *
     *  @throw None No throw guarantee.
     */
    ShellView(ShellView&& other) noexcept;

    /** @brief Overrides the state in *this with a copy of @p rhs.
     *
     *  This method will create a copy of the state in @p rhs, release the state
     *  held by *this, and then set *this to the copy.
     *
     *  @param[in] rhs The view whose state is being copied.
     *
     *  @return *this, but with its state set to a copy of the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the
     */
    ShellView& operator=(const ShellView& rhs);

    /** @brief Overrides the state in *this with the state in @p rhs.
     *
     *  This ctor will free up the state held by *this and then replace it
     *  with the state held by @p rhs.
     *
     *  @param[in,out] rhs The view we are taking the state from. After the
     *                     assignment @p rhs will be in a state consistent with
     *                     aliasing a null shell.
     *
     *  @return *this, but with its original state replaced with that in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    ShellView& operator=(ShellView&& rhs) noexcept;

    /// Default no-throw dtor
    ~ShellView() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters/setters
    // -------------------------------------------------------------------------

    /** @brief Returns the purity of the aliased shell.
     *
     *  @return The purity of the aliased shell as a read/write reference if
     *          @p T is non-const and a read-only reference if @p T is const.
     *
     *  @throw std::runtime_error if *this is a view of a null shell.
     */
    pure_reference pure();

    /** @brief Returns the purity of the aliased shell
     *
     *  @return A read-only reference to whether the aliased shell is pure.
     *
     *  @throw std::runtime_error if *this is a view of a null shell
     */
    const_pure_reference pure() const;

    /** @brief Returns the total angular momentum of the aliased shell
     *
     *  @return The total angular momentum of the shell as a read/write
     *          reference if @p T is non-const and a read-only reference if it
     *          is const.
     *
     *  @throw none No throw guarantee.
     */
    angular_momentum_reference l();

    /** @brief Returns the total angular momentum of the aliased shell
     *
     *  @return The total angular momentum of the shell as a read-only reference
     *
     *  @throw std::runtime_error if *this is a view of the null shell. Strong
     *                            throw guarantee.
     */
    const_angular_momentum_reference l() const;

    /** @brief The segmented contracted Gaussian function.
     *
     *  @return A potentially mutable reference to the contracted Gaussian.
     *
     *  @throw std::runtime_error if *this is a view of a null shell. Strong
     *                            throw guarantee.
     */
    contracted_gaussian_reference contracted_gaussian();

    /** @brief The segmented contracted Gaussian function.
     *
     *  This method behaves similar to the non-const version except that the
     *  returned reference is guaranteed to be read-only.
     *
     *  @return A read-only reference to the contracted Gaussian.
     *
     *  @throw std::runtime_error if *this is a view of a null shell. Strong
     *                            throw guarantee.
     */
    const_cg_reference contracted_gaussian() const;

    /** @brief Returns the number of primitives in the contracted Gaussian.
     *
     *  Each Shell is associated with a single contracted Gaussian, which in
     *  turn is made up of Gaussian primitives. This method is a convenience
     *  function for calling `contracted_gaussian().size()`. If *this is a
     *  view of a null shell this method will return zero.
     *
     *  @return The number of primitives in the contracted Gaussian.
     *
     *  @throw None.
     *
     *  Complexity: constant
     */
    size_type n_primitives() const noexcept;

    /** @brief Returns the @p i-th primitive in the contracted Gaussian.
     *
     *  @param[in] i The index of the requested primitive. Must be in the
     *               range [0, n_primitives()).
     *
     *  @return A read-/write-able reference to the requested primitive.
     *
     *  @throw std::out_of_range if @p i is not in the range [0,
     *                           n_primitives()). Strong throw guarantee.
     *
     *  Complexity: Constant.
     */
    typename cg_traits::primitive_reference primitive(size_type i);

    /** @brief Returns the @p i-th unique primitive in this ShellView.
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

    /** @brief Returns the number of AOs in *this.
     *
     *  @return The number of AOs in this shell.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with the state of @p other.
     *
     *  @param[in,out] other The instance we are swapping state with. After this
     *                       call @p other will contain the state which was
     *                       previously in *this and *this will contain the
     *                       state which was in @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(ShellView& other) noexcept;

    /** @brief Is this a view of a null Shell?
     *
     *  Shell instances may be a null, consequently we can have ShellViews
     *  which alias a null Shell. This method is used to determine if *this is
     *  a view of a null shell.
     *
     *  @return True if *this is a null shell and false otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool is_null() const noexcept;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two ShellView objects are value equal if they both are views of the
     *  null shell, or if they both are views of non-null shells who compare
     *  value equal. In particular, this means it is the parameters of the
     *  aliased shells which are compared not the addresses.
     *
     *  @param[in] rhs The view being compared to *this.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ShellView& rhs) const noexcept;

    /** @brief Determines if *this is a view of a different Shell than @p rhs.
     *
     *  We define two ShellViews to be different if they are not value equal.
     *  See operator== for the definition of value equal.
     *
     *  @param[in] rhs The view we are comparing to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const ShellView& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    ShellView(pimpl_pointer pimpl) noexcept;

    /// True if *this has a PIMPL and false otherwise
    bool has_pimpl_() const noexcept;

    /// Raises std::runtime_error if *this is a view of a null Shell
    void assert_non_null_() const;

    /// Implements size()
    size_type size_() const noexcept;

    /// The state of *this
    pimpl_pointer m_pimpl_;

}; // ShellView class

extern template class ShellView<ShellD>;
extern template class ShellView<const ShellD>;
extern template class ShellView<ShellF>;
extern template class ShellView<const ShellF>;

} // namespace chemist
