#pragma once
#include "chemist/tensor/detail_/backends/tiled_array.hpp"
#include "chemist/tensor/fields.hpp"
#include "chemist/tensor/tensor_fwd.hpp"
#include <memory>
#include <pluginplay/pluginplay.hpp>
#include <vector>

namespace chemist::tensor {
namespace detail_ {
template<typename FieldType>
class ShapePIMPL;

} // namespace detail_

/** @brief Object describing a tensor's elemental layout.
 *
 *  The shape of a tensor is usually taken to mean the number of modes, and
 *  the lengths of each of said modes. Here we generalize a bit and also
 *  include the sparsity as part of the shape.
 *
 *  The Shape class is a complete class on its own; however, it is possible to
 *  customize the behavior by deriving from this class and overriding:
 *
 *  - clone_
 *  - make_tensor_
 *
 *  @tparam FieldType The types of the elements in the tensor. Expected to
 *                    be either Scalar or Tensor.
 */
template<typename FieldType>
class Shape {
private:
    /// Type of this instance
    using my_type = Shape<FieldType>;

    /// Type of the PIMPL associated with this instance
    using pimpl_type = detail_::ShapePIMPL<FieldType>;

    /// Type of the backend
    using backend_type = backends::TiledArrayTraits<FieldType>;

    /// Determines if @p T is a different field type than @p FieldType
    template<typename T>
    static constexpr bool not_my_field_v = !std::is_same_v<FieldType, T>;

    /// Enables a function when @p T is a different field than @p FieldType
    template<typename T>
    using enable_if_not_my_field_t = std::enable_if_t<not_my_field_v<T>>;

public:
    /// Type of a pointer to a PIMPL instance
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of the field the associated tensor is over
    using field_type = FieldType;

    /// Base type of an allocator associated with the same tensor
    using allocator_type = Allocator<field_type>;

    /// Read-only reference to an allocator
    using const_allocator_reference = const allocator_type&;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Type used to provide/return extents
    using extents_type = std::vector<size_type>;

    /// Type of a read-only reference to the extents
    using const_extents_reference = const extents_type&;

    /// Type of a pointer to this class
    using pointer_type = std::unique_ptr<my_type>;

    /// How make_tensor returns the resulting tensor
    using tensor_type = typename backend_type::variant_type;

    /** @brief Creates a shape with no extents.
     *
     *  This ctor creates a Shape object with no PIMPL. The resulting object is
     *  primarily usable as a temporary and has no state unless it is assigned
     *  to.
     *
     *  @throw None No throw guarantee.
     */
    Shape() noexcept;

    /** @brief Creates a shape with the provided extents.
     *
     *  Assuming @p extents is `r` elements long, the shape which results from
     *  this call describes:
     *
     *  - A rank `r` tensor such that the `i`-th mode has a length of
     *    `extents[i]` (FieldType == field::Scalar), or
     *  - A tensor with `r` independent modes such that the `i`-the independent
     *    mode has a length of `extents[i]` (FieldType == field::Tensor)
     *
     *  @param[in] extents How long each mode of the corresponding tensor is.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    explicit Shape(extents_type extents);

    /// Defaulted dtor
    virtual ~Shape() noexcept;

    /** @brief Polymorphic deep-copy.
     *
     *  This function makes a deep-copy of the current instance. The deep copy
     *  includes all state in the Shape class as well as in any derived classes.
     *
     *  @return A deep copy of this instance, returned as a pointer to the base
     *          class.
     *
     *  @throw std::bad_alloc if there is a problem allocating the new instance.
     *                        Strong throw guarantee.
     */
    pointer_type clone() const { return clone_(); }

    /** @brief Returns the extents of the tensor described by this Shape.
     *
     *  The extents describe the length of each of the tensor's (independent)
     *  modes. Note that for `FieldType == field::Scalar` all modes are
     *  independent.
     *
     *  @return For a tensor with `r` independent modes the resulting object is
     *          an `r` element random-access container, such that the `i`-th
     *          element is the length of the `i`-th mode of the tensor.
     *
     *  @throw std::runtime_error if the instance does not have a PIMPL. Strong
     *                            throw guarantee.
     */
    const_extents_reference extents() const;

    /** @brief Given an allocator, this function will create a tensor consistent
     *         with the present Shape instance.
     *
     *  The default implementation creates a dense tensor with the specified
     *  shape. Derived classes may override this behavior, to for example, take
     *  into account sparsity.
     *
     *  @param[in] p The allocator to use for making the tensor.
     *
     *  @throw std::runtime_error if this instance does not contain a PIMPL.
     *                            Strong throw guarantee.
     */
    tensor_type make_tensor(const_allocator_reference p) const;

