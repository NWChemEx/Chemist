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
#include <chemist/basis_set/contracted_gaussian.hpp>
#include <chemist/detail_/view_traits.hpp>

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

    /// Is @p OtherType a mutable view?
    template<typename OtherType>
    static constexpr auto other_is_mutable_view =
      std::is_same_v<OtherType,
                     ContractedGaussianView<typename traits_type::type>>;

    /// Is @p OtherType a read-only view?
    template<typename OtherType>
    static constexpr auto other_is_const_view =
      std::is_same_v<OtherType,
                     ContractedGaussianView<const typename traits_type::type>>;

    /// Disables a function if *this is not read-only and @p OtherType isn't
    /// a mutable view
    template<typename OtherType>
    using enable_if_this_const_other_mutable =
      std::enable_if_t<is_const_v && other_is_mutable_view<OtherType>>;

    /// Disables a function if *this is read-only and @p OtherType is
    /// a mutable view
    template<typename OtherType>
    using enable_if_this_mutable_other_const =
      std::enable_if_t<!is_const_v && other_is_const_view<OtherType>>;

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

    /// Type used to represent the value of *this at a point
    using numerical_value = typename contracted_gaussian_type::numerical_value;

    /// Type used to represent the value of *this at a set of points
    using numerical_vector =
      typename contracted_gaussian_type::numerical_vector;

    /// Type used to represent a view of a point
    using const_point_view =
      typename contracted_gaussian_type::const_point_view;

    /// Type used to represent constant view of a set of points
    using const_point_set_view =
      typename contracted_gaussian_type::const_point_set_view;

private:
    /// Type of the traits giving *this the Primitive types
    using primitive_traits = PrimitiveTraits<reference>;

public:
    // -------------------------------------------------------------------------
    // -- Ctors, assignment, and dtor
    // -------------------------------------------------------------------------

    /** @brief Aliases a null ContractedGaussian.
     *
     *  A default view aliases a null ContractedGaussian (a ContractedGaussian
     *  with no center and no primitives).
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussianView() noexcept;

    /** @brief Creates a ContractedGaussianView that aliases the provided state.
     *
     *  This ctor assumes that the state it is aliasing is stored contiguously
     *  elsewhere. More specifically it assumes that `(&coef_begin +i)` and
     *  `(&exp_begin + i)` respectively are the coefficient and exponent for
     *  the `i`-th primitive, `i` in the range [0, n_prims).
     *
     *  @note This ctor can NOT be used to create a view of a null
     *        ContractedGaussian. Use the default ctor instead.
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

    /** @brief Creates a view of the ContractedGaussian @p cg.
     *
     *  This ctor allows existing ContractedGaussian objects to be used
     *  interchangeably with ContractedGaussianView objects by creating a view
     *  which aliases the state of the ContractedGaussian object.
     *
     *  @note This ctor is NOT explicit to allow the conversion from
     *        ContractedGaussian to ContractedGaussianView to happen
     *        automatically. Also note this is NOT the copy ctor.
     *
     *  @param[in] cg The contracted Gaussian function *this will alias.
     *
     *  @throw std::bad_alloc if there's a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    ContractedGaussianView(apply_const_ref<contracted_gaussian_type> cg);

    /** @brief Sets the already aliased ContractedGaussian's state to a copy
     *         of @p rhs.
     *
     *  This operator will set the state of the ContractedGaussian aliased by
     *  *this to a copy of @p rhs. In particular, this operator will NOT
     *  alias the state in @p rhs. To make *this alias the state in @p rhs,
     *  use the copy assignment operator.
     *
     *  @tparam CGType2 The type of the ContractedGaussian whose state is being
     *                  copied. Should always be non-cv-qualified @p CGType.
     *  @tparam <anonymous> Used to disable this function when the type of *this
     *                      is a view of read-only ContractedGaussian.
     *
     *  @param[in] rhs The ContractedGaussian whose state is being assigned to
     *                 *this. @p rhs must be non-null and must contain the same
     *                 number of primitive Gaussian functions as *this.
     *
     *  @return *this after modifying the aliased state.
     *
     *  @throw std::runtime_error if *this is a view of a null
     *             ContractedGaussian, @p rhs is a null ContractedGaussian, or
     *             if the sizes of *this and @p rhs are different. Strong throw
     *             guarantee.
     *
     */
    template<typename CGType2 = std::decay_t<CGType>,
             typename         = std::enable_if<std::is_same_v<CGType2, CGType>>>
    ContractedGaussianView& operator=(const CGType2& rhs);

    /** @brief Implicit conversion of a mutable view to a read-only view
     *
     *  @note This is NOT the copy ctor.
     *
     *  This ctor can be used to convert a ContractedGaussianView of a mutable
     *  contracted Gaussian to a view of a read-only contracted Gaussian.
     *
     *  @tparam OtherType The type of the other view. @p OtherType is assumed to
     *                    be ContractedGaussianView<ContractedGaussian<T>>.
     *  @tparam <anonymous> Used to disable this function when *this is mutable
     *                      or when @p OtherType is read-only
     *
     *  @param[in] other The view we are implicitly converting to a read-only
     *                   view.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    template<typename OtherType,
             typename = enable_if_this_const_other_mutable<OtherType>>
    ContractedGaussianView(const OtherType& other);

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

    /** @brief Computes the value of *this at the point @p r.
     *
     *  The value of a contracted Gaussian at the vector @f$r@f$ is the sum of
     *  the primitive gaussians forming the contraction evaluated at @f$r@f$.
     *
     *  @param[in] r The point where the contracted Gaussian should be
     *             evaluated.
     *
     *  @return The value of the contracted Gaussian at the point @p r.
     */
    numerical_value evaluate(const_point_view r) const;

    /** @brief Computes the value of *this at a series of points @p points.
     *
     *  This function is the same as the other overload of evaluate except that
     *  it computes the value of the contracted Gaussian at a series of points
     *  and returns a vector of the values.
     *
     *  @param[in] points The points where the contracted Gaussian should be
     *                    evaluated.
     *
     *  @return The value of the contracted Gaussian at the requested points.
     */
    numerical_vector evaluate(const_point_set_view points) const;

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

    /// Raises std::runtime_error if this is a view of a null CG
    void assert_non_null_() const;

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

#include "contracted_gaussian_view.ipp"
