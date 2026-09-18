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
#include <chemist/experimental/basis_set/contracted_gaussian_common.hpp>
#include <chemist/types/floating_point.hpp>
#include <stdexcept>

namespace chemist::experimental {

/** @brief Models a (segmented) contracted Gaussian by value.
 *
 *  See ContractedGaussianCommon for the function *this models and for the
 *  shared API.
 *
 *  *this owns its coefficient/exponent arrays, its angular momentum, and its
 *  center. See ContractedGaussianView for a class with the same API which
 *  aliases state owned by something else.
 *
 *  *this has no null/non-null distinction: a default-constructed instance is
 *  a contracted Gaussian with no primitives. It is equivalent to the zero
 *  function (i.e., returns zero regardless of the input).
 */
class ContractedGaussian
  : public ContractedGaussianCommon<ContractedGaussian, ContractedGaussian> {
private:
    /// Type *this inherits from
    using base_type =
      ContractedGaussianCommon<ContractedGaussian, ContractedGaussian>;

    /// Lets the CRTP base reach coefficient_buffer_, exponent_buffer_, l_,
    /// and center_
    friend base_type;

    /// Lets a view alias *this's buffers directly
    template<typename ContractedGaussianType>
    friend class ContractedGaussianView;

    /** @brief The concrete types push_back is allowed to widen a buffer to.
     *
     *  See PointSet::fp_types for why chemist's full list, and not
     *  wtf::default_fp_types, is named explicitly here.
     */
    using fp_types = chemist::types::floating_point_types;

public:
    /// Pull the shared API's types into *this's API
    ///@{
    using typename base_type::angular_momentum_reference;
    using typename base_type::angular_momentum_type;
    using typename base_type::buffer_reference;
    using typename base_type::buffer_type;
    using typename base_type::center_reference;
    using typename base_type::center_type;
    using typename base_type::coefficient_reference;
    using typename base_type::coefficient_type;
    using typename base_type::const_angular_momentum_reference;
    using typename base_type::const_buffer_reference;
    using typename base_type::const_center_reference;
    using typename base_type::const_coefficient_reference;
    using typename base_type::const_exponent_reference;
    using typename base_type::const_reference;
    using typename base_type::coord_type;
    using typename base_type::exponent_reference;
    using typename base_type::exponent_type;
    using typename base_type::reference;
    using typename base_type::size_type;
    using typename base_type::value_type;
    ///@}

    // -------------------------------------------------------------------------
    // -- Ctors and assignment
    // -------------------------------------------------------------------------

    /** @brief Creates an empty contracted Gaussian.
     *
     *  The resulting contraction has no primitives, an angular momentum of
     *  0, and is centered at the origin. Primitives can be added by calling
     *  one of the other ctors and swapping the result in, or *this can be
     *  used as-is, e.g., as the target of load().
     *
     *  @throw std::bad_alloc if there is a problem allocating the state of
     *                        *this. Strong throw guarantee.
     */
    ContractedGaussian() = default;

    /** @brief Constructs a ContractedGaussian from the provided parameters.
     *
     *  This ctor is largely intended for constructing a ContractedGaussian
     *  when the parameters are not already stored in Primitives.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] l The total angular momentum shared by every primitive in
     *               the contraction.
     *  @param[in] x The x-coordinate where *this will be centered.
     *  @param[in] y The y-coordinate where *this will be centered.
     *  @param[in] z The z-coordinate where *this will be centered.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges
     *                               do not have the same length. Strong
     *                               throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        buffers. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    ContractedGaussian(CoefBeginItr&& cbegin, CoefEndItr&& cend,
                       ExpBeginItr&& ebegin, ExpEndItr&& eend,
                       angular_momentum_type l, coord_type x, coord_type y,
                       coord_type z) :
      ContractedGaussian(
        std::forward<CoefBeginItr>(cbegin), std::forward<CoefEndItr>(cend),
        std::forward<ExpBeginItr>(ebegin), std::forward<ExpEndItr>(eend), l,
        center_type(std::move(x), std::move(y), std::move(z))) {}

    /** @brief Constructs a ContractedGaussian from the provided parameters.
     *
     *  This ctor is the same as the other range ctor except that it takes an
     *  already existing point to center *this on.
     *
     *  @param[in] cbegin Iterator to the beginning of the container holding
     *                    the coefficients.
     *  @param[in] cend Iterator to the end of the container holding the
     *                  coefficients.
     *  @param[in] ebegin Iterator to the beginning of the container holding
     *                    the exponents.
     *  @param[in] eend Iterator to the end of the container holding the
     *                  exponents.
     *  @param[in] l The total angular momentum shared by every primitive in
     *               the contraction.
     *  @param[in] center Where *this is centered in Cartesian space.
     *
     *  @throw std::invalid_argument if the coefficient and exponent ranges
     *                               do not have the same length. Strong
     *                               throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        buffers. Strong throw guarantee.
     */
    template<typename CoefBeginItr, typename CoefEndItr, typename ExpBeginItr,
             typename ExpEndItr>
    ContractedGaussian(CoefBeginItr&& cbegin, CoefEndItr&& cend,
                       ExpBeginItr&& ebegin, ExpEndItr&& eend,
                       angular_momentum_type l, center_type center) :
      m_l_(l), m_center_(std::move(center)) {
        for(auto itr = cbegin; itr != cend; ++itr)
            m_coefficients_.template push_back<fp_types>(*itr);
        for(auto itr = ebegin; itr != eend; ++itr)
            m_exponents_.template push_back<fp_types>(*itr);
        if(m_coefficients_.size() == m_exponents_.size()) return;
        throw std::invalid_argument(
          "chemist::experimental::ContractedGaussian: the coefficient and "
          "exponent ranges must be the same length.");
    }

    /** @brief Creates a deep copy of @p other.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy.
     *                        Strong throw guarantee.
     */
    ContractedGaussian(const ContractedGaussian& other) = default;

    /** @brief Takes ownership of @p other's state.
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussian(ContractedGaussian&& other) noexcept = default;

    /// Overwrites *this with a deep copy of @p other.
    ContractedGaussian& operator=(const ContractedGaussian& other) = default;

    /// Overwrites *this with @p other's state.
    ContractedGaussian& operator=(ContractedGaussian&& other) noexcept =
      default;

    /// Default, no-throw dtor
    ~ContractedGaussian() noexcept = default;

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state of *this with that of @p other.
     *
     *  @throw None No throw guarantee.
     */
    void swap(ContractedGaussian& other) noexcept;

    /** @brief Deserializes *this from @p ar.
     *
     *  Reads back what ContractedGaussianCommon::save wrote: the number of
     *  primitives, then the (shared) angular momentum and center exactly
     *  once, then each primitive's coefficient/exponent pair. Unlike
     *  ContractedGaussianView, *this owns its state and so can replace it
     *  outright.
     *
     *  @throw std::runtime_error if the archive names a floating-point type
     *                            chemist does not know about or can not
     *                            deserialize. Weak throw guarantee.
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type n{};
        ar(n);
        angular_momentum_type l{};
        ar(l);
        center_type center;
        center.load(ar);

        buffer_type coefficients, exponents;
        for(size_type i = 0; i < n; ++i) {
            coefficients.template push_back<fp_types>(detail_::load_float(ar));
            exponents.template push_back<fp_types>(detail_::load_float(ar));
        }

        ContractedGaussian buffer;
        buffer.m_l_            = l;
        buffer.m_center_       = std::move(center);
        buffer.m_coefficients_ = std::move(coefficients);
        buffer.m_exponents_    = std::move(exponents);
        swap(buffer);
    }

private:
    /** @brief Implements the CRTP base's parameter access. */
    ///@{
    buffer_reference coefficient_buffer_() { return m_coefficients_.as_view(); }
    const_buffer_reference coefficient_buffer_() const {
        return m_coefficients_.as_view();
    }
    buffer_reference exponent_buffer_() { return m_exponents_.as_view(); }
    const_buffer_reference exponent_buffer_() const {
        return m_exponents_.as_view();
    }
    angular_momentum_reference l_() noexcept { return m_l_; }
    const_angular_momentum_reference l_() const noexcept { return m_l_; }
    center_reference center_() noexcept { return m_center_; }
    const_center_reference center_() const noexcept { return m_center_; }
    ///@}

    /// The contraction coefficients, in order
    buffer_type m_coefficients_;

    /// The primitive exponents, in order
    buffer_type m_exponents_;

    /// The total angular momentum shared by every primitive in *this
    angular_momentum_type m_l_ = 0;

    /// The point shared by every primitive in *this
    center_type m_center_{};
};

} // namespace chemist::experimental
