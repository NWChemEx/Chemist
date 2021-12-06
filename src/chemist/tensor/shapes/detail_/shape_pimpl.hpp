#pragma once
#include "chemist/tensor/allocators/allocator.hpp"
#include "chemist/tensor/shapes/shape.hpp"
#include <pluginplay/pluginplay.hpp>

namespace chemist::tensor::detail_ {

/** @brief Base class for PIMPLs in the Shape hierachy.
 *
 *  The base PIMPL is suitable for tensors which are either dense or whose
 *  sparsity is determined upon filling in the tensor (assuming the tensor
 *  backend supports such a use case).
 *
 *  All PIMPLs in the Shape hierarchy are expected to inherit from this class.
 *  This instance defines the API that the Shape class will use to interact with
 *  the PIMPL. Derived instances of Shape will typically have a derived PIMPL
 *  instance associated with them. The derived PIMPL instances may define
 *  additional functions/state and the derived Shape instance may downcast to
 *  interact with that additional functions/state.
 *
 *  Derived classes should override the following functions as appropriate:
 *
 *  - clone_
 *  - hash_
 *
 *  @tparam FieldType The type of the elements in the tensor. Assumed to be
 *                    either field::Scalar or field::Tensor.
 */
template<typename FieldType>
class ShapePIMPL {
private:
    /// Type of the Shape class being implemented
    using parent_type = Shape<FieldType>;

    /// Type of this PIMPL
    using my_type = ShapePIMPL<FieldType>;

public:
    /// Type used to specify the lengths of each mode
    using extents_type = typename parent_type::extents_type;

    /// Type of a pointer to the base of the ShapePIMPL hierarchy
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Type TA uses for specifying the tile sparsity of a tensor
    using ta_shape = TA::SparseShape<float>;

    /** @brief Creates a new ShapePIMPL with the provided extents.
     *
     *
     *  @param[in] x The extents of each mode of the tensor. When the field is
     *               scalar @p x should specify the extents of each mode. When
     *               the field is tensor @p x should only specify the extents of
     *               the independent modes. Default is a default object of type
     *               `extents_type`.
     *
     *  @throw None No throw guarantee.
     */
    explicit ShapePIMPL(extents_type x = {}) : m_extents_(std::move(x)) {}

    /** @brief Makes a non-polymorphic deep copy of this instance.
     *
     *  This ctor will deep copy the state in the provided ShapePIMPL
     * instance. If @p other is being passed polymorphically by the base
     * class, then the resulting ShapePIMPL will be the ShapePIMPL slice of @p
     * other.
     *
     *  This ctor is public so that the Shape class can call it for its
     *  non-polymorphic copy.
     *
     *  @param[in] other The PIMPL instance being copied.
     *
     *  @throw std::bad_alloc if there is a problem copying the state. Strong
     *                        throw guarantee.
     */
    ShapePIMPL(const ShapePIMPL& other) = default;

    /// Deleted to avoid slicing
    ///@{
    ShapePIMPL(ShapePIMPL&& other) = delete;
    ShapePIMPL& operator=(const ShapePIMPL& rhs) = delete;
    ShapePIMPL& operator=(ShapePIMPL&& rhs) = delete;
    ///@}

    /// Default dtor
    virtual ~ShapePIMPL() noexcept = default;

    /** @brief Polymorphic deep copy.
     *
     *  This function makes a polymorphic deep copy of the current instance.
     *
     *  @return A deep copy of the current instance via a pointer to its
     *          ShapePIMPL base class.
     *
     *  @throw std::bad_alloc if there's a problem allocating memory. Strong
     *                        throw guarantee.
     */
    pimpl_pointer clone() const { return clone_(); }

    /** @brief Returns the lengths of each mode of the tensor.
     *
     *  The extents of a tensor are the lengths of each mode. This function
     *  returns the extents of all modes, when the field is scalar, and the
     *  extents of the independent modes, when the field is tensor.
     *
     *  @return A read-only reference to the tensor's extents.
     *
     *  @throw None No throw gurantee.
     */
    const extents_type& extents() const { return m_extents_; }

    /** @brief Non-polymorphic comparison.
     *
     *  This operator is used to compare a ShapePIMPL instance to another
     *  ShapePIMPL instance. The comparision only checks if the extents are the
     *  same in each instance, *i.e.*, if either instance is polymorphic, the
     *  comparison does not consider state outside the ShapePIMPL part of the
     *  object.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if @p rhs and the current instance have the same extents,
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ShapePIMPL& rhs) const noexcept;

    /** @brief Polymorphic hash operation.
     *
     *  This function will hash the entire state of the current instance,
     *  including any state in the derived classes. For this to work, the
     *  derived class must override `hash_`.
     *
     *  @param[in,out] h The hasher instance to use to hash this instance. After
     *                   this call the interal state of @p h will be updated
     *                   with a hash of the current instance.
     */
    void hash(pluginplay::Hasher& h) const { hash_(h); }

protected:
    /// To be overridden by the derived class to implement hash()
    virtual void hash_(pluginplay::Hasher& h) const { h(m_extents_); }

private:
    /// To be overridden by the derived class to implement clone()
    virtual pimpl_pointer clone_() const;

    /// The extents of the corresponding tensor
    extents_type m_extents_;
};

#define SHAPE_PIMPL ShapePIMPL<FieldType>

template<typename FieldType>
typename SHAPE_PIMPL::pimpl_pointer SHAPE_PIMPL::clone_() const {
    return pimpl_pointer(new my_type(*this));
}

template<typename FieldType>
bool SHAPE_PIMPL::operator==(const ShapePIMPL& rhs) const noexcept {
    return m_extents_ == rhs.m_extents_;
}

#undef SHAPE_PIMPL

extern template class ShapePIMPL<field::Scalar>;
extern template class ShapePIMPL<field::Tensor>;

} // namespace chemist::tensor::detail_
