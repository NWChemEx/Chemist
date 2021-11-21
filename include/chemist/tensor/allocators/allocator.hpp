#pragma once
#include "chemist/tensor/detail_/backends/tiled_array.hpp"
#include "chemist/tensor/fields.hpp"
#include "chemist/tensor/type_traits/nd_initializer_list_traits.hpp"
#include <memory>
#include <tiledarray.h>
#include <vector>

namespace chemist::tensor {

/** @brief Abstracts away the details of how the TensorWrapper's internal tensor
 *         is formed.
 *
 *  In TensorWrapper we basically treat the wrapped tensor as the memory of our
 *  tensor wrapper. Standard C++ practice is to have allocator classes for
 *  allocating/deallocating memory, hence we introduce a class hierarchy of
 *  allocators for allocating the wrapped tensor. Allocators are responsible for
 *  hiding the general set-up details of the wrapped tensor when given a size.
 *  Some examples of things the allocator can hide if we assume that TiledArray
 *  is the wrapped tensor library: distribution, tiling, and whether tiles are
 *  lazy.
 *
 *  The Allocator class defines the common API that all TensorWrapper allocators
 *  must have. We borrow from the C++ concept of allocator, but aren't too
 *  concerned with following it to the letter given that the tensors returned
 *  by the allocator aren't usable as memory in the C++ sense anyways (*i.e.*,
 *  even if we followed the API to the letter, you're not going to be doing
 *  something like: `std::vector<TA::DistArrayD, Allocator<DistArrayD>>`).
 *
 *  Adding A New Allocator Specialization
 *  =====================================
 *
 *  To create a new Allocator specialization of type U:
 *  1. Derive a class from Allocator<FieldType>. Depending on the details of
 *     your class it may make senese for your class to also be templated on
 *     additional details beside the field type.
 *  2. Implement `clone_()`. In most cases this implementation is just:
 *     ```
 *     allocator_ptr clone_() const overrride {
 *         return std::make_unique<U>(*this);
 *     }
 *     ```
 *     as long as the copy ctor works correctly.
 *  3. Implement `is_equal_()`. In most cases this is just:
 *     ```
 *     bool is_equal_(const Allocator<FieldType>& rhs) const noexcept
 *      override {
 *         const auto prhs = dynamic_cast<const U*>(&rhs);
 *         if(!prhs) return false;
 *         return *this == *prhs;
 *     }
 *     ```
 *     as long as the non-polymorphic comparison operator (*i.e.*, operator==)
 *     is implemented correctly.
 *  4. Implement make_tr_(). The implementation of this function is highly
 *     specific to  the derived class.
 *
 *  Realistically 4. should probably be made more general, but we're punting on
 *  that for now.
 */
template<typename FieldType>
class Allocator {
private:
    /// The type of this allocator
    using my_type = Allocator<FieldType>;

    /// The traits class for the backend
    using backend_traits = backends::TiledArrayTraits<FieldType>;

public:
    /// The type of object this allocator can make
    using value_type = typename backend_traits::variant_type;

    /// Unsigned integral type used to specify the extent of a mode
    using extent_type = std::size_t;

    /// Container-of-extents type used to specify the extents of all modes
    using extents_type = std::vector<extent_type>;

    /// Type used to specify the tilings of the tensor
    using tiled_range_type = TA::TiledRange;

    /// Pointer to the base class of the allocator hierarchy
    using allocator_ptr = std::unique_ptr<my_type>;

    /// Type of the object describing the runtime
    using runtime_type = TA::World;

    /// A read-write reference to the runtime object
    using runtime_reference = runtime_type&;

    /// Type of the scalars in the tensor
    using scalar_type = double;

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

    /** @brief Polymorphic comparison operator.
     *
     *  This function comares two Allocator instances polymorphically. Two
     *  Allocator instances are equal if they were both declared as the same
     *  type, and all parts of the class hierarchy compare equal.
     *
     *
     *  @param[in] other The allocator we are comparing to.
     *
     *  @return True if this is polymorphically equal to @p other. False
     *          otherwise.
     */
    bool is_equal(const Allocator& other) const;

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

    /** @brief Creates an unininitialized tensor with the specified shape.
     *
     *  This function combines the call to `make_tiled_range` with the
     *  construction of a tensor. In theory which of the variant alternatives is
     *  used can be controlled in some manner. At the moment we have hard-coded
     *  the choice to the first alternative.
     *
     *  @param[in] shape A container such that the i-th element is the extent of
     *                   the i-th mode of the resulting tensor. Extent lengths
     *                   are in elements, not tiles.
     *
     *  @return A variant containing the created tensor. Which of the variant
     *          choices is initialized is up to the allocator.
     */
    value_type new_tensor(const extents_type& shape) const;

    /** @brief Creates a tensor initialized with provided initializer list.
     *
     *
     *  @param[in] il The initializer list to use to initialize the tensor. The
     *                shape of the tensor will be deduced from the shape of the
     *                initializer list.
     *
     *  @note because of how C++ implements initializer lists, we must
     *  explicitly create a ctor for each scalar type and level of nesting,
     *  *i.e.*, we can not rely on templating to deduce them. We currently
     *  support scalars which are doubles and nestings from [1, 4] inclusive.
     *
     *  @throw std::runtime_error if the initializer list is jagged (*i.e.*, the
     *                            initializer lists at a given dept do not all
     *                            have the same extent). Strong throw guarantee.
     */
    ///@{
    value_type new_tensor(
      const n_d_initializer_list_t<scalar_type, 1>& il) const;

