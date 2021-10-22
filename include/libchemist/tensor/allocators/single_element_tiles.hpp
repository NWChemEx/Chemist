#pragma once
#include "libchemist/tensor/allocators/allocator.hpp"

namespace libchemist::tensor {

class SingleElementTiles : public Allocator {
public:
    SingleElementTiles(TA::World& world = TA::get_default_world());

protected:
    SingleElementTiles(const SingleElementTiles&) = default;

private:
    virtual allocator_ptr clone_() const override;
    virtual tiled_range_type make_tr_(const extents_type& shape) const override;
};

inline SingleElementTiles::SingleElementTiles(TA::World& world) :
  Allocator(world) {}

inline typename Allocator::allocator_ptr SingleElementTiles::clone_() const {
    SingleElementTiles rv(*this);
    return allocator_ptr(new SingleElementTiles(*this));
}

inline typename Allocator::tiled_range_type SingleElementTiles::make_tr_(
  const extents_type& shape) const {
    // TODO: Make me work
    return TA::TiledRange{};
}

} // namespace libchemist::tensor
