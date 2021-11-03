#pragma once
#include "libchemist/tensor/allocators/allocator.hpp"

namespace libchemist::tensor {

/** @brief An allocator where every tile is a single element.
 *
 *  @warning This allocator should not be used in production as it will be very
 *           very slow. It exists primarily for unit-testing purposes and even
 *           then should only be used sparingly.
 *
 *  This allocator with tile the resulting tensor such that every element is in
 *  its own tile. While this allows element-wise access, it also ruins the
 *  performance.
 *
 *  @tparam VariantType A variant containing all of the possible tensor types
 *                      which may be wrapped by the TensorWrapper class.
 */
template<typename VariantType>
class SingleElementTiles : public Allocator<VariantType> {
private:
    /// Type this class derives from
    using base_type = Allocator<VariantType>;

    /// Type of this class
    using my_type = SingleElementTiles<VariantType>;

public:
    /// Pulls in the base class's typedefs
    ///@{
    using typename base_type::runtime_reference;

    using typename base_type::allocator_ptr;

    using typename base_type::extents_type;

    using typename base_type::tiled_range_type;
    ///@}

    SingleElementTiles(runtime_reference world = TA::get_default_world());

protected:
    /// Copy ctor is used to implement clone, but shouldn't be called otherwise
    SingleElementTiles(const SingleElementTiles&) = default;

private:
    /// Implement the Allocator interfrace
    ///@{
    virtual allocator_ptr clone_() const override;
    virtual tiled_range_type make_tr_(const extents_type& shape) const override;
    virtual bool is_equal_(const base_type& other) const noexcept override;
    ///@}
};

//------------------------------------------------------------------------------
//                         Inline Implementations
//------------------------------------------------------------------------------

template<typename VariantType>
SingleElementTiles<VariantType>::SingleElementTiles(runtime_reference world) :
  Allocator<VariantType>(world) {}

template<typename VariantType>
typename SingleElementTiles<VariantType>::allocator_ptr
SingleElementTiles<VariantType>::clone_() const {
    return allocator_ptr(new SingleElementTiles<VariantType>(*this));
}

template<typename VariantType>
typename SingleElementTiles<VariantType>::tiled_range_type
SingleElementTiles<VariantType>::make_tr_(const extents_type& shape) const {
    using tr1_type    = TA::TiledRange1;
    using size_type   = typename extents_type::size_type;
    using offset_type = typename tr1_type::index1_type;

    const auto n_modes = shape.size();
    std::vector<tr1_type> tr1s(n_modes);
    for(size_type mode_i = 0; mode_i < n_modes; ++mode_i) {
        std::vector<offset_type> edges(shape[mode_i] + 1);
        std::iota(edges.begin(), edges.end(), 0);
        tr1s[mode_i] = tr1_type(edges.begin(), edges.end());
    }

    return TA::TiledRange(tr1s.begin(), tr1s.end());
}

template<typename VariantType>
bool SingleElementTiles<VariantType>::is_equal_(
  const base_type& rhs) const noexcept {
    const auto* prhs = dynamic_cast<const my_type*>(&rhs);
    if(!prhs) return false;
    return base_type::operator==(rhs);
}

} // namespace libchemist::tensor
