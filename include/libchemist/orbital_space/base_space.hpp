#pragma once
#include "libchemist/orbital_space/types.hpp"
#include <sde/detail_/memoization.hpp>

namespace libchemist::orbital_space {

/** @brief Common base class for all orbital spaces.
 *
 *  BaseSpace provides a generic API for working with an orbital space. In
 *  particular this API provides members for:
 *  - the number of orbitals via `size()`
 *  - transforming a tensor to the space `transform()`
 *  - hashing the orbital space `hash()`
 *  - comparing the orbital spaces `operator==` and `operator!=`
 */
class BaseSpace {
public:
    /** @brief Creates a BaseSpace
     *
     *  BaseSpace is an abstract class with no state so this ctor is a no-op.
     *
     *  @throw None no throw guarantee.
     */
    BaseSpace() = default;

    /** @brief Returns the number of orbitals in this space.
     *
     *  The size of an orbital space is the total number of orbitals in that
     *  space.
     *
     *  @throw ??? Throws if the derived class's implementation of size_ throws.
     *             Same throw guarantee.
     */
    type::size size() const { return size_(); }

    /** @brief Transforms the specified modes of a tensor to the orbital space.
     *
     *  This function takes an input tensor @p t and a list of mode indices,
     *  @p modes. For each index in @p modes, it will transform the
     *  corresponding mode of @p t to the orbital space described by this
     *  instance.
     *
     *  @tparam ModeTypes A template parameter pack consisting of 0 or more
     *                    integral types. Each type in @p ModeTypes must be
     *                    implicitly convertible to `orbital_space::type::size`.
     *
     *  @param t The tensor to be transformed
     *  @param modes The modes of @p t to transform.
     *
     *  @return @p t after transforming the modes specified by the list
     *          @p modes.
     *
     *  @throw ??? Throws if the derived class's implmentation of transform_
     *             throws. Same throw guarantee.
     */
    template<typename... ModeTypes>
    type::tensor_wrapper transform(const type::tensor_wrapper& t,
                                   ModeTypes&&... modes) const;

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
    void hash(sde::Hasher& h) const { hash_(h); }

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

private:
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
     *  @throw ??? Throws if the derived implementation throws. Same throw
     *             guarantee as the derived implementation.
     */
    virtual type::size size_() const = 0;

    /** @brief To be overridden by the derived class to implement transform().
     *
     *  `BaseSpace::transform` worries about the scenario when @p modes is empty
     *  so derived classes need only worry about when transformations must
     *  acutally be done. The implementation is responsible for ensuring that
     *  all of the specified modes are transformed before returing the result.
     *
     *  @param[in] t The tensor to transform.
     *  @param[in] modes A list of indices corresponding to the modes of @p t
     *                   which need to be transformed to this orbital space.
     *
     *  @return @p t After transforming each mode specified in @p modes.
     *
     *  @throw ??? Throws if the implementation throws. Same throw guarantee.
     */
    virtual type::tensor_wrapper transform_(
      const type::tensor_wrapper& t, const mode_container& modes) const = 0;

    /// Actually implements hash. Should be overridden by derived classes
    virtual void hash_(sde::Hasher& h) const = 0;

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
    bool equal_(const BaseSpace& rhs) const noexcept = 0;
};

//------------------------- Implementations -----------------------------------
template<typename... ModeTypes>
type::tensor_wrapper BaseSpace::transform(const type::tensor_wrapper& t,
                                          ModeTypes... modes) const {
    constexpr bool do_transform = sizeof...(ModeTypes);
    std::vector<type::size> mode_v{modes...};
    return do_transform ? transform_(t, mode_v) : t;
}

bool BaseSpace::equal(const BaseSpace& rhs) const noexcept {
    return equal_(rhs) && rhs == *this;
}

template<typename DerivedType>
bool BaseSpace::equal_common(const DerivedType& lhs,
                             const BaseSpace& rhs) const noexcept {
    auto prhs = dynamic_cast<const DerivedType*>(rhs);
    return prhs == nullptr ? false : lhs == *(prhs);
}

} // namespace libchemist::orbital_space
