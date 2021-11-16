#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"

namespace libchemist::tensor::detail_ {

/** @brief Holds the state of a TensorWrapper.
 *
 *  The TensorWrapperPIMPL class is designed to encapsulate as much of the
 *  TensorWrapper state as possible. Full encapsulation is not possible unless
 *  we want to supress the expression templating of the wrapped library. For
 *  TiledArray this "leakage" occurs through the annotate member (and also
 *  somewhat through the ctor).
 *
 *  @tparam FieldType What sort of elements are in the tensor? Assumed to be
 *                    either field::Scalar or field::Tensor.
 */
template<typename FieldType>
class TensorWrapperPIMPL {
private:
    /// The type of this PIMPL
    using my_type = TensorWrapperPIMPL<FieldType>;

    /// Type of the TensorWrapper this is a PIMPL for
    using parent_type = TensorWrapper<FieldType>;

    /// Traits class used to get types associated with the field
    using field_traits = FieldTraits<FieldType>;

public:
    /// Type used to annotate the tensor (i.e. type of the dummy indices)
    using annotation_type = typename parent_type::annotation_type;

    /// Type of the field the tensor is over
    using field_type = FieldType;

    /// Type which wraps all of the possible backend tensors
    using variant_type = typename field_traits::variant_type;

    /// Type which results from labeling the possible backend
    using labeled_variant_type = typename field_traits::labeled_variant_type;

    /// Type of a read-only labeled backend
    using const_labeled_type =
      typename field_traits::const_labeled_variant_type;

    /// Type of a read-only reference to an allocator
    using const_allocator_reference =
      typename parent_type::const_allocator_reference;

    /// Type of a type-erased allocator
    using allocator_pointer = typename parent_type::allocator_pointer;

    /// These will be added back in a forthcoming PR
    // using shape_type                = Shape<field_type>;
    // using shape_pointer             = typename shape_type::shape_ptr;

    /// Type used to describe the shape (will be removed in forthcoming PR)
    using extents_type = typename parent_type::extents_type;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Type used to return ranks
    using rank_type = typename parent_type::rank_type;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    /// Type used for initializer lists of sizes
    using il_type = typename parent_type::il_type;

    /** @brief Creates a new PIMPL which wraps the provided tensor and allocator
     *
     *
     *  @param[in] v A variant which wraps the tensor
     *  @param[in] p The allocator to use for tiling, distribution, etc.
     */
    TensorWrapperPIMPL(variant_type v, allocator_pointer p);

    /** @brief Makes a deep-copy of the current PIMPL
     *
     *  Strictly speaking this calls the clone member of the allocator and the
     *  copy ctor of the variant. Whether that's a deep copy or shallow copy is
     *  defined by those functions.
     *
     *  @return A new PIMPL which is a "deep-copy" of this
     */
    pimpl_pointer clone() const;

    /** @brief Returns the allocator in a read-only state.
     *
     *  This function can be used to retrieve the allocator
     * that the TensorWrapper was initialized with. If the
     * instance does not have an allocator an error will be
     * thrown.
     *
     *  @return The allocator used for the tensor.
     *
     *  @throw std::runtime_error if the instance does not
     */
    const_allocator_reference allocator() const;

    /** @brief Annotates the modes of the wrapped index with
     * the provided labels.
     *
     *  The domain-specific language of tensor operations is
     * written in terms of Einstein notation. This requires us
     * to assign dummy indices to each mode of the tensor. This
     * function pairs @p annotation with the wrapped tensor to
     * provide a labeled tensor. The resulting labeled tensor
     * can be used in tensor expressions.
     *
     *  @param[in] annotation The dummy indices we are
     * annotating the underlying tensor with.
     *
     *  @return A labeled read/write tensor.
     */
    labeled_variant_type annotate(const annotation_type& annotation);

    /** @brief Annotates the modes of the wrapped index with
     * the provided labels.
     *
     *  The domain-specific language of tensor operations is
     * written in terms of Einstein notation. This requires us
     * to assign dummy indices to each mode of the tensor. This
     * function pairs @p annotation with the wrapped tensor to
     * provide a labeled tensor. The resulting labeled tensor
     * can be used in tensor expressions.
     *
     *  @param[in] annotation The dummy indices we are
     * annotating the underlying tensor with.
     *
     *  @return A labeled read-only tensor.
     */
    const_labeled_type annotate(const annotation_type& annotation) const;

    /** @brief Returns the shape of the tensor.
     *
     *  The shape of a rank @f$r@f$ tensor, also known as its
     * extents, is a
     *  @f$r@f$ element array such that the @f$i@f$-th element
     * is the length of the @f$i@f-th mode.
     *
     *  @return An array-like object containing the shape of
     * the tensor.
     */
    extents_type extents() const;

    /** @brief Creates an annotation suitable for the wrapped
     * tensor.
     *
     *  For a rank @f$r@f$ tensor this function will create a
     * string containing
     *  @f$r@f$ indices. The @f$i@f$-th 0-based mode's index
     * will be the result of concatentating @p letter with
     * @f$i@f$. For example, using the default value of @p
     * letter this function will generate the annotation
     *  `"i0,i1,i2"` for a rank 3 tensor. If the wrapped tensor
     * is a tensor-of-tensors, this function will insert a
     * semicolon where appropriate.
     *
     *  This function is meant to be used to perform generic
     * operations on the tensors by string replacement. For
     * example:
     *
     *  @code
     *  auto idx0 = t0.make_annotation();
     *  auto idx1 = t1.make_annotation("j");
     *  // To contract mode 1 of t0 with mode 3 of t1 we
     * replace "j3" with "i1" auto new_idx1 =
     * std::regex_replace(idx1, std::regex("j3"), "i1");
     *  // In practice we would work out the resulting
     * annotation too auto mult_op = t0(idx0) * t1(idx1);
     *  @endcode
     *
     *  @param[in] letter The label part of the index. This is
     * what will be concatenated with the mode number to get
     * the final annotation for each mode. Default is `"i"`.
     *
     *  @return A string containing an annotation which is
     * appropriate for the tensor.
     */
    annotation_type make_annotation(const annotation_type& letter) const;

