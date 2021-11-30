#pragma once
#include "chemist/tensor/allocators/allocator.hpp"
#include "chemist/tensor/shapes/shape.hpp"

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
    using extents_type = typename parent_type::extents_type;

    using pimpl_pointer = typename parent_type::pimpl_pointer;

    using ta_shape = TA::SparseShape<float>;

    explicit ShapePIMPL(extents_type x = {}) : m_extents_(std::move(x)) {}

    /** @brief Makes a non-polymorphic deep copy of this instance.
     *
     *  This ctor will deep copy the state in the provided ShapePIMPL instance.
     *  If @p other is being passed polymorphically by the base class, then the
     *  resulting ShapePIMPL will be the ShapePIMPL slice of @p other.
     *
     *  @param[in] other The PIMPL instance being copied.
     *
     *  @throw std::bad_alloc if there is a problem copying the state. Strong
     *                        throw guarantee.
     */
    ShapePIMPL(const ShapePIMPL& other) = default;

    ShapePIMPL(ShapePIMPL&& other) = delete;

    ShapePIMPL& operator=(const ShapePIMPL& rhs) = delete;

    ShapePIMPL& operator=(ShapePIMPL&& rhs) = delete;

    virtual ~ShapePIMPL() noexcept = default;

    pimpl_pointer clone() const { return clone_(); }

    const extents_type& extents() const { return m_extents_; }

protected:
private:
    virtual pimpl_pointer clone_() const;

    extents_type m_extents_;
};

template<typename FieldType>
typename ShapePIMPL<FieldType>::pimpl_pointer ShapePIMPL<FieldType>::clone_()
  const {
    return std::make_unique<my_type>(*this);
}

extern template class ShapePIMPL<field::Scalar>;
extern template class ShapePIMPL<field::Tensor>;

} // namespace chemist::tensor::detail_
