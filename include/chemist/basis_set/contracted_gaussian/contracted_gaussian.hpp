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
#include <chemist/basis_set/primitive/primitive_traits.hpp>
#include <chemist/basis_set/primitive/primitive_view.hpp>
#include <utilities/containers/indexable_container_base.hpp>
#include <vector>

namespace chemist {
namespace detail_ {
template<typename PrimitiveType>
class ContractedGaussianPIMPL;
} // namespace detail_

/** @brief Represents a (segmented) contracted Gaussian function.
 *
 *  A (segmented) contracted Gaussian function, $\chi$, is a linear combination
 *  of @f$N@f$ primitive Gaussians and is given by:
 *
 *  @f{
 *     \chi(\vec{r};\vec{r_0}, \vec{c}, \vec{\alpha}) =
 *        \sum_{i=1}^N c_ie^{-\alpha_i (\vec{r}-\vec{r_0})^2}

 *  @f}.
 *  Here, @f$\vec{r_0}@f$ is the point where it is centered, @f$c_i@f$ is the
 *  expansion coefficient of the @f$i@f$-th primitive, and @f$\alpha_i@f$ is
 *  the exponent of the @f$i@f$-th primitive.
 *
 *  Conceptually the ContractedGaussian class is simply a container of
 *  Primitive instances which enforces that the primitives must be centered on
 *  the same point in space. It should be noted that modifying where *this is
 *  centered through calling `center` or by directly modifying where a specific
 *  primitive is centered, will affect where all primitives are centered.
 *
 * @tparam PrimitiveType The non-cv qualified type of each primitive Gaussian.
 */
template<typename PrimitiveType>
class ContractedGaussian : public utilities::IndexableContainerBase<
                             ContractedGaussian<PrimitiveType>> {
private:
    /// The type of this class
    using my_type = ContractedGaussian<PrimitiveType>;

    /// the type of the IndexableContainerBase
    using container_base = utilities::IndexableContainerBase<my_type>;

public:
    /// The type of the PIMPL implementing this class
    using pimpl_type = detail_::ContractedGaussianPIMPL<PrimitiveType>;

    /// The type of a pointer to this class's PIMPL
    using pimpl_ptr = std::unique_ptr<pimpl_type>;

    /// Type of the primitives in this contraction
    using value_type = PrimitiveType;

    /// Type of a mutable view of a primitive
    using reference = PrimitiveView<value_type>;

    /// Type of a read-only view to a Primitive
    using const_reference = PrimitiveView<const value_type>;

    /// Unsigned integral type of an index/offset
    using size_type = typename container_base::size_type;

    /// ------------------------------------------------------------------------
    /// -- Primitive types
    /// ------------------------------------------------------------------------

    using primitive_traits = PrimitiveTraits<reference>;

    /// Type of a std::vector of coefficents
    using coefficient_vector =
      std::vector<typename primitive_traits::coefficient_type>;

    /// Type of a std;:vector of exponents
    using exponent_vector =
      std::vector<typename primitive_traits::exponent_type>;

    /** @brief Makes a null ContractedGaussian.
     *
     *  The default ctor makes a null ContractedGaussian instance which means
     *  the resulting instance contains no Primitives and is not centered
     *  anywhere. State can be added to a null instance by calling `center()`
     *  and/or adding primitives.
     *
     *  @throw None No throw guarantee.
     */
    ContractedGaussian() noexcept;

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
     *  @param[in] x The x-coordinate where *this will be centered.
     *  @param[in] y The y-coordinate where *this will be centered.
     *  @param[in] z The z-coordinate where *this will be centered.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    ContractedGaussian(coefficient_vector coefs, exponent_vector exps,
                       typename primitive_traits::coord_type x,
                       typename primitive_traits::coord_type y,
                       typename primitive_traits::coord_type z);

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
     *  @param[in] center Where *this is centered in Cartesian space.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    ContractedGaussian(coefficient_vector coefs, exponent_vector exps,
                       typename primitive_traits::center_type center);

    /** @brief Constructs a new ContractedGaussian instance by copying the state
     *         of @p rhs.
     *
     *  This ctor is used to initialize a ContractedGaussian with a deep copy
     *  of another instance's state.
     *
     *  @param[in] rhs The ContractedGaussian instance whose state is being
     *                 copied.
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         PIMPL. Strong throw guarantee.
     */
    ContractedGaussian(const my_type& rhs);

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
    ContractedGaussian(my_type&& rhs) noexcept;

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
    my_type& operator=(const my_type& rhs);

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
    my_type& operator=(my_type&& rhs) noexcept;

    /// Default dtor
    ~ContractedGaussian() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters/Setters
    // -------------------------------------------------------------------------

    /** @brief Returns the location where *this is centered.
     *
     *  All primitive Gaussians forming a contracted Gaussian, must be centered
     *  on the same point. This method can be used to retrieve a reference to
     *  that point. If *this is a null instance, this method will first allocate
     *  state and then return the default initialized center (which will be
     *  oriented at the origin).
     *
     *  It should be noted that modifying where *this is centered, either
     *  through this method or by directly accessing any of the primitives,
     *  changes where all primitives are centered.
     *
     *  @return The point in Cartesian space where *this is located.
     *
     *  @throw std::bad_alloc if *this is a null instance and allocating the
     *                        PIMPL fails. Strong throw guarantee.
     */
    typename primitive_traits::center_reference center();

    /** @brief Returns the location where *this is centered.
     *
     *  This method is the same as the non-const version except that an error
     *  is raised if *this is null and the resulting center is read-only.
     *
     *  @return A read-only reference to the point where *this is centered.
     *
     *  @throw std::runtime_error if *this is null. Strong throw guarantee.
     */
    typename primitive_traits::const_center_reference center() const;

    // -------------------------------------------------------------------------
    // -- Utility functions
    // -------------------------------------------------------------------------

    /** @brief Exchanges the state in *this with that in @p rhs.
     *
     *  @param[in, out] rhs The instance *this will take state from. After the
     *                      operation, @p rhs will contain the state which
     *                      previously was in *this.
     *
     *  @throw None No throw gurantee.
     */
    void swap(my_type& rhs) noexcept;

    /** @brief Used to determine if *this is null.
     *
     *  A null ContractedGaussian is one that does not have any primitives in
     *  it and does not have a center set. Such instances are created by
     *  default.
     *
     *  @return True if *this is null and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool is_null() const noexcept { return !has_pimpl_(); }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  We define non-null ContractedGaussian instances to be value equal if:
     *  they contain the same number of primitives @f$N@f$ and if for each
     *  @f$i@f$ in the range [0, @f$N@f$) `(*this)[i] == rhs[i]`. Notably, this
     *  means the primitives must be in the same order and that the parameters
     *  in the primitives are also compared for value equality (*i.e.*, the
     *  parameters must be bitwise equivalent).
     *
     *  @param[in] rhs The ContractedGaussian we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const my_type& rhs) const noexcept;

    /** @brief Determines if *this is different than @p rhs.
     *
     *  We define two ContractedGaussian instances to be different if they are
     *  not value equal. In practice this means that this method simply negates
     *  a call to operator==. See the description for operator== for the
     *  definition of value equal.
     *
     *  @param[in] rhs The ContractedGaussian we are comparing to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const my_type& rhs) const noexcept;

    /** @brief Serialize ContractedGaussian instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void save(Archive& ar) const {
        ar& this->size() & this->center();
        for(const auto& p : *this) { ar& p.coefficient() & p.exponent(); }
    }

    /** @brief Deserialize for ContractedGaussian instance
     *
     * @param ar The archive object
     */
    template<typename Archive>
    void load(Archive& ar) {
        size_type mysize;
        typename primitive_traits::center_type center;
        ar& mysize& center;
        coefficient_vector cs(mysize, 0);
        exponent_vector es(mysize, 0);
        for(int i = 0; i < mysize; ++i) { ar& cs[i] & es[i]; }

        ContractedGaussian(cs, es, center).swap(*this);
    }

private:
    /// Allows the IndexableContainerBase to access the implementations
    friend container_base;

    /** @brief Constructs a new ContractedGaussian instance by wrapping the
     *         provided state.
     *
     *  This ctor is intended for use when the PIMPL holding the state has
     *  already been created elsewhere. In practice, this ctor is the ultimate
     *  dispatch point for most of the stateful ctors.
     *
     *  @param[in] my_pimpl The PIMPL which stores the coefficients and
     *                      exponents for the ContractedGaussian class.
     *
     *  @throw none No throw guarantee.
     */
    explicit ContractedGaussian(pimpl_ptr my_pimpl) noexcept;

    /// Does *this have a PIMPL instantiated?
    bool has_pimpl_() const noexcept;

    /// Throw std::runtime_error if *this has no PIMPL
    void assert_pimpl_() const;

    /// Implements size()
    size_type size_() const noexcept;

    /// Implements operator[]/at
    reference at_(size_type index) noexcept;

    /// Implements operator[]const/at const
    const_reference at_(size_type index) const noexcept;

    /// The instance actually implementing the class
    pimpl_ptr m_pimpl_;
}; // End class contracted_gaussian

/// Type of a ContractedGaussian where all parameters are doubles
using ContractedGaussianD = ContractedGaussian<PrimitiveD>;

/// Type of a ContractedGaussian where all parameters are floats
using ContractedGaussianF = ContractedGaussian<PrimitiveF>;

extern template class ContractedGaussian<PrimitiveD>;
extern template class ContractedGaussian<PrimitiveF>;

} // namespace chemist
