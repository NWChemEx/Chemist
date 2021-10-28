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
 */
class SingleElementTiles : public Allocator {
public:
    SingleElementTiles(runtime_reference world = TA::get_default_world());

protected:
    SingleElementTiles(const SingleElementTiles&) = default;

private:
    virtual allocator_ptr clone_() const override;
    virtual tiled_range_type make_tr_(const extents_type& shape) const override;
};

//------------------------------------------------------------------------------
//                         Inline Implementations
//------------------------------------------------------------------------------

inline SingleElementTiles::SingleElementTiles(runtime_reference world) :
  Allocator(world) {}

inline typename Allocator::allocator_ptr SingleElementTiles::clone_() const {
    return allocator_ptr(new SingleElementTiles(*this));
}

inline typename Allocator::tiled_range_type SingleElementTiles::make_tr_(
  const extents_type& shape) const {
    using tr1_type    = TA::TiledRange1;
    using size_type   = extents_type::size_type;
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

} // namespace libchemist::tensor
