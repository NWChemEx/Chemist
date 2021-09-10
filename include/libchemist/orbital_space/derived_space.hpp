#pragma once
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/orbital_space/dependent_space.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp" // For equality
#include <memory>

namespace libchemist::orbital_space {

/** @brief Models an orbital space defined in terms of another orbital space.
 *
 *  In practice most orbital spaces are defined as a transformation from another
 *  space. For example the MOs are usually defined as a linear combination of
 *  the AOs.
 *
 *  @tparam TransformType Type of the object holding the transformation
 *                        coefficients from FromSpace to this space.
 *  @tparam FromSpace The type of the object that the transformation is from.
 *                    Must be default constructable.
 *  @tparam BaseType  Type of the class from which this class is derived.
 */
template<typename TransformType, typename FromSpace, typename BaseType>
class DerivedSpace : public BaseType {
private:
    /// Type of an instance of this class
    using my_type = DerivedSpace<TransformType, FromSpace, BaseType>;

public:
    /// Type of the transformation matrix
    using transform_type = TransformType;

    /// Type of the orbital space this space is a transformation of
    using from_space_type = FromSpace;

    /// Type of a pointer to the from space
    using from_space_ptr = std::shared_ptr<const FromSpace>;

    /// Type of an offset or index
    using size_type = typename BaseType::size_type;

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
     * @tparam Args The types of the arguments which will be forwarded to the
     *              the base class's ctor.
     *
     * @param[in] C The transformation coefficients that take you from the
     *              orbital space `from_space` to this instance's orbital space.
     *              Default value is a default constructed instance of type
     *              `TransformType`.
     *
     * @param[in] from_space The orbital space in terms of which this space is
     *                       defined.
     * @param[in] args The arguments which will be forwarded to the base class's
     *                 ctor.
     *
     * @throw ??? If fowarding `args` throws. Same throw guarantee.
     */
    template<typename... Args>
    DerivedSpace(transform_type C, from_space_type from_space, Args&&... args);

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
    template<typename... Args>
    DerivedSpace(transform_type C, from_space_ptr pfrom_space, Args&&... args);

    /** @brief Initializes this instance by copying another DerivedSpace
     *
     *  This copy is a mix of shallow and deep. The transformation coefficients
     *  are deep copied, whereas the from space is shallow copied. The
     *  motivation is that by copying a DerivedSpace instance the user is
     *  rotating the orbitals (so same from space, different to space). Since
     *  under normal circumstances the from space is read-only there's no
     *  reason to copy it and we simply alias it.
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
     *  This copy is a mix of shallow and deep. The transformation coefficients
     *  are deep copied, whereas the from space is shallow copied. The
     *  motivation is that by copying a DerivedSpace instance the user is
     *  rotating the orbitals (so same from space, different to space). Since
     *  under normal circumstances the from space is read-only there's no
     *  reason to copy it and we simply alias it.
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
     *          space in a read/write format.
     */
    auto& C() { return m_C_; }

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
    const auto& C() const { return m_C_; }

    /** @brief Returns the space that this orbital space is defined in terms of.
     *
     *
     *  @return The orbital space that this space is defined in terms of in a
     *          read-only manner.
     *
     *  @throw std::runtime_error if this instance does not contain a from
     *                            space. Strong throw guarantee.
     */
    const auto& from_space() const;

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

protected:
    /// Type of the container used for passing multiple modes to transform
    using mode_container = typename BaseType::mode_container;

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
    virtual void hash_(pluginplay::Hasher& h) const override;

    /// Include the transformation and the from space in the comparison
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The transformation coefficients to this orbital space
    transform_type m_C_;

    /// The basis set that this space is defined in terms of
    from_space_ptr m_pbase_;
};

template<typename LTransformType, typename LFromSpace, typename LBaseType,
         typename RTransformType, typename RFromSpace, typename RBaseType>
bool operator==(
  const DerivedSpace<LTransformType, LFromSpace, LBaseType>& lhs,
  const DerivedSpace<RTransformType, RFromSpace, RBaseType>& rhs) {
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>)
        return false;
    else {
        const LBaseType& lbase = lhs;
        const LBaseType& rbase = rhs;
        if(lbase != rbase) return false;
        auto plhs = lhs.from_space_data();
        auto prhs = rhs.from_space_data();
        if(plhs && prhs) { // Both have from_spaces
            if(lhs.from_space() != rhs.from_space()) return false;
        } else if(plhs && !prhs)
            return false; // LHS has from, RHS doesn't
        else if(!plhs && prhs)
            return false; // RHS has from, LHS doesn't
        // else is both don't have from-space

        if(lhs.size() != rhs.size()) return false;
        return lhs.size() ? lhs.C() == rhs.C() : true;
    }
}

template<typename LTransformType, typename LFromSpace, typename LBaseType,
         typename RTransformType, typename RFromSpace, typename RBaseType>
bool operator!=(
  const DerivedSpace<LTransformType, LFromSpace, LBaseType>& lhs,
  const DerivedSpace<RTransformType, RFromSpace, RBaseType>& rhs) {
    return !(lhs == rhs);
}

// ------------------------- Typedefs ------------------------------------------

using DerivedSpaceD   = DerivedSpace<type::tensor, AOSpaceD, BaseSpace>;
using IndDerivedSpace = DerivedSpace<type::tensor, DepAOSpaceD, BaseSpace>;
using DepDerivedSpace =
  DerivedSpace<type::tensor_of_tensors, DepAOSpaceD, DependentSpace>;

// ----------------------- Inline Implementations ------------------------------

template<typename TransformType, typename FromSpace, typename BaseType>
template<typename... Args>
DerivedSpace<TransformType, FromSpace, BaseType>::DerivedSpace(
  transform_type C, from_space_type base, Args&&... args) :
  DerivedSpace(std::move(C), std::make_shared<from_space_type>(std::move(base)),
               std::forward<Args>(args)...) {}

template<typename TransformType, typename FromSpace, typename BaseType>
template<typename... Args>
DerivedSpace<TransformType, FromSpace, BaseType>::DerivedSpace(
  transform_type C, from_space_ptr pbase, Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_C_(std::move(C)), m_pbase_(pbase) {}

template<typename TransformType, typename FromSpace, typename BaseType>
const auto& DerivedSpace<TransformType, FromSpace, BaseType>::from_space()
  const {
    if(m_pbase_) return *m_pbase_;
    throw std::runtime_error("No from space set. Was this instance default "
                             "initialized or moved from?");
}

// ---------------- Explicit Template Instantiations ---------------------------

extern template class DerivedSpace<type::tensor, AOSpaceD, BaseSpace>;
extern template class DerivedSpace<type::tensor, DepAOSpaceD, BaseSpace>;
extern template class DerivedSpace<type::tensor_of_tensors, DepAOSpaceD,
                                   DependentSpace>;

} // namespace libchemist::orbital_space
