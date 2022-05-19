#pragma once
#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include <memory>

namespace chemist::orbital_space {

/** @brief Models an orbital space defined in terms of another orbital space.
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
template<typename TransformType, typename FromSpace>
class DerivedSpace : public BaseSpace {
private:
    /// Type of an instance of this class
    using my_type = DerivedSpace<TransformType, FromSpace>;

public:
    /// Type of the transformation matrix
    using transform_type = TransformType;

    /// How the transformation matrix is stored
    using transform_ptr = std::shared_ptr<const transform_type>;

    /// A read-only reference to the transform
    using const_transform_reference = const transform_type&;

    /// Type of the orbital space this space is a transformation of
    using from_space_type = FromSpace;

    /// Type of a pointer to the from space
    using from_space_ptr = std::shared_ptr<const FromSpace>;

    /// A read-only reference to the from space
    using const_from_space_reference = const from_space_type&;

    /// Type of an offset or index
    using size_type = typename BaseSpace::size_type;

    /** @brief Creates a new DerivedSpace which has no transformation
     *         or from space.
     *
     *  The instance created by this ctor is simply a placeholder. It will not
     *  have a transformation or a from space. The only way to make the
     *  resulting instance valid is to assign to it.
     *
     *  @throw None No throw guarantee
     */
    DerivedSpace() = default;

    /** @brief Creates a new DerivedSpace_ with the specified state.
     *
     *  Of note, this ctor will create an instance which owns the "from space"
     *  associated with it. Other spaces can alias this space if desired, but
     *  use of this ctor guarantees that the "from space" will not change for
     *  the duration of this instance.
     *
     * @param[in] C The transformation coefficients that take you from the
     *              orbital space `from_space` to this instance's orbital space.
     *              Default value is a default constructed instance of type
     *              `TransformType`.
     *
     * @param[in] from_space The orbital space in terms of which this space is
     *                       defined.
     *
     * @throw None No throw guarantee.
     */
    DerivedSpace(transform_type C, from_space_type from_space);

    /** @brief Creates a new DerivedSpace with the specified state.
     *
     *  This ctor is designed to be used when the "from space" instance has
     *  already been created and lives in  another "to space". For example one
     *  could use this ctor to construct the virtual MO space using the AO space
     *  stored in the already constructed occupied MO space. Since this ctor is
     *  public there's nothing stopping you from doing:
     *
     *  @code
     *  auto pfrom = std::make_shared<from_space_type>();
     *  DerivedSpace derive(C, pfrom);
     *  @endcode
     *
     *  The above code violates the assumption that the original from space
     *  resides in another DerivedSpace and allows you to modify the from space.
     *  Modifying the from space after it has been set is undefined behavior.
     *
     *
     * @tparam Args The types of the inputs which will be forwarded to the base
     *              class's ctor.
     * @param[in] C The transformation from the orbital space `*pfrom_space` to
     *              the current orbital space.
     * @param[in] pfrom_space A shared pointer to the "from space". The object
     *                        pointed at is read-only.
     * @param[in] args The arguments which will be forwarded to the base class's
     *                 ctor.
     *
     * @throw ??? If forwarding `args` throws. Same throw guarantee.
     */
    DerivedSpace(transform_type C, from_space_ptr pfrom_space);

    /** @brief Aliasing CTor.
     *
     *  This ctor can be used to create a DerivedSpace which aliases the state
     *  of another orbital space.
     */
    DerivedSpace(transform_ptr pC, from_space_ptr pfrom_space);

    /** @brief Initializes this instance by copying another DerivedSpace
     *
     *  This is actually a shallow copy. The members of a DerivedSpace are
     *  read-only.
     *
     *  @note This constructor is not polymorphic, i.e. the resulting instance
     *        may slice @p other.
     *
     *  @param[in] other The DerivedSpace instance we are copying
     *
     *  @throw std::bad_alloc if there is a problem allocating the new
     *                        transformation. Strong throw guarantee.
     */
    DerivedSpace(const DerivedSpace& other) = default;

    /** @brief Initializes this instance by taking the state of @p other.
     *
     *  The move ctor is a standard move ctor. After this call the new instance
     *  will contain the same transformation as @p other and alias the same
     *  from space as @p other.
     *
     *  @param[in,out] other The DerivedSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *  @throw ??? Throws if the moving the base class throws. Same throw
     *             guarantee.
     */
    DerivedSpace(DerivedSpace&& other) = default;

    /** @brief Replaces the internal state with a copy of another DerivedSpace
     *         instance's state.
     *
     *  This copy is a shallow copy. The members of a Derived
     *
     *  @note This operator is not polymorphic, i.e. the resulting instance
     *        may slice @p other.
     *
     *  @param[in] rhs The DerivedSpace instance we are copying
     *
     *  @return The current instance, after replacing its state with a copy of
     *          the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new
     *                        transformation. Strong throw guarantee.
     */
    DerivedSpace& operator=(const DerivedSpace& rhs) = default;

    /** @brief Replaces this instance's state with that of @p other.
     *
     *  This move assignment is the default move assignment. After this call the
     *  new instance will contain the same transformation as @p other and alias
     *  the same from space as @p other.
     *
     *  @param[in,out] other The DerivedSpace instance that we are taking the
     *                       state of. After this call @p other will be in a
     *                       valid, but otherwise undefined state.
     *
     *  @return The current instance, after taking ownership of the state in
     *          @p rhs.
     *
     *  @throw ??? Throws if the moving the base class throws. Same throw
     *             guarantee.
     */
    DerivedSpace& operator=(DerivedSpace&& rhs) = default;

