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
#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "chemist/orbital_space/spin_space.hpp"
#include <memory>

namespace chemist::orbital_space {

/** @brief Models an orbital space defined as a direct product of two orbital
 * spaces.
 *
 *  In practice most orbital spaces are defined as a transformation from another
 *  space. For example the MOs are usually defined as a linear combination of
 *  the AOs. The DerivedSpace class holds two members: the transformation and
 *  the from space. Both members are read-only and immutable. The only way to
 *  change the values of the members is to assign another DerivedSpace to the
 *  current instance.
 *
 *  @tparam TransformType Type of the object holding the transformation
 *                        coefficients from FromSpace to this space.
 *  @tparam FromSpace The type of the object that the transformation is from.
 *                    Must be default constructable.
 */
template<typename ProductSpaceA, typename ProductSpaceB>
class ProductSpace : public BaseSpace {
public:
    /// Types of the orbital spaces which make the product
    using space_type_a = ProductSpaceA;
    using space_type_b = ProductSpaceB;

    /// Type of pointers to the two spaces
    using space_ptr_a = std::shared_ptr<const ProductSpaceA>;
    using space_ptr_b = std::shared_ptr<const ProductSpaceB>;

    /// Read-only references to the two spaces
    using const_space_reference_a = const space_type_a&;
    using const_space_reference_b = const space_type_b&;

    /// Type of an offset or index
    using size_type = typename BaseSpace::size_type;

    /** @brief Creates a new ProductSpace which has no spaces which
     *         make the product.
     *
     *  The instance created by this ctor is simply a placeholder. It will not
     *  have two spaces. The only way to make the
     *  resulting instance valid is to assign to it.
     *
     *  @throw None No throw guarantee
     */
    ProductSpace() = default;

    /** @brief Creates a new ProductSpace with two spaces.
     *
     * @param[in] SpaceA, SpaceB The two spaces of which the direct product
     *                           gives the result space.
     *
     * @throw None No throw guarantee.
     */
    ProductSpace(ProductSpaceA SpaceA, ProductSpaceB SpaceB);

    /** @brief Aliasing CTor.
     *
     *  This ctor can be used to create a ProductSpace which aliases the state
     *  of another orbital space.
     */
    ProductSpace(space_ptr_a pSpaceA, space_ptr_b pSpaceB);

    /** @brief Initializes this instance by copying another ProductSpace
     *
     *  This is actually a shallow copy. The members of a ProductSpace are
     *  read-only.
     *
     *  @note This constructor is not polymorphic, i.e. the resulting instance
     *        may slice @p other.
     *
     *  @param[in] other The ProductSpace instance we are copying
     *
     *  @throw std::bad_alloc if there is a problem allocating the new
     *                        transformation. Strong throw guarantee.
     */
    ProductSpace(const ProductSpace& other) = default;

    /** @brief Initializes this instance by taking the state of @p other.
     *
     *  The move ctor is a standard move ctor. After this call the new instance
     *  will contain the same two spaces as @p other.
     *
     *  @param[in,out] other The ProductSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *  @throw ??? Throws if the moving the base class throws. Same throw
     *             guarantee.
     */
    ProductSpace(ProductSpace&& other) = default;

    /** @brief Replaces the internal state with a copy of another ProductSpace
     *         instance's state.
     *
     *  This copy is a shallow copy. The members of a ProductSpace are
     * read-only.
     *
     *  @note This operator is not polymorphic, i.e. the resulting instance
     *        may slice @p other.
     *
     *  @param[in] rhs The ProductSpace instance we are copying
     *
     *  @return The current instance, after replacing its state with a copy of
     *          the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new
     *                        transformation. Strong throw guarantee.
     */
    ProductSpace& operator=(const ProductSpace& rhs) = default;

    /** @brief Replaces this instance's state with that of @p other.
     *
     *  This move assignment is the default move assignment. After this call the
     *  new instance will contain the same two spaces as @p other.
     *
     *  @param[in,out] other The ProductSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *
     *  @return The current instance, after taking ownership of the state in
     *          @p rhs.
     *
     *  @throw ??? Throws if the moving the base class throws. Same throw
     *             guarantee.
     */
    ProductSpace& operator=(ProductSpace&& rhs) = default;

