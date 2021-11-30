#pragma once
#include "chemist/tensor/detail_/backends/tiled_array.hpp"
#include "chemist/tensor/fields.hpp"
#include "chemist/tensor/tensor_fwd.hpp"
#include <memory>
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

    /// Type of a pointer to this class
    using pointer_type = std::unique_ptr<my_type>;

    using tensor_type = typename backend_type::variant_type;

    Shape() noexcept;

    Shape(extents_type extents);

    virtual ~Shape() noexcept;

    pointer_type clone() const { return clone_(); }

    extents_type extents() const;

    /** @brief Given an allocator, this function will create a tensor consistent
     *         with the present Shape instance.
     *
     *
     *  @param[in] p The allocator to use for making the tensor.
     *
     *  @throw std::runtime_error if this instance does not contain a PIMPL.
     *                            Strong throw guarantee.
     */
    tensor_type make_tensor(const_allocator_reference p) const;

protected:
    Shape(pimpl_pointer pimpl) noexcept;

    Shape(const Shape& other);

    Shape(Shape&& other) noexcept = delete;

    Shape& operator=(const Shape& rhs) = delete;

    Shape& operator=(Shape&& rhs) noexcept = delete;

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
    virtual pointer_type clone_() const;

    virtual tensor_type make_tensor_(const_allocator_reference p) const;

    /// Object actually holding the state of the instance
    pimpl_pointer m_pimpl_;
};

template<typename FieldType>
typename Shape<FieldType>::tensor_type Shape<FieldType>::make_tensor(
  const_allocator_reference p) const {
    return make_tensor_(p);
}

extern template class Shape<field::Scalar>;
extern template class Shape<field::Tensor>;

} // namespace chemist::tensor
