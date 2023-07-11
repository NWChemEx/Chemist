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
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian.hpp>
#include <chemist/detail_/view/traits.hpp>
namespace chemist::basis_set {
namespace detail_ {
template<typename CGType>
class ContractedGaussianViewPIMPL;
}

/** @brief Models a reference to a ContractedGaussian
 *
 *  This class models a reference to a ContractedGaussian. Instances of this
 *  class are aliases and any modifications made through the API will modify
 *  the state aliased by *this.
 *
 *  @tparam CGType The type *this is acting as a view of. @p CGType
 *          is expected to be a cv-qualified instantiation of
 *          ContractedGaussian.
 */
template<typename CGType>
class ContractedGaussianView
  : public utilities::IndexableContainerBase<ContractedGaussianView<CGType>> {
private:
    /// The type of *this
    using my_type = ContractedGaussianView<CGType>;

    /// The type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;

    /// Used to work out view types
    using traits_type = chemist::detail_::ViewTraits<CGType>;

    /// Is CGType const-qualified?
    static constexpr bool is_const_v = traits_type::is_const_v;

    /// Typedefs so we don't need "typename" and "template" below
    template<typename T>
    using apply_const = typename traits_type::template apply_const<T>;

    template<typename T>
    using apply_const_ref = typename traits_type::template apply_const_ref<T>;

    template<typename T>
    using ptr_type = typename traits_type::template apply_const_ptr<T>;

public:
    /// Type of the PIMPL implementing *this
    using pimpl_type = detail_::ContractedGaussianViewPIMPL<CGType>;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// The type *this is a view of
    using contracted_gaussian_type = typename traits_type::type;

    using const_cg_reference =
      ContractedGaussianView<const contracted_gaussian_type>;

    /// Type of a primitive in *this
    using value_type = typename contracted_gaussian_type::value_type;

    /// Type of a read-only view to a primitive
    using const_reference = typename contracted_gaussian_type::const_reference;

    /// Type of a potentially mutable view to a primitive
    using reference =
      std::conditional_t<is_const_v, const_reference,
                         typename contracted_gaussian_type::reference>;

    /// Unsigned integral type used for indexing and offsets
    using size_type = typename base_type::size_type;

private:
    /// Type of the traits giving *this the Primitive types
    using primitive_traits = PrimitiveTraits<reference>;

public:
    ContractedGaussianView() noexcept;

    /** @brief Creates a ContractedGaussianView that aliases the provided state.
     *
     *  This ctor assumes that the state it is aliasing is stored contiguously
     *  elsewhere. More specifically it assumes that `(&coef_begin +i)` and
     *  `(&exp_begin + i)` respectively are the coefficient and exponent for
     *  the `i`-th primitive, `i` in the range [0, n_prims).
     *
     *  @param[in] n_prims The number of primitives being contracted to form
     *                     *this.
     *  @param[in] coef_begin The first coefficient in the contracted Gaussian.
     *  @param[in] exp_being The first exponent in the contracted Gaussian.
     *  @param[in] center Where the contracted Gaussian is centered.
     *
     *  @throw std::bad_alloc if there's a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    ContractedGaussianView(
      size_type n_prims,
      typename primitive_traits::coefficient_reference coef_begin,
      typename primitive_traits::exponent_reference exp_begin,
      typename primitive_traits::center_reference center);

    ContractedGaussianView(contracted_gaussian_type cg);

    /** @brief Makes a copy of *this which aliases the same state.
     *
     *  The copy ctor makes a new ContractedGaussianView instance which aliases
     *  the same state as *this.
     *
     *  @param[in] other The instance we are copying.
     *
     *  @throw std::bad_alloc if copying fails. Strong throw guarantee.
     */
    ContractedGaussianView(const ContractedGaussianView& other);

    /** @brief Takes ownership of the state in @p other.
     *
     *  The move ctor creates a new ContractedGaussianView instance by taking
     *  ownership of the state inside @p other. After calling this ctor, *this
     *  will contain the state which was originally in @p other.
     *
     *  @param[in,out] other The instance whose state is being taken. After this
     *                       call @p other will be in a state consistent with
     *                       being a reference to a null ContractedGaussian.
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussianView(ContractedGaussianView&& other) noexcept;

    /** @brief Overrides the state in *this with a copy of @p rhs.
     *
     *  This method will copy the state of @p rhs, release the state of *this,
     *  and then set the state of *this to the copy.
     *
     *  @param[in] rhs The instance we are copying.
     *
     *  @return *this containing a copy of @p rhs.
     *
     *  @throw std::bad_alloc if copying fails. Strong throw guarantee.
     */
    ContractedGaussianView& operator=(const ContractedGaussianView& rhs);

    /** @brief Overrides the state in *this with the state in @p rhs.
     *
     *  This method will release the state held by *this and replace it with
     *  the state in @p rhs.
     *
     *  @param[in,out] rhs The instance we are taking the state of. After this
     *                     call @p rhs will be in a state consistent with being
     *                     a reference to a null ContractedGaussian.
     *
     *  @return *this, but now containing the state which was in @p rhs.
     */
    ContractedGaussianView& operator=(ContractedGaussianView&& rhs) noexcept;

    /// Defaulted no throw dtor
    ~ContractedGaussianView() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters/Setters
    // -------------------------------------------------------------------------

    /** @brief Returns a possibly modifiable reference to the center.
     *
     *  This method is used to retrieve the point in space where *this is
     *  located. The result of this method is mutable if @p CGType is NOT
     *  const-qualified and read-only if @p CGType is const-qualified.
     *
     *  @return A possibly modifiable reference to the center.
     *
     *  @throw std::runtime_error if *this is a reference to a null object.
     *                            Strong throw guarantee.
     */
    typename primitive_traits::center_reference center();

    /** @brief Returns a read-only reference to the center.
     *
     *  This method is similar to the non-const version except the return is
     *  guaranteed to be read-only.
     *
     *  @return A read-only reference to center we are a view of.
     *
     *  @throw std::runtime_error if *this is a view of a null object. Strong
     *                            throw guarantee.
     */
    typename primitive_traits::const_center_reference center() const;

    // -------------------------------------------------------------------------
    // -- Utility functions
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with @p rhs
     *
     *  @param[in,out] rhs The instance whose state *this is taking. After
     *                     calling this method, *this will contain the state
     *                     which was previously in @p rhs and @p rhs will
     *                     contain the state which was previously in *this.
     *
     *  @throw None No throw guarantee
     *
     */
    void swap(ContractedGaussianView& rhs) noexcept;

    /** @brief Is *this a reference to a null ContractedGaussianView?
     *
     *  Being reference-like, *this is guaranteed to always be a view of a
     *  ContractedGaussian; however, that ContractedGaussian may be null. This
     *  method is used to determine if the *this is a view of a null
     *  ContractedGaussian.
     *
     *  @return True if *this is a view of a null ContractedGaussian and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool is_null() const noexcept;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two ContractedGaussianView objects are value equal if the
     *  ContractedGaussian objects they are views of are value equal. In
     *  particular this means that both views must alias a contracted
     *  Gaussian with the same number of primitive Gaussians, those primitive
     *  Gaussians must be stored in the same order, and must have the same
     *  parameters (including coefficient, exponent, and center).
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ContractedGaussianView& rhs) const noexcept;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  This overload is similar to the one above except that @p rhs is owns
     *  its state instead of aliasing it.
     *
     *  @param[in] rhs The object we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const contracted_gaussian_type& rhs) const noexcept;

    /** @brief Determines if *this is different from @p rhs.
     *
     *  We define two ContractedGaussianView objects as different if they are
     *  not value equal. This means that this method simply negates the result
     *  of operator==. See operator== for the definition of value equal.
     *
     *  @param[in] rhs The object we are comparing against.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool operator!=(const ContractedGaussianView& rhs) const noexcept;

    /** @brief Determines if *this is different from @p rhs.
     *
     *  This overload is conceptually similar to the on above, except that\
     *  @p rhs now owns its state instead of aliasing it.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const contracted_gaussian_type& rhs) const noexcept;

    operator const_cg_reference() const {
        return const_cg_reference(size_(), at_(0).coefficient(),
                                  at_(0).exponent(), center());
    }

private:
    /// Allows the base class to implement the container API
    friend base_type;

    /** @brief  Creates the ContractedGaussianView with the provided state.
     *
     *  This ctor is used internally to set the PIMPL after it has been
     *  created.
     *
     *  @param[in] pimpl The state to initialize *this with.
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussianView(pimpl_pointer pimpl) noexcept;

    /// Returns true if *this has a PIMPL and false otherwise
    bool has_pimpl_() const noexcept;

    /// Raises std::runtime_error if *this does not have a PIMPL
    void assert_pimpl_() const;

    size_type size_() const noexcept;

    /// Implements the non-const version of operator[]/at
    reference at_(size_type i) noexcept;

    /// Implements the const version of operator[]/at
    const_reference at_(size_type i) const noexcept;

    /// The state of *this
    pimpl_pointer m_pimpl_;
}; // End class contracted_gaussian

/// Ensures operator== is symmetric for when ContractedGaussian is on the left
template<typename CGType>
bool operator==(const CGType& lhs,
                const ContractedGaussianView<CGType>& rhs) noexcept {
    return rhs == lhs;
}

/// Ensures operator!= is symmetric for when ContractedGaussian is on the left
template<typename CGType>
bool operator!=(const CGType& lhs,
                const ContractedGaussianView<CGType>& rhs) noexcept {
    return rhs != lhs;
}

extern template class ContractedGaussianView<ContractedGaussianD>;
extern template class ContractedGaussianView<const ContractedGaussianD>;
extern template class ContractedGaussianView<ContractedGaussianF>;
extern template class ContractedGaussianView<const ContractedGaussianF>;

} // namespace chemist::basis_set
