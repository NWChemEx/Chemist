#pragma once
#include "libchemist/tensor/allocators/allocator.hpp"
namespace libchemist::tensor {

/** @brief Allocator which puts all the elements in one big tile.
 *
 *  This allocator is unlikely to be performant except for small dense systems.
 */
class OneBigTile : public Allocator {
public:
    OneBigTile(runtime_reference world = TA::get_default_world());

protected:
    OneBigTile(const OneBigTile&) = default;

private:
    virtual allocator_ptr clone_() const override;
    virtual tiled_range_type make_tr_(const extents_type& shape) const override;
};

//------------------------------------------------------------------------------
//                            Inline Implementations
//------------------------------------------------------------------------------

inline OneBigTile::OneBigTile(runtime_reference world) : Allocator(world) {}

inline typename OneBigTile::allocator_ptr OneBigTile::clone_() const {
    return allocator_ptr(new OneBigTile(*this));
}

inline typename OneBigTile::tiled_range_type OneBigTile::make_tr_(
  const extents_type& shape) const {
    using tr1_type    = TA::TiledRange1;
    using size_type   = extents_type::size_type;
    using offset_type = typename tr1_type::index1_type;

    const auto n_modes = shape.size();
    std::vector<tr1_type> tr1s(n_modes);
    for(size_type mode_i = 0; mode_i < n_modes; ++mode_i) {
        tr1s[mode_i] = tr1_type{0, shape[mode_i]};
    }
    return TA::TiledRange(tr1s.begin(), tr1s.end());
}

} // namespace libchemist::tensor
