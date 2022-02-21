#pragma once
#include "chemist/detail_/hashing.hpp"
#include "chemist/types.hpp"

namespace chemist::orbital_space {

/** @brief Common base class for all orbital spaces.
 *
 *  BaseSpace provides a generic API for working with an orbital space. In
 *  particular this API provides members for:
 *  - the number of orbitals via `size()`
 *  - transforming a tensor to the space `transform()`
 *  - hashing the orbital space `hash()`
 *  - polymorphically comparing the orbital spaces `equal()` and `not_equal()`
 */
class BaseSpace {
public:
    /// Type used for indexing and offsets
    using size_type = type::size;

    /// Default polymorphic dtor
    virtual ~BaseSpace() noexcept = default;

    /** @brief Returns the number of orbitals in this space.
     *
     *  The size of an orbital space is the total number of orbitals in that
     *  space.
     *
     *  @throw None No throw guarantee.
     */
    auto size() const { return size_(); }

    /** @brief Hashes the current instance.
     *
     *  @param[in,out] h The hasher instance to use for hashing. The internal
     *                   state of h will be modified so that its internal hash
     *                   includes state information about this BaseSpace_
     *                   instance.
     *
     *  @throw ??? Throws if the derived class's implementation of `hash_`
     *             throws. Same throw guarantee.
     */
    void hash(chemist::detail_::Hasher& h) const { hash_(h); }

    /** @brief Polymorphically compares two orbital spaces to determine if they
     *         are equal.
     *
     *  Exactly what it means for two orbital spaces to be equal depends on what
     *  the most derived classes of the spaces are. In general two orbital
     *  spaces are equal if they:
     *  - have the same most derived class
     *    - e.g., a `DerivedSpace` instance and a `CanonicalSpace` compared
     *      through a common base class are not equal.
     *  - contain the same number of orbitals
     *  - the orbitals are ordered the same
     *  - the parameters associated with the orbitals are the same
     *
     *  Note that all floating-point comparisons are exact with no threshold
     *  tolerance. Meaning a parameter of 1.000000 is different than 1.000001.
     *
     *  @param[in] rhs The orbital space we are comparing to.
     *
     *  @return True if this orbital space is equivalent to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool equal(const BaseSpace& rhs) const noexcept;

    /** @brief Polymorphically compares two orbital spaces to determine if they
     *         are different.
     *
     *  @relates BaseSpace
     *
     *  Exactly what it means for two orbital spaces to be equal depends on what
     *  the most derived classes of the spaces are. In general two orbital
     *  spaces are equal if they:
     *  - have the same most derived class
     *    - e.g., a `DerivedSpace` instance and a `CanonicalSpace` compared
     *      through a common base class are not equal.
     *  - contain the same number of orbitals
     *  - the orbitals are ordered the same
     *  - the parameters associated with the orbitals are the same
     *
     *  Note that all floating-point comparisons are exact with no threshold
     *  tolerance. Meaning a parameter of 1.000000 is different than 1.000001.
     *
     *  @param[in] rhs The orbital space we are comparing to.
     *
     *  @return False if this orbital spaces is equivalent to @p rhs and true
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool not_equal(const BaseSpace& rhs) const noexcept { return !equal(rhs); }

protected:
    /// Type of a container of mode indices
    using mode_container = std::vector<type::size>;

    /** @brief Creates a BaseSpace
     *
     *  Users will never directly create a BaseSpace instance because it is an
     *  abstract class. The ctor is made protected to make this more clear.
     *  BaseSpace has no state so this ctor is a no-op.
     *
     *  @throw None no throw guarantee.
     */
    BaseSpace() = default;

    /** @brief Makes this `BaseSpace` instance by copying @p rhs.
     *
     *  The copy ctor is protected to avoid accidental slicing. `BaseSpace` has
     *  no state so the copy ctor is a no-op.
     *
     *  @param[in] rhs The instance being copied.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace(const BaseSpace& rhs) = default;

    /** @brief Makes this `BaseSpace` instance by taking state from @p rhs.
     *
     *  The move ctor is protected to avoid accidental slicing. `BaseSpace` has
     *  no state so the move ctor is a no-op.
     *
     *  @param[in,out] rhs The instance being moved from.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace(BaseSpace&& rhs) = default;

    /** @brief Replaces this instance's state with a copy of the state in
     *         @p rhs.
     *
     *  The copy assignment operator is protected to avoid accidental slicing.
     *  `BaseSpace` has no state so the move assignment is a no-op.
     *
     *  @param[in] rhs The instance being copied from.
     *
     *  @return The current instance, unchanged.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace& operator=(const BaseSpace& rhs) = default;

    /** @brief Replaces this instance's state by taking state from @p rhs.
     *
     *  The move assignment operator is protected to avoid accidental slicing.
     *  `BaseSpace` has no state so the move assignment is a no-op.
     *
     *  @param[in,out] rhs The instance being moved from.
     *
     *  @return The current instance, unchanged.
     *
     *  @throw None No throw gurantee.
     */
    BaseSpace& operator=(BaseSpace&& rhs) = default;

