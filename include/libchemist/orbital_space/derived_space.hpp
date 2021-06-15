#pragma once
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/orbital_space/dependent_space.hpp"
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

    /** @brief Creates a new DerivedSpace_ which has no transformation
     *         or from space.
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
     * @throw ??? If moving `C`, moving `from_space`, or fowarding `args`
     *            throws. Same throw guarantee.
     */
    template<typename... Args>
    DerivedSpace(transform_type C, from_space_type from_space, Args&&... args);

    /** @brief Creates a new DerivedSpace_ with the specified state.
     *
     *  This ctor is designed to be used when the "from space" instance has
     *  already been created and is in use by another "to space". For example
     *  one would use this ctor to construct the virtual MO space using the AO
     *  space stored in the already constructed occupied MO space. Note that if
     *  the "from space" is modified elsewhere the instance aliased within this
     *  class will reflect those changes.
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
     * @throw ??? If moving `C` or forwarding `args` throws. Same throw
     *            guarantee.
     */
    template<typename... Args>
    DerivedSpace(transform_type C, from_space_ptr pfrom_space, Args&&... args);

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
     *
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
     */
    const auto& from_space() const { return *m_pbase_; }

    /** @brief Returns the shared_ptr holding the from space
     *
     *  @return The shared_ptr holding the from space.
     */
    auto from_space_data() const { return m_pbase_; }

protected:
    using mode_container = typename BaseType::mode_container;
    virtual size_type size_() const noexcept override;
    virtual type::tensor_wrapper transform_(const type::tensor_wrapper& t,
                                            const mode_container& modes) const;

    /// Include the transformation and the from space in the hash
    virtual void hash_(sde::Hasher& h) const override;

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
        auto lstate = std::tie(lhs.C(), lhs.from_space());
        auto rstate = std::tie(rhs.C(), rhs.from_space());
        return lstate == rstate;
    }
}

template<typename LTransformType, typename LFromSpace, typename LBaseType,
         typename RTransformType, typename RFromSpace, typename RBaseType>
bool operator!=(
  const DerivedSpace<LTransformType, LFromSpace, LBaseType>& lhs,
  const DerivedSpace<RTransformType, RFromSpace, RBaseType>& rhs) {
    return !(lhs == rhs);
}

using DerivedSpaceD = DerivedSpace<type::tensor<double>, AOSpaceD, BaseSpace>;
using IndDerivedSpaceD =
  DerivedSpace<type::tensor<double>, DepAOSpaceD, BaseSpace>;
using DepDerivedSpaceD =
  DerivedSpace<type::tensor_of_tensors<double>, DepAOSpaceD, DependentSpace>;

extern template class DerivedSpace<type::tensor<double>, AOSpaceD, BaseSpace>;
extern template class DerivedSpace<type::tensor<double>, DepAOSpaceD,
                                   BaseSpace>;
extern template class DerivedSpace<type::tensor_of_tensors<double>, DepAOSpaceD,
                                   DependentSpace>;

} // namespace libchemist::orbital_space
