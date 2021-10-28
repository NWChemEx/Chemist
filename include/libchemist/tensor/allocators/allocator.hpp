#pragma once
#include <memory>
#include <tiledarray.h>
#include <vector>

namespace libchemist::tensor {

/** @brief Abstracts away the details of how the TensorWrapper's internal tensor
 *         is formed.
 *
 *  In TensorWrapper we basically treat the wrapped tensor as the memory of our
 *  tensor wrapper. Standard C++ practice is to have allocator classes for
 *  allocating/deallocating memory, hence we introduce a class hierarchy of
 *  allocators for allocating the wrapped tensor.
 *
 *  The Allocator class defines the common API that all TensorWrapper allocators
 *  must have. We borrow from the C++ concept of allocator, but aren't too
 *  concerned with following it to the letter given that the tensors returned
 *  by the allocator aren't usable as memory anyways (thus even if we followed
 *  the API to the letter, you're not going to be doing something like:
 *  `std::vector<TA::DistArrayD, Allocator<DistArrayD>>`).
 */
class Allocator {
public:
    /// Unsigned integral type used to specify the extent of a mode
    using extent_type = std::size_t;

    /// Container-of-extents type used to specify the extents of all modes
    using extents_type = std::vector<extent_type>;

    /// Type used to specify the tilings of the tensor
    using tiled_range_type = TA::TiledRange;

    /// Pointer to the base class of the allocator hierarchy
    using allocator_ptr = std::unique_ptr<Allocator>;

    /// Type of the object describing the runtime
    using runtime_type = TA::World;

    /// A read-write reference to the runtime object
    using runtime_reference = runtime_type&;

    /** @brief Creates a new allocator with the optionally specified runtime.
     *
     *  Allocator is a pure-virtual class, so instances of it can not actually
     *  be created. Instead the ctor provides derived classes with an API for
     *  setting the runtime.
     *
     *  @param[in] world The runtime the allocator will use for allocating.
     *                   Defaults to `TA::get_default_world()`.
     *
     *  @throw None No throw guarantee.
     */
    explicit Allocator(runtime_reference world = TA::get_default_world());

    /** @brief Polymorphic copy.
     *
     *  The `clone()` member allows users of an Allocator to create polymorphic
     *  copies (*i.e.*, copies that include the derived state too) through the
     *  base class. This is important since the TensorWrapper class interacts
     *  with Allocator hierarchies through the base class.
     *
     *  @return A deep-, polymoprhic-copy of the current allocator.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        copy. Strong throw guarantee.
     */
    allocator_ptr clone() const { return clone_(); }

    /// Standard default dtor
    virtual ~Allocator() noexcept = default;

    /** @brief Creates a TiledRange for the given shape.
     *
     *  This is the public API for requesting a tiled range for a tensor.
     *  Exactly how @p shape gets mapped to a tiled range is left up to the
     *  derived class.
     *
     *  @param[in] shape A container such that the i-th element is the extent of
     *                   the i-th mode of the resulting tensor. Extent lengths
     *                   are in elements, not tiles.
     *
     *  @return The tiled range the TensorWrapper should use to allocate its
     *          internal tensor.
     */
    tiled_range_type make_tiled_range(const extents_type& shape) const;

protected:
    /// To help derived classes implement clone_
    Allocator(const Allocator&) = default;

private:
    /// Deleted to avoid slicing
    Allocator(Allocator&&) = delete;

    /// Deleted to avoid slicing
    Allocator& operator=(const Allocator&) = delete;

    /// Deleted to avoid slicing
    Allocator& operator=(Allocator&&) = delete;

    /** @brief Hook for polymorphic copy.
     *
     *  The clone_ member needs to be implemented by the derived class such that
     *  it returns an object of type allocator_ptr that is a deep copy of the
     *  current instance.
     *
     *  @throw std::bad_alloc The derived class may throw std::bad_alloc if a
     *                        problem arises making the deep copy. The derived
     *                        class is responsible for ensuring that if an
     *                        exception does arise, this function obeys a strong
     *                        throw guarantee.
     */
    virtual allocator_ptr clone_() const = 0;

    /** @brief Hook for making a tiled range
     *
     *  @param[in] shape The instance provided to make_tiled_range.
     *
     *  @return The tiled range instance the derived class maps @p shape to.
     *          The details depend on the derived class.
     *
     *  @throw std::bad_alloc The derived class may throw std::bad_alloc if a
     *                        problem arises while allocating any intermediate
     *                        buffers or the final result. If an exception is
     *                        thrown the derived class is responsible for
     *                        ensuring that this function obeys a strong throw
     *                        gurantee.
     */
    virtual tiled_range_type make_tr_(const extents_type& shape) const = 0;

    /// The runtime being used for the allocation
    runtime_reference m_world_;
};

//------------------------------------------------------------------------------
//                         Inline Implementations
//------------------------------------------------------------------------------

inline Allocator::Allocator(runtime_reference world) : m_world_(world) {}

inline typename Allocator::tiled_range_type Allocator::make_tiled_range(
  const extents_type& shape) const {
    return make_tr_(shape);
}

} // namespace libchemist::tensor
