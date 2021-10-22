#pragma once
#include <memory>
#include <tiledarray.h>
#include <vector>

namespace libchemist::tensor {

class Allocator {
public:
    using size_type = std::size_t;

    using extents_type = std::vector<std::size_t>;

    using tiled_range_type = TA::TiledRange;

    using allocator_ptr = std::unique_ptr<Allocator>;

    explicit Allocator(TA::World& world = TA::get_default_world());

    allocator_ptr clone() const { return clone_(); }

    virtual ~Allocator() noexcept = default;

    tiled_range_type make_tiled_range(const extents_type& shape) const;

protected:
    /// To help derived classes implement clone_
    Allocator(const Allocator&) = default;

private:
    Allocator(Allocator&&) = delete;
    Allocator& operator=(const Allocator&) = delete;
    Allocator& operator=(Allocator&&) = delete;

    virtual allocator_ptr clone_() const = 0;

    virtual tiled_range_type make_tr_(const extents_type& shape) const = 0;

    TA::World& m_world_;
};

inline Allocator::Allocator(TA::World& world) : m_world_(world) {}

inline typename Allocator::tiled_range_type Allocator::make_tiled_range(
  const extents_type& shape) const {
    return make_tr_(shape);
}

} // namespace libchemist::tensor