    /** @brief Implements polymorphic comparisons of orbital spaces.
     *
     *  `equal_common` takes care of:
     *  - ensuring that this instance's derived type appears in @p rhs's class
     *    hierarchy (ensuring that it is also @p rhs's most derived type is
     *    accomplished by requiring equal to commute).
     *  - calling `DerivedType::operator==` to actually compare the
     *    `DerivedType` instances.
     *
     *  @tparam DerivedType The type of the class being compared to @p rhs.
     *          Assumed to be derived from `BaseSpace`.
     *
     *  @param[in] lhs When called from the derived class should be `*this`.
     *  @param[in] rhs The orbital space we are compring to @p lhs.
     *
     *  @return True if @p rhs can be downcast to `DerivedType` and if the
     *          resulting `DerivedType` instance compares equal to @p lhs; false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename DerivedType>
    bool equal_common(const DerivedType& lhs,
                      const BaseSpace& rhs) const noexcept;

    /** @brief To be overridden by the derived class to implement size().
     *
     *  The derived class is responsible for ensuring this function returns the
     *  correct size for its respective space.
     *  - For AOSpace this is the number of AOs (radial plus angular, not just
     *    radial).
     *  - For DependentSpace this is the sum of the sizes of the orbital spaces
     *    for each independent index.
     *  - For DerivedSpace derived directly from BaseSpace (as opposed to
     *    derived from `DependentSpace`) this is simply the number of orbitals.
     *
     *  @return The number of orbitals in the space.
     *
     *  @throw None No throw guarantee.
     */
    virtual size_type size_() const noexcept = 0;

    /// Actually implements hash. Should be overridden by derived classes
    virtual void hash_(chemist::detail_::Hasher& h) const {};

    /** @brief To be overridden by the derived class to implement `equal`
     *
     *  Assuming that each derived class implements an appropriate `operator==`,
     *  `equal_` is implemented simply by calling the `equal_common` member:
     *
     *  @code
     *  bool equal_(const BaseType& rhs) const noexcept {
     *      return equal_common(*this, rhs);
     *  }
     *  @endcode
     *
     *  @note This little bit of boilerplate is required because otherwise, and
     *        without resorting to CRTP (thereby eliminating the common base
     *        class), BaseSpace does not know the type of the derived class.
     *
     *  @param[in] rhs The orbital space we are comparing to.
     *
     *  @return True if this orbital space is equivalent to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    virtual bool equal_(const BaseSpace& rhs) const noexcept = 0;
};

/** @brief Compares two BaseSpace instances for equality.
 *
 *  @relates BaseSpace
 *
 *  This comparison is not done polymorphically (use `BaseSpace::equal` for
 *  that), rather it compares the state unique to the BaseSpace part of the
 *  class hierarchy. Since there is no state unique in the BaseSpace part this
 *  function always returns true.
 *
 *  @note This function is defined so that derived classes do not need to worry
 *        about whether they derive directly from BaseSpace.
 *
 *  @param[in] <anonymous> The instance on the left of the equality operator.
 *  @param[in] <anonymous> The instance on the right of the equality operator.
 *
 *  @return True for all BaseSpace instances.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator==(const BaseSpace&, const BaseSpace&) { return true; }

/** @brief Determines if two BaseSpace instances are different.
 *
 *  @relates BaseSpace
 *
 *  This comparison is not done polymorphically (use `BaseSpace::equal` for
 *  that), rather it compares the state unique to the BaseSpace part of the
 *  class hierarchy. Since there is no state unique in the BaseSpace part this
 *  function always returns false.
 *
 *  @note This function is defined so that derived classes do not need to worry
 *        about whether they derive directly from BaseSpace.
 *
 *  @param[in] <anonymous> The instance on the left of the inequality operator.
 *  @param[in] <anonymous> The instance on the right of the inequality operator.
 *
 *  @return False for all BaseSpace instances.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const BaseSpace&, const BaseSpace&) { return false; }

//------------------------- Implementations -----------------------------------

inline bool BaseSpace::equal(const BaseSpace& rhs) const noexcept {
    return equal_(rhs) && rhs.equal_(*this);
}

template<typename DerivedType>
bool BaseSpace::equal_common(const DerivedType& lhs,
                             const BaseSpace& rhs) const noexcept {
    auto prhs = dynamic_cast<const DerivedType*>(&rhs);
    return prhs == nullptr ? false : lhs == *(prhs);
}

} // namespace chemist::orbital_space