    value_type new_tensor(
      const n_d_initializer_list_t<scalar_type, 2>& il) const;

    value_type new_tensor(
      const n_d_initializer_list_t<scalar_type, 3>& il) const;

    value_type new_tensor(
      const n_d_initializer_list_t<scalar_type, 4>& il) const;
    ///@}

    /** @brief Non-polymorphically compares two Allocators for equality.
     *
     *  This is a non-polymorphic comparison of two Allocator instances, meaning
     *  that derived state is NOT considered in the comparison. The only state
     *  that an allocator possess is the runtime. Two Allocators are equivalent
     *  if they share the same runtime.
     *
     *  @todo When ParallelZone rolls out actually compare runtimes.
     *
     *  @param[in] rhs The Allocator instance we are comparing to.
     *
     *  @return True if this instance is equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const Allocator& rhs) const { return true; }

    /** @brief Non-polymorphically compares two Allocators for equality.
     *
     *  This is a non-polymorphic comparison of two Allocator instances, meaning
     *  that derived state is NOT considered in the comparison. The only state
     *  that an allocator possess is the runtime. Two Allocators are equivalent
     *  if they share the same runtime.
     *
     *  @todo When ParallelZone rolls out actually compare runtimes.
     *
     *  @param[in] rhs The Allocator instance we are comparing to.
     *
     *  @return False if this instance is equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const Allocator& rhs) const { return !((*this) == rhs); }

    /** @brief Provides access to the runtime to which the tensor will belong.
     *
     *  At the moment the runtime associated with the tensor is just a
     *  TiledArray World. This will change when ParallelZone rolls out.
     *
     *  @return A read/write reference to the runtime.
     *
     *  @throw None No throw guarantee.
     */
    runtime_reference runtime() const { return m_world_; }

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

    /** @brief Hook for polymorphically comparing two Allocators.
     *
     *  The derived class is responsible for checking that @p rhs is the same
     *  type as it (via a dynamic_cast). If @p rhs is the same type, then the
     *  derived class is also responsible for comparing all state between the
     *  current instance and @p rhs.
     *
     *  @param[in] rhs The allocator to compare to.
     *
     *  @return True if the current instance polymorphically
     *
     *  @throw None The derived class is responsible for making sure the
     *              comparison is done in a no-throw manner.
     */
    virtual bool is_equal_(const Allocator& rhs) const noexcept;

    /// The runtime being used for the allocation
    runtime_reference m_world_;
};

//------------------------------------------------------------------------------
//                         Inline Implementations
//------------------------------------------------------------------------------

template<typename FieldType>
Allocator<FieldType>::Allocator(runtime_reference world) : m_world_(world) {}

template<typename FieldType>
bool Allocator<FieldType>::is_equal(const Allocator& other) const {
    // Needs to be symmetrized to verify both have the same most-derived type
    return is_equal_(other) && other.is_equal_(*this);
}

template<typename FieldType>
typename Allocator<FieldType>::tiled_range_type
Allocator<FieldType>::make_tiled_range(const extents_type& shape) const {
    return make_tr_(shape);
}

template<typename FieldType>
typename Allocator<FieldType>::value_type Allocator<FieldType>::new_tensor(
  const extents_type& shape) const {
    return value_type(std::in_place_index<0>, m_world_,
                      make_tiled_range(shape));
}

template<typename FieldType>
typename Allocator<FieldType>::value_type Allocator<FieldType>::new_tensor(
  const n_d_initializer_list_t<scalar_type, 1>& il) const {
    const auto tr = make_tiled_range(il2extents<scalar_type, 1>(il));
    return value_type(std::in_place_index<0>, m_world_, tr, il);
}

template<typename FieldType>
typename Allocator<FieldType>::value_type Allocator<FieldType>::new_tensor(
  const n_d_initializer_list_t<scalar_type, 2>& il) const {
    const auto tr = make_tiled_range(il2extents<scalar_type, 2>(il));
    return value_type(std::in_place_index<0>, m_world_, tr, il);
}

template<typename FieldType>
typename Allocator<FieldType>::value_type Allocator<FieldType>::new_tensor(
  const n_d_initializer_list_t<scalar_type, 3>& il) const {
    const auto tr = make_tiled_range(il2extents<scalar_type, 3>(il));
    return value_type(std::in_place_index<0>, m_world_, tr, il);
}

template<typename FieldType>
typename Allocator<FieldType>::value_type Allocator<FieldType>::new_tensor(
  const n_d_initializer_list_t<scalar_type, 4>& il) const {
    const auto tr = make_tiled_range(il2extents<scalar_type, 4>(il));
    return value_type(std::in_place_index<0>, m_world_, tr, il);
}

template<typename FieldType>
bool Allocator<FieldType>::is_equal_(const Allocator& rhs) const noexcept {
    return *this == rhs;
}

} // namespace chemist::tensor