    /** @brief Returns the transformation coefficients to go from `from_space`
     *         to the current space.
     *
     *  Assuming the coefficients are stored in a standard matrix, the
     *  convention is that the basis set for the rows is `from_space` and the
     *  current orbital space is the basis set for the columns of the
     *  transformation matrix. For more complicated tensors this convention is
     *  generalized.
     *
     *  @return The transformation coefficients to go from `from_space` to this
     *          space in a read-only format.
     */
    const_transform_reference C() const;

    /** @brief Returns the shared_ptr holding the transformation.
     *
     *  This function can be used to retrieve a pointer to the transformation.
     *
     *  @return A shared_ptr holding the transformation
     *
     *  @throw None No throw guarantee.
     */
    auto C_data() const { return m_pC_; }

    /** @brief Returns the space that this orbital space is defined in terms of.
     *
     *
     *  @return The orbital space that this space is defined in terms of in a
     *          read-only manner.
     *
     *  @throw std::runtime_error if this instance does not contain a from
     *                            space. Strong throw guarantee.
     */
    const_from_space_reference from_space() const;

    /** @brief Returns the shared_ptr holding the from-space
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
    auto from_space_data() const { return m_pbase_; }

    /** @brief Non-polymorphically takes the union of this space and another
     *         derived space.
     *
     *  This function will take the union of this space with @p rhs. The
     *  union is done non-polymorphically, meaning state in derived classes is
     *  not considered.
     *
     *  @note Strictly speaking this is simply a concatenation of the two
     *        spaces. As long as the spaces are different this will be a union.
     *        However, no attempt to remove redundant basis functions is made.
     *
     *  @param[in] rhs The space we are taking the union with.
     *
     *  @return A new basis set which is the union of this space and that of
     *          @p rhs. The basis functions will be ordered this space's
     *          functions followed by @p rhs's functions.
     *
     *  @throw std::runtime_error if this space has a from space that differs
     *                            from that of @p rhs. Strong throw guarantee.
     */
    DerivedSpace operator+(const DerivedSpace& rhs) const;

    /** @brief Compares two DerivedSpace instances non-polymorphically.
     *
     *  This operator will comare the state in DerivedSpace (and all base
     *  classes) for equality. This is an exact equality comparison, i.e. it
     *  compares floating point values to full precision with no tolerance.
     *
     *  @note If you want a polymorphic comparison, i.e. you care about whether
     *        state in the derived classes is also equal, use the `equal` member
     *        function.
     *
     *  @return True if this space is equal to @p rhs and False otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const DerivedSpace& rhs) const noexcept;

protected:
    /** @brief Implements size() using the transformation coefficients stored in
     *         this instane.
     *
     *  Exactly how size() behaves depends on how the coefficients in this class
     *  are stored. If the transformation uses a normal tensor (i.e., not a
     *  ToT) the return is the number of columns (transformation is assumed to
     *  be from-space on the rows, to-space on the columns), noting that an
     *  uninitialized tensor has zero columns. If the transformation is a ToT
     *  this function returns the sum of the domain sizes.
     *
     *  @return The number of orbitals in this space.
     *
     *  @throw None No throw guarantee.
     */
    virtual size_type size_() const noexcept override;

    /// Include the transformation and the from space in the hash
    virtual void hash_(chemist::detail_::Hasher& h) const override;

    /// Include the transformation and the from space in the comparison
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The transformation coefficients to this orbital space
    transform_ptr m_pC_;

    /// The basis set that this space is defined in terms of
    from_space_ptr m_pbase_;
};

/** @brief Compares DerivedSpace instances non-polymorphically.
 *
 *  @relates DerivedSpace
 *
 *  @tparam LTransformType The type of the tranform in @p lhs.
 *  @tparam LFromSpace     The type of the from space in @p lhs.
 *  @tparam RTransformType The type of the transform in @p rhs.
 *  @tparam RFromSpace     The type of the from space in @p rhs.
 *
 *  @param[in] lhs The orbital space on the left side of the operator.
 *  @param[in] rhs The orbital space on the right side of the operator.
 *
 *  @return True if @p lhs equals @p rhs and false otherwise.
 */
template<typename LTransformType, typename LFromSpace, typename RTransformType,
         typename RFromSpace>
bool operator==(const DerivedSpace<LTransformType, LFromSpace>& lhs,
                const DerivedSpace<RTransformType, RFromSpace>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>)
        return false;
    else {
        return lhs.operator==(rhs);
    }
}

/** @brief Determines if two DerivedSpace instances are non-polymorphically
 *         different.
 *
 *  @relates DerivedSpace
 *
 *  @tparam LTransformType The type of the tranform in @p lhs.
 *  @tparam LFromSpace     The type of the from space in @p lhs.
 *  @tparam RTransformType The type of the transform in @p rhs.
 *  @tparam RFromSpace     The type of the from space in @p rhs.
 *
 *  @param[in] lhs The orbital space on the left side of the operator.
 *  @param[in] rhs The orbital space on the right side of the operator.
 *
 *  @return False if @p lhs equals @p rhs and true otherwise.
 */
template<typename LTransformType, typename LFromSpace, typename RTransformType,
         typename RFromSpace>
bool operator!=(const DerivedSpace<LTransformType, LFromSpace>& lhs,
                const DerivedSpace<RTransformType, RFromSpace>& rhs) {
    return !(lhs == rhs);
}

// ------------------------- Typedefs ------------------------------------------

using DerivedSpaceD   = DerivedSpace<type::tensor, AOSpaceD>;
using ToTDerivedSpace = DerivedSpace<type::tensor_of_tensors, AOSpaceD>;

// ---------------- Explicit Template Instantiations ---------------------------

extern template class DerivedSpace<type::tensor, AOSpaceD>;
extern template class DerivedSpace<type::tensor_of_tensors, AOSpaceD>;

} // namespace chemist::orbital_space