    /** @brief Returns the two spaces which make the current product space.
     *
     *  Assuming the coefficients are stored in a standard matrix, the
     *  convention is that the basis set for the rows is `from_space` and the
     *  current orbital space is the basis set for the columns of the
     *  transformation matrix. For more complicated tensors this convention is
     *  generalized.
     *
     *  @return The two spaces which make this
     *          product space in a read-only format.
     */
    const_space_reference_a SpaceRefA() const;
    const_space_reference_b SpaceRefB() const;

    /** @brief Returns the shared_ptr holding the two spaces
     *
     *  Spaces derived from the same from-space can hold shared pointers to that
     *  space for optimization purposes (users are also deep copy the
     *  from-space too). Regardless of whether this instance holds the original
     *  from-space instance or an alias to it, this function can be used to get
     *  a shared pointer to a read-only version of the from-space.
     *
     *  @return The shared_ptr holding the from space.
     *
     *  @throw None No throw guarantee.
     */
    auto space_data_a() const { return m_pSpaceA_; }
    auto space_data_b() const { return m_pSpaceB_; }

    /** @brief Compares two ProductSpace instances non-polymorphically.
     *
     *  This operator will comare the state in ProductSpace (and all base
     *  classes) for equality. This is an exact equality comparison, i.e. it
     *  compares floating point values to full precision with no tolerance.
     *
     *  @note If you want a polymorphic comparison, i.e. you care about whether
     *        state in the product classes is also equal, use the `equal` member
     *        function.
     *
     *  @return True if this space is equal to @p rhs and False otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ProductSpace& rhs) const noexcept;

protected:
    /** @brief Implements size() using the sizes of the two spaces.
     *
     *  size = size(ProductSpaceA) * size(ProductSpaceB)
     *
     *  @return The number of orbitals in this space.
     *
     *  @throw None No throw guarantee.
     */
    virtual size_type size_() const noexcept override;

    /// Include the transformation and the from space in the comparison
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

    /// Construct a vector of space A (direct product of A and B)
    using SpaceVec = std::vector<space_type_a>;
    SpaceVec getSpaceVec(SpaceVec& PSpace);

private:
    /// Type of an instance of this class
    using my_type = ProductSpace<ProductSpaceA, ProductSpaceB>;
    space_ptr_a m_pSpaceA_;
    space_ptr_b m_pSpaceB_;
};

/** @brief Compares ProductSpace instances non-polymorphically.
 *
 *  @relates ProductSpace
 *
 *  @tparam LSpaceA The type of space A in @p lhs.
 *  @tparam LSpaceB     The type of space B in @p lhs.
 *  @tparam RSpaceA The type of space A in @p rhs.
 *  @tparam RSpaceB     The type of space B in @p rhs.
 *
 *  @param[in] lhs The orbital space on the left side of the operator.
 *  @param[in] rhs The orbital space on the right side of the operator.
 *
 *  @return True if @p lhs equals @p rhs and false otherwise.
 */
template<typename LSpaceA, typename LSpaceB, typename RSpaceA, typename RSpaceB>
bool operator==(const ProductSpace<LSpaceA, LSpaceB>& lhs,
                const ProductSpace<RSpaceA, RSpaceB>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>)
        return false;
    else {
        return lhs.operator==(rhs);
    }
}

/** @brief Determines if two ProductSpace instances are non-polymorphically
 *         different.
 *
 *  @relates ProductSpace
 *
 *  @tparam LSpaceA The type of space A in @p lhs.
 *  @tparam LSpaceB     The type of space B in @p lhs.
 *  @tparam RSpaceA The type of space A in @p rhs.
 *  @tparam RSpaceB     The type of space B in @p rhs.
 *
 *  @param[in] lhs The orbital space on the left side of the operator.
 *  @param[in] rhs The orbital space on the right side of the operator.
 *
 *  @return False if @p lhs equals @p rhs and true otherwise.
 */
template<typename LSpaceA, typename LSpaceB, typename RSpaceA, typename RSpaceB>
bool operator!=(const ProductSpace<LSpaceA, LSpaceB>& lhs,
                const ProductSpace<RSpaceA, RSpaceB>& rhs) {
    return !(lhs == rhs);
}

// ------------------------- Typedefs ------------------------------------------

using ASOSpaceD = ProductSpace<AOSpaceD, SpinSpace>;

// ---------------- Explicit Template Instantiations ---------------------------

extern template class ProductSpace<AOSpaceD, SpinSpace>;

} // namespace chemist::orbital_space