    /** @brief Returns the number of modes in the wrapped
     * tensor.
     *
     *  For a normal, non-hierarchical tensor the rank is
     * simply the number of modes in the tensor. For a
     * hierarchical tensor the rank is still the number of
     * modes in the tensor, but it should be noted that this is
     * the sum of the number of independent and dependent
     * modes.
     *
     *  @return The number of modes in the tensor.
     */
    rank_type rank() const;

    void reallocate(allocator_pointer p);

    /** @brief Used to view the tensor as if it has a different
     * shape.
     *
     *  This function is mainly used to flatten and un-flatten
     * a tensor. Under the hood this function remaps indices
     * by:
     *  - mapping the original coordinate index to its ordinal
     * index
     *  - mapping the ordinal index to the new coordinate index
     *  The data is then redistributed, if needed.
     *
     *  @param[in] shape The shape of the resulting tensor. The
     * volume of
     *                   @p shape must match the result of
     * `size()`.
     *
     *  @return A copy of the current tensor with the new
     * shape.
     *
     *  @throw std::runtime_error if @p shape does not have the
     * same volume as the wrapped tensor. Strong throw
     * guarantee.
     */
    void reshape(const il_type& shape);

    /** @brief Returns the number of elements in this tensor.
     *
     *  This function returns the total number of elements in
     * the tensor, which is sometimes also known as the volume.
     * The volume is computed as the product of the extents. An
     * empty tensor has a volume of zero.
     *
     *  @return The number of elements in the wrapped tensor.
     */
    size_type size() const;

    /** @brief Returns a slice of the wrapped tensor.
     *
     *  This function can be used to extract a slice of the
     * underlying tensor. The slice is assumed to contiguous
     * along each mode, and have the same rank as the
     * underlying tensor. For mode `i` the slice will contain
     * the elements in the range [`lo[i]`, `hi[i]`).
     *
     *  @param[in] lo The index of the first element to include in the slice.
     *  @param[in] hi The index of the first element, which is just outside the
     *                slice.
     * @param[in] p The allocator to use for the resulting slice. Default value
     *              is the allocator returned by default_allocator().
     *
     *  @return The requested slice.
     */
    pimpl_pointer slice(const il_type& lo, const il_type& hi,
                        allocator_pointer p) const;

    /** @brief Adds a string representation of the wrapped
     * tensor to the provided stream.
     *
     *  @param[in,out] os The stream we adding the string
     * representation of the tensor to. After the call @p os
     * will contain the string representation of the wrapped
     * tensor instance.
     *
     *  @return This function returns @p os to facilitate
     * operator chaining.
     */
    std::ostream& print(std::ostream& os) const;

    /** @brief Adds the hash of the wrapped tensor to the
     * provided Hasher.
     *
     *  @param[in] h The hasher we are adding the wrapped
     * tensor to.
     */
    void hash(pluginplay::Hasher& h) const;

    /** @brief Determines if two TensorWrapperPIMPLs wrap identical
     *         tensors.
     *
     *  This comparison determines if the two wrapped tensors are identical
     *  elementwise.
     *
     *  @tparam RHSType the type of the variant used by @p rhs.
     *
     *  @param[in] rhs The wrapped tensor we are comparing to.
     *
     *  @return True if the wrapped tensor compares equal to @p rhs and false
     *          otherwise.
     */
    bool operator==(const TensorWrapperPIMPL& rhs) const;

    /** @brief Returns the variant in a read/write state.
     *
     *  @return The wrapped variant in a modifiable state.
     */
    auto& variant() { return m_tensor_; }

    /** @brief Returns the variant in a read-only state.
     *
     *
     * @return The wrapped variant in a read-only state.
     */
    const auto& variant() const { return m_tensor_; }

private:
    /** @brief Returns the inner rank of the tensor.
     *
     *  For a normal non-hierarchical tensor there are zero
     * inner modes. For a ToT the modes to the right of the
     * semi-colon are inner modes.
     *
     *  @return The number of inner modes.
     */
    rank_type inner_rank_() const noexcept;

    /** @brief Returns the outer rank of the tensor.
     *
     *  For a normal non-hierarchical tensor all modes are
     * "outer" modes. For a ToT the modes to the left of the
     * semi-colon are outer modes.
     *
     *  @return The number of outer modes.
     *
     *  @throw No throw guarantee.
     */
    rank_type outer_rank_() const noexcept;

    /// The actual tensor
    variant_type m_tensor_;

    /// The allocator for the tensor, stored in a type-erased state
    allocator_pointer m_allocator_;

    // Will be added back in a forthcoming PR
    // shape_ptr m_shape_;
};

extern template class TensorWrapperPIMPL<field::Scalar>;
extern template class TensorWrapperPIMPL<field::Tensor>;

} // namespace libchemist::tensor::detail_
