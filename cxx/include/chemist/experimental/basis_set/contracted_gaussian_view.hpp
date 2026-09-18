/*
 * Copyright 2026 NWChemEx-Project
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
#include <chemist/experimental/basis_set/contracted_gaussian_class.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace chemist::experimental {

/** @brief An object which behaves like a ContractedGaussian, but aliases its
 *         buffers.
 *
 *  *this is the contracted-Gaussian-level counterpart of PrimitiveView and
 *  PointSetView, and exists for the same reason: to give a non-owning handle
 *  the exact same API as the owning class. *this is also useful on its own,
 *  as a non-owning handle to a ContractedGaussian.
 *
 *  @tparam CGType A, possibly const-qualified, ContractedGaussian.
 *                 ContractedGaussianView<ContractedGaussian> aliases mutable
 *                 buffers; ContractedGaussianView<const ContractedGaussian>
 *                 aliases read-only buffers and has no setters at all.
 */
template<typename CGType>
class ContractedGaussianView
  : public ContractedGaussianCommon<ContractedGaussianView<CGType>, CGType> {
private:
    /// Type *this inherits from
    using base_type =
      ContractedGaussianCommon<ContractedGaussianView<CGType>, CGType>;

    /// Lets the CRTP base reach coefficient_buffer_, exponent_buffer_, l_,
    /// and center_
    friend base_type;

    /// Lets the container base reach the CRTP base's at_ and size_
    friend utilities::IndexableContainerBase<ContractedGaussianView<CGType>>;

    /// Lets the other const-qualification of *this reach its members
    template<typename OtherCGType>
    friend class ContractedGaussianView;

    /// Enables the mutable-to-const converting ctor, and only that direction
    template<typename T>
    using enable_mutable_to_const_t =
      std::enable_if_t<std::is_const_v<CGType> && !std::is_const_v<T>>;

    /// Type used to alias the angular momentum. A pointer rather than an
    /// angular_momentum_reference for the same reason PrimitiveView's
    /// l_pointer is a pointer: the latter would be a genuine C++ reference,
    /// which can be bound but never rebound, making it impossible to
    /// implement swap() or the converting ctors below.
    using l_pointer =
      std::remove_reference_t<typename base_type::angular_momentum_reference>*;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_reference;
    using typename base_type::buffer_reference;
    using typename base_type::buffer_type;
    using typename base_type::center_reference;
    using typename base_type::coefficient_reference;
    using typename base_type::coefficient_type;
    using typename base_type::const_reference;
    using typename base_type::exponent_reference;
    using typename base_type::exponent_type;
    using typename base_type::reference;
    using typename base_type::size_type;
    using typename base_type::value_type;
    ///@}

    /// Type of a, possibly read-only, reference to the aliased
    /// ContractedGaussian
    using contracted_gaussian_type =
      typename ChemistClassTraits<CGType>::value_type;
    using contracted_gaussian_reference =
      typename ChemistClassTraits<CGType>::reference;

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates a view aliasing nothing.
     *
     *  The resulting view behaves like an empty contraction.
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussianView() noexcept = default;

    /** @brief Creates a view of @p cg.
     *
     *  This ctor is deliberately implicit so that a ContractedGaussian can be
     *  passed wherever a ContractedGaussianView is expected. The result
     *  aliases @p cg; it does not copy it, so writing through *this writes
     *  into @p cg.
     *
     *  @param[in] cg The contracted Gaussian *this will alias.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    ContractedGaussianView(contracted_gaussian_reference cg) :
      ContractedGaussianView(cg.m_coefficients_.as_view(),
                             cg.m_exponents_.as_view(), cg.m_l_, cg.m_center_) {
    }

    /** @brief Creates a view of already-aliased buffers.
     *
     *  This ctor is what lets *this alias buffers which are not owned by a
     *  ContractedGaussian at all.
     *
     *  @param[in] coefficients A view aliasing the coefficient array.
     *  @param[in] exponents A view aliasing the exponent array.
     *  @param[in] l A reference aliasing the (shared) angular momentum.
     *  @param[in] center A view aliasing the (shared) center.
     *
     *  @throw std::invalid_argument if @p coefficients and @p exponents are
     *                               not the same length. Strong throw
     *                               guarantee.
     */
    ContractedGaussianView(buffer_reference coefficients,
                           buffer_reference exponents,
                           angular_momentum_reference l,
                           center_reference center) :
      m_coefficients_(std::move(coefficients)),
      m_exponents_(std::move(exponents)),
      m_l_(&l),
      m_center_(std::move(center)) {
        if(m_coefficients_.size() == m_exponents_.size()) return;
        throw std::invalid_argument(
          "chemist::experimental::ContractedGaussianView: the coefficient "
          "and exponent arrays must be the same length.");
    }

    /** @brief Converts a mutable view into a read-only one.
     *
     *  This ctor is deliberately implicit, so that a mutable view can be
     *  passed wherever a read-only one is expected. The reverse direction is
     *  not provided.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    template<typename OtherCGType,
             typename = enable_mutable_to_const_t<OtherCGType>>
    ContractedGaussianView(const ContractedGaussianView<OtherCGType>& other) :
      m_coefficients_(other.m_coefficients_),
      m_exponents_(other.m_exponents_),
      m_l_(other.m_l_),
      m_center_(other.m_center_) {}

    /** @brief Creates a view aliasing the same buffers as @p other.
     *
     *  The copy is shallow: the resulting view aliases the same buffers
     *  @p other does.
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    ContractedGaussianView(const ContractedGaussianView& other) = default;

    /** @brief Takes the aliased state from @p other.
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussianView(ContractedGaussianView&& other) noexcept = default;

    /** @brief Overwrites the primitives *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's values through into whatever *this is already
     *        aliasing. Use swap if you want to rebind *this.
     *
     *  @tparam OtherDerived The derived type of @p rhs.
     *  @tparam OtherCG The contracted-Gaussian type @p rhs models.
     *
     *  @param[in] rhs The contracted Gaussian whose primitives should be
     *                 copied into the state *this aliases.
     *
     *  @throw std::runtime_error if @p rhs does not hold the same number of
     *                            primitives as *this, or if the concrete
     *                            floating-point types do not match. Weak
     *                            throw guarantee.
     */
    template<typename OtherDerived, typename OtherCG>
    ContractedGaussianView& operator=(
      const ContractedGaussianCommon<OtherDerived, OtherCG>& rhs) {
        static_assert(!std::is_const_v<CGType>,
                      "Can not assign through a read-only "
                      "ContractedGaussianView.");
        assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the primitives *this aliases with @p rhs's.
     *
     *  @note This does NOT make *this alias what @p rhs aliases. It writes
     *        @p rhs's values through into whatever *this is already
     *        aliasing. Use swap if you want to rebind *this.
     *
     *  @throw std::runtime_error if @p rhs does not hold the same number of
     *                            primitives as *this, or if the concrete
     *                            floating-point types do not match. Weak
     *                            throw guarantee.
     */
    ContractedGaussianView& operator=(const ContractedGaussianView& rhs) {
        static_assert(!std::is_const_v<CGType>,
                      "Can not assign through a read-only "
                      "ContractedGaussianView.");
        if(this != &rhs) assign_(rhs);
        return *this;
    }

    /** @brief Overwrites the primitives *this aliases with @p rhs's.
     *
     *  This is identical to the copy assignment operator; there is nothing
     *  to steal, because *this does not own what it aliases.
     *
     *  @throw std::runtime_error under the same conditions as the copy
     *                            assignment operator. Weak throw guarantee.
     */
    ContractedGaussianView& operator=(ContractedGaussianView&& rhs) {
        static_assert(!std::is_const_v<CGType>,
                      "Can not assign through a read-only "
                      "ContractedGaussianView.");
        if(this != &rhs) assign_(rhs);
        return *this;
    }

    /// Default, no-throw dtor. Does not affect the aliased state.
    ~ContractedGaussianView() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Returns a ContractedGaussian holding a copy of the aliased
     *         state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    contracted_gaussian_type as_contracted_gaussian() const {
        const auto n = this->size();
        std::vector<coefficient_type> cs;
        std::vector<exponent_type> es;
        cs.reserve(n);
        es.reserve(n);
        for(size_type i = 0; i < n; ++i) {
            cs.push_back(detail_::copy(m_coefficients_.at(i)));
            es.push_back(detail_::copy(m_exponents_.at(i)));
        }
        return contracted_gaussian_type(cs.begin(), cs.end(), es.begin(),
                                        es.end(), *m_l_, m_center_.as_point());
    }

    /** @brief Makes *this alias what @p other aliases, and vice versa.
     *
     *  Unlike assignment, this rebinds the views; it does not touch the
     *  aliased values.
     *
     *  @throw None No throw guarantee.
     */
    void swap(ContractedGaussianView& other) noexcept {
        std::swap(m_coefficients_, other.m_coefficients_);
        std::swap(m_exponents_, other.m_exponents_);
        std::swap(m_l_, other.m_l_);
        m_center_.swap(other.m_center_);
    }

private:
    /// Implements every assignment operator
    template<typename OtherDerived, typename OtherCG>
    void assign_(const ContractedGaussianCommon<OtherDerived, OtherCG>& rhs) {
        const auto n = this->size();
        if(n != rhs.size())
            throw std::runtime_error(
              "chemist::experimental::ContractedGaussianView: can not assign "
              "a contraction of a different size through a view.");
        for(size_type i = 0; i < n; ++i) (*this)[i] = rhs[i];
        *m_l_     = rhs.get_l();
        m_center_ = rhs.get_center();
    }

    /** @brief Implements the CRTP base's parameter access.
     *
     *  Unlike ContractedGaussian, *this only needs a single, const-qualified
     *  overload of each hook: whether the returned reference is itself
     *  mutable is decided by CGType, via the reference types, not by the
     *  const-qualification of *this.
     */
    ///@{
    buffer_reference coefficient_buffer_() const { return m_coefficients_; }
    buffer_reference exponent_buffer_() const { return m_exponents_; }
    angular_momentum_reference l_() const { return *m_l_; }
    center_reference center_() const { return m_center_; }
    ///@}

    /// Aliases the coefficient array
    buffer_reference m_coefficients_;

    /// Aliases the exponent array
    buffer_reference m_exponents_;

    /// Aliases the (shared) angular momentum. A pointer, not a reference ---
    /// see l_pointer.
    l_pointer m_l_;

    /// Aliases the (shared) center
    center_reference m_center_;
};

/// Type of a view of a mutable ContractedGaussian
using contracted_gaussian_view = ContractedGaussianView<ContractedGaussian>;

/// Type of a view of a read-only ContractedGaussian
using const_contracted_gaussian_view =
  ContractedGaussianView<const ContractedGaussian>;

/** @note Only the mutable instantiation is declared here, for the same
 *        reason PrimitiveView<const Primitive> is not: explicitly
 *        instantiating ContractedGaussianView<const ContractedGaussian>
 *        would force the compiler to instantiate its copy-assignment
 *        operator, which deliberately does not compile for a read-only
 *        view.
 */
extern template class ContractedGaussianView<ContractedGaussian>;

} // namespace chemist::experimental