    /** @brief Non-polymorphic equality comparison for shapes with the same
     *         field.
     *
     *  This function will determine if the Shape part of the current instance
     *  is equal to the Shape part of @p rhs. In other words, state in derived
     *  classes, for either this instance or @p rhs, is not considered. Two
     *  Shapes are considered equal if they have the same field, both contain a
     *  PIMPL (or both don't contain a PIMPL), and if they do contain a PIMPL
     *  the PIMPL must have the same extents.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if the Shape part of this instance compares equal to the
     *          Shape part of @p rhs, false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Shape& rhs) const noexcept;

    /** @brief Non-polymorphic equality comparison for Shapes with different
     *         fields.
     *
     *  This function always returns false since Shapes with different field
     *  types can not be equal.
     *
     *  @tparam RHSType The field type of rhs. Assumed to be either
     *                  field::Scalar or field::Tensor.
     *  @tparam <anonymous> Type used to enable this function, via SFINAE, when
     *                      @p RHSType is different than FieldType.
     *
     *  @return False. This function is only considered when the field types are
     *          different.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSType, typename = enable_if_not_my_field_t<RHSType>>
    bool operator==(const Shape<RHSType>& rhs) const noexcept {
        return false;
    }

    /** @brief Polymorphic hash function.
     *
     *  This function will hash the entire state (including that in any derived
     *  classes).
     *
     *  @param[in,out] h The instance to use for hashing. After this call, the
     *                   internal state of @p h will be updated to include the
     *                   hash of the current Shape instance.
     */
    void hash(pluginplay::Hasher& h) const;

protected:
    /** @brief Creates the Shape with the specified PIMPL
     *
     *  This ctor is intended for use by derived classes. In the derived class
     *  ctors, they will initialize a PIMPL and then the base will store it via
     *  this ctor.
     *
     *  @param[in] pimpl The PIMPL the base should store. The PIMPL may be
     *                   polymorphic.
     *
     *  @throw None No throw guarantee.
     */
    Shape(pimpl_pointer pimpl) noexcept;

    /// Deleted to avoid slicing
    ///@{
    Shape(const Shape& other) = delete;

    Shape(Shape&& other) noexcept = delete;

    Shape& operator=(const Shape& rhs) = delete;

    Shape& operator=(Shape&& rhs) noexcept = delete;
    ///@}

    /** @brief Returns the PIMPL in a read-only state.
     *
     *  This function will assert that the instance has a PIMPL (throwing if it
     *  does not) and then return the PIMPL if it does.
     *
     *  @return A read-only reference to the PIMPL.
     *
     *  @throw std::runtime_error if the current instance is PIMPL-less. Strong
     *                            throw guarantee.
     */
    const pimpl_type& pimpl_() const;

    /** @brief Raises an exception if the Shape instance does not contain a
     *         PIMPL.
     *
     *  This function is code factorization for determining if the instance has
     *  a PIMPL, or not. As long as the Shape was not default constructed, or
     *  moved from, it should have a PIMPL.
     *
     *  @throw std::runtime_error if the current instance is PIMPL-less. Strong
     *                            throw guarantee.
     */
    void assert_pimpl_() const;

    /** @brief Determines if the instance has a PIMPL or not.
     *
     *  @return True if the instance has a PIMPL and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_pimpl_() const noexcept;

private:
    /// Derived class should override to implement polymorphic deep-copy
    virtual pointer_type clone_() const;

    /// Derived class should override to implement make_tensor
    virtual tensor_type make_tensor_(const_allocator_reference p) const;

    /// Object actually holding the state of the instance
    pimpl_pointer m_pimpl_;
};

/** @brief Determines if two Shape instances are different.
 *
 *  @relates Shape
 *
 *  This function simply negates a call to Shape::operator==. Hence it returns
 *  false when the Shape instances compare equal (non-polymorphically) and true
 *  otherwise. See Shape::operator== for the definition of non-polymorphic
 *  equality.
 *
 *  @tparam LHSType The type of the field associated with @p lhs. Assumed to be
 *                  either field::Scalar or field::Tensor.
 *  @tparam RHSType The type of the field associated with @p rhs. Assumed to be
 *                  either field::Scalar or field::Tensor.
 *
 *  @param[in] lhs The instance on the left side of the inequality operator.
 *  @param[in] rhs The instance on the right side of the inequality operator.
 *
 *  @return False if @p lhs compares non-polymorphically equal to @p rhs and
 *          true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSType, typename RHSType>
bool operator!=(const Shape<LHSType>& lhs, const Shape<RHSType>& rhs) {
    return !(lhs == rhs);
}

//------------------------------------------------------------------------------
//                    Out of Line Inline Implementations
//------------------------------------------------------------------------------

template<typename FieldType>
typename Shape<FieldType>::tensor_type Shape<FieldType>::make_tensor(
  const_allocator_reference p) const {
    return make_tensor_(p);
}

extern template class Shape<field::Scalar>;
extern template class Shape<field::Tensor>;

} // namespace chemist::tensor
