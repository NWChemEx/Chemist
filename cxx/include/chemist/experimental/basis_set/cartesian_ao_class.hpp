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
#include <chemist/experimental/basis_set/cartesian_ao_common.hpp>
#include <chemist/experimental/basis_set/contracted_gaussian_class.hpp>
#include <chemist/experimental/basis_set/detail_/ao_impl.hpp>
#include <utility>

namespace chemist::experimental {

/** @brief Models a Cartesian atomic orbital by value.
 *
 *  See CartesianAOCommon for the function *this models and for the shared API,
 *  and AO for the interface *this satisfies as one of the two kinds of atomic
 *  orbital.
 *
 *  *this owns the contracted Gaussian it is built on, and with it that
 *  contraction's center and angular momentum. See CartesianAOView for a class
 *  with the same API which aliases a contracted Gaussian owned by something
 *  else --- which is what a shell of AOs sharing one radial part needs.
 *
 *  *this has no null/non-null distinction: a default-constructed instance is
 *  an s function built on a contraction with no primitives, and so is
 *  equivalent to the zero function.
 */
class CartesianAO : public CartesianAOCommon<CartesianAO, CartesianAO>,
                    public detail_::AOImpl<CartesianAO> {
private:
    /// Type implementing the API shared with CartesianAOView
    using common_type = CartesianAOCommon<CartesianAO, CartesianAO>;

    /// Type implementing the AO interface in terms of *this
    using impl_type = detail_::AOImpl<CartesianAO>;

    /// Lets the CRTP base reach contracted_gaussian_(), i_(), j_(), and k_()
    friend common_type;

    /// Lets a view alias *this's contracted Gaussian directly
    template<typename CartesianAOType>
    friend class CartesianAOView;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename common_type::angular_index_type;
    using typename common_type::angular_momentum_type;
    using typename common_type::center_type;
    using typename common_type::const_center_reference;
    using typename common_type::const_contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_reference;
    using typename common_type::contracted_gaussian_type;
    using typename common_type::coord_type;
    using typename common_type::numerical_value;
    using typename common_type::numerical_vector;
    using typename common_type::size_type;
    using typename common_type::value_type;
    ///@}

    /// Pull the AO interface's types into *this's API
    ///@{
    using typename AO::base_pointer;
    using typename AO::const_base_reference;
    ///@}

    /** @brief Resolves the shared API against the AO interface.
     *
     *  *this inherits each of these twice: once from CartesianAOCommon, which
     *  implements them, and once from AO, which declares them in terms of
     *  virtual methods. The two agree --- AO's are implemented by delegating
     *  to CartesianAOCommon's, below --- but an unqualified call would be
     *  ambiguous, so these declarations pick the non-virtual implementations
     *  for a caller holding a CartesianAO. A caller holding an AO& still goes
     *  through the virtual path, and gets the same answer.
     */
    ///@{
    using common_type::evaluate;
    using common_type::get_center;
    using common_type::get_contracted_gaussian;
    using common_type::get_l;
    using common_type::normalization_constant;
    using common_type::normalized_evaluate;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates an s function built on an empty contraction.
     *
     *  The resulting AO has Cartesian powers of (0, 0, 0), a contraction with
     *  no primitives, and is centered at the origin. Primitives can be added
     *  by calling the other ctor and swapping the result in, exactly as for
     *  ContractedGaussian, and the powers can be changed with set_i/set_j/
     *  set_k. *this can also be used as-is, e.g., as the target of load().
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    CartesianAO() = default;

    /** @brief Constructs a CartesianAO from the provided parameters.
     *
     *  The argument order follows Primitive's and ContractedGaussian's ctors,
     *  with the Cartesian powers in the place those ctors give to the total
     *  angular momentum. That is not an accident: the contracted Gaussian
     *  *this builds takes @f$\ell = i + j + k@f$, so the powers are what
     *  *this is given instead of @f$\ell@f$, not in addition to it. Nothing
     *  needs to be validated as a result --- the two can not disagree, because
     *  only one of them was supplied.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] i The power of @f$x@f$ in the monomial.
     *  @param[in] j The power of @f$y@f$ in the monomial.
     *  @param[in] k The power of @f$z@f$ in the monomial.
     *  @param[in] x The x-coordinate where *this will be centered.
     *  @param[in] y The y-coordinate where *this will be centered.
     *  @param[in] z The z-coordinate where *this will be centered.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges do
     *                               not have the same length. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        contraction. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    CartesianAO(CoefBeginItr&& cbegin, CoefEndItr&& cend, ExpBeginItr&& ebegin,
                ExpEndItr&& eend, angular_index_type i, angular_index_type j,
                angular_index_type k, coord_type x, coord_type y,
                coord_type z) :
      CartesianAO(
        std::forward<CoefBeginItr>(cbegin), std::forward<CoefEndItr>(cend),
        std::forward<ExpBeginItr>(ebegin), std::forward<ExpEndItr>(eend), i, j,
        k, center_type(std::move(x), std::move(y), std::move(z))) {}

    /** @brief Constructs a CartesianAO from the provided parameters.
     *
     *  This ctor is the same as the other range ctor except that it takes an
     *  already existing point to center *this on. See that ctor for the
     *  argument order and for why no validation is needed.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] i The power of @f$x@f$ in the monomial.
     *  @param[in] j The power of @f$y@f$ in the monomial.
     *  @param[in] k The power of @f$z@f$ in the monomial.
     *  @param[in] center Where *this is centered in Cartesian space.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges do
     *                               not have the same length. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        contraction. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    CartesianAO(CoefBeginItr&& cbegin, CoefEndItr&& cend, ExpBeginItr&& ebegin,
                ExpEndItr&& eend, angular_index_type i, angular_index_type j,
                angular_index_type k, center_type center) :
      m_cg_(std::forward<CoefBeginItr>(cbegin), std::forward<CoefEndItr>(cend),
            std::forward<ExpBeginItr>(ebegin), std::forward<ExpEndItr>(eend),
            i + j + k, std::move(center)),
      m_i_(i),
      m_j_(j),
      m_k_(k) {}

    /** @brief Creates a deep copy of @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    CartesianAO(const CartesianAO& other) = default;

    /** @brief Takes ownership of @p other's state.
     *
     *  @throw None No throw guarantee.
     */
    CartesianAO(CartesianAO&& other) noexcept = default;

    /// Overwrites *this with a deep copy of @p other.
    CartesianAO& operator=(const CartesianAO& other) = default;

    /// Overwrites *this with @p other's state.
    CartesianAO& operator=(CartesianAO&& other) noexcept = default;

    /// Default, no-throw dtor
    ~CartesianAO() noexcept override = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(CartesianAO& other) noexcept;

    /** @brief Serializes *this into @p ar.
     *
     *  Writes the Cartesian powers, then the contracted Gaussian. The
     *  contraction's angular momentum is written as part of it, and is
     *  redundant with the powers, but reproducing ContractedGaussian's own
     *  format exactly is worth more than saving three integers.
     *
     *  @tparam Archive The type of the cereal output archive.
     *
     *  @param[in,out] ar The archive to write to.
     *
     *  @throw std::runtime_error if the contraction can not be serialized.
     *                            Weak throw guarantee.
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar(m_i_, m_j_, m_k_);
        m_cg_.save(ar);
    }

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what save wrote. Unlike CartesianAOView, *this owns its
     *  state and so can replace it outright.
     *
     *  @tparam Archive The type of the cereal input archive.
     *
     *  @param[in,out] ar The archive to read from.
     *
     *  @throw std::runtime_error if the archive names a floating-point type
     *                            chemist does not know about or can not
     *                            deserialize. Weak throw guarantee.
     */
    template<typename Archive>
    void load(Archive& ar) {
        CartesianAO buffer;
        ar(buffer.m_i_, buffer.m_j_, buffer.m_k_);
        buffer.m_cg_.load(ar);
        swap(buffer);
    }

private:
    /** @brief Implements the CRTP base's state access.
     *
     *  The contracted Gaussian is handed out as a view aliasing m_cg_, rather
     *  than as a reference to it, so that the type a caller sees is the same
     *  one CartesianAOView hands out.
     */
    ///@{
    contracted_gaussian_reference contracted_gaussian_() {
        return contracted_gaussian_reference(m_cg_);
    }
    const_contracted_gaussian_reference contracted_gaussian_() const {
        return const_contracted_gaussian_reference(m_cg_);
    }
    angular_index_type& i_() noexcept { return m_i_; }
    angular_index_type i_() const noexcept { return m_i_; }
    angular_index_type& j_() noexcept { return m_j_; }
    angular_index_type j_() const noexcept { return m_j_; }
    angular_index_type& k_() noexcept { return m_k_; }
    angular_index_type k_() const noexcept { return m_k_; }
    ///@}

    /** @brief Implements the AO interface in terms of the shared API.
     *
     *  clone_ and are_equal_ are not here; AOImpl implements those from *this's
     *  copy ctor and operator==.
     */
    ///@{
    typename AO::angular_momentum_type get_l_() const noexcept override {
        return common_type::get_l();
    }
    typename AO::const_center_reference get_center_() const override {
        return common_type::get_center();
    }
    typename AO::const_contracted_gaussian_reference get_contracted_gaussian_()
      const override {
        return common_type::get_contracted_gaussian();
    }
    typename AO::numerical_value normalization_constant_() const override {
        return common_type::normalization_constant();
    }
    typename AO::numerical_value evaluate_(
      typename AO::const_point_reference r) const override {
        return common_type::evaluate(r);
    }
    typename AO::numerical_value normalized_evaluate_(
      typename AO::const_point_reference r) const override {
        return common_type::normalized_evaluate(r);
    }
    ///@}

    /// The contracted Gaussian *this pairs its monomial with
    ContractedGaussian m_cg_;

    /// The powers of x, y, and z in the monomial of *this
    ///@{
    angular_index_type m_i_ = 0;
    angular_index_type m_j_ = 0;
    angular_index_type m_k_ = 0;
    ///@}
};

} // namespace chemist::experimental
