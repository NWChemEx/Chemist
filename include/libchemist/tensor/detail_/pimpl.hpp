#pragma once
#include "libchemist/tensor/tensor_wrapper.hpp"

namespace libchemist::tensor::detail_ {

/** @brief Holds the state of a TensorWrapper.
 *
 */
template<typename FieldType>
class TensorWrapperPIMPL {
private:
    using my_type      = TensorWrapperPIMPL<FieldType>;
    using parent_type  = TensorWrapper<FieldType>;
    using field_traits = FieldTraits<FieldType>;

public:
    /// Type used to annotate the tensor (i.e. type of the dummy indices)
    using annotation_type = typename parent_type::annotation_type;

    /// Type of the field the tensor is over
    using field_type = FieldType;

    using variant_type = typename field_traits::variant_type;

    using labeled_variant_type = typename field_traits::labeled_variant_type;

    using const_labeled_type =
      typename field_traits::const_labeled_variant_type;

    using const_allocator_reference =
      typename parent_type::const_allocator_reference;
    using allocator_pointer = typename parent_type::allocator_pointer;
    // using shape_type                = Shape<field_type>;
    // using shape_pointer             = typename shape_type::shape_ptr;

    using extents_type = typename parent_type::extents_type;

    /// Type of a pointer to a PIMPL
    using pimpl_pointer = typename parent_type::pimpl_pointer;

    /// Type used to return ranks
    using rank_type = typename parent_type::rank_type;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    /// Type used for initializer lists of sizes
    using il_type = typename parent_type::il_type;

    TensorWrapperPIMPL(variant_type v, allocator_pointer p);

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

    labeled_variant_type annotate(const annotation_type& annotation);
    const_labeled_type annotate(const annotation_type& annotation) const;

    extents_type extents() const;

    annotation_type make_annotation(const annotation_type& letter) const;

    rank_type rank() const;

    void reallocate(allocator_pointer p);

    void reshape(const il_type& shape);

    size_type size() const;

    pimpl_pointer slice(const il_type& lo, const il_type& hi,
                        allocator_pointer p) const;

    std::ostream& print(std::ostream& os) const;

    void hash(pluginplay::Hasher& h) const;

    bool operator==(const TensorWrapperPIMPL& rhs) const;

    auto& variant() { return m_tensor_; }

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

    variant_type m_tensor_;

    allocator_pointer m_allocator_;

    // shape_ptr m_shape_;
};

extern template class TensorWrapperPIMPL<field::Scalar>;
extern template class TensorWrapperPIMPL<field::Tensor>;

} // namespace libchemist::tensor::detail_
