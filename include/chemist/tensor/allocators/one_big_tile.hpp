#pragma once
#include "chemist/tensor/allocators/allocator.hpp"
namespace chemist::tensor {

/** @brief Allocator which puts all the elements in one big tile.
 *
 *  This allocator puts all of the elements of a tensor into one big tile. As
 *  such it is unlikely to be performant except for small dense systems. The
 *  main purpose of this allocator is for unit testing. More specifically,
 *  along with single-element tiles, one-big tile is an easy way to guarantee
 *  tile compatability between tensors (as long as the element extents are
 *  also compatible). Unlike the single-element tiles, the resulting tensors
 *  will perform well enought as to not impede
 */
template<typename VariantType>
class OneBigTile : public Allocator<VariantType> {
private:
    /// Type of the base allocator
    using base_type = Allocator<VariantType>;

    /// Type of this class
    using my_type = OneBigTile<VariantType>;

public:
    /// Pull in types from the base class
    ///@{
    using typename base_type::runtime_reference;

    using typename base_type::allocator_ptr;

    using typename base_type::tiled_range_type;

    using typename base_type::extents_type;
    ///@}

    /** @brief Creates a new OneBigTile allocator.
     *
     *  The only state of a OneBigTile indicator is the runtime stored in the
     *  base class. This ctor simply forwards the provided runtime to the base
     *  class.
     *
     *  @param[in] world The runtime the allocator should use for allocating.
     *
     *  @throw None No throw guarantee.
     */
    explicit OneBigTile(runtime_reference world = TA::get_default_world());

protected:
    /// Defaulted copy ctor, used to implement clone_
    OneBigTile(const OneBigTile&) = default;

    virtual void hash_(pluginplay::Hasher& h) const override;

private:
    /// Implement the Allocator interface
    ///@{
    virtual allocator_ptr clone_() const override;
    virtual tiled_range_type make_tr_(const extents_type& shape) const override;
    virtual bool is_equal_(const base_type& rhs) const noexcept override;
    ///@}
};

//------------------------------------------------------------------------------
//                            Inline Implementations
//------------------------------------------------------------------------------

template<typename VariantType>
OneBigTile<VariantType>::OneBigTile(runtime_reference world) :
  Allocator<VariantType>(world) {}

template<typename VariantType>
typename OneBigTile<VariantType>::allocator_ptr
OneBigTile<VariantType>::clone_() const {
    return allocator_ptr(new OneBigTile<VariantType>(*this));
}

template<typename VariantType>
typename OneBigTile<VariantType>::tiled_range_type
OneBigTile<VariantType>::make_tr_(const extents_type& shape) const {
    using tr1_type    = TA::TiledRange1;
    using size_type   = typename extents_type::size_type;
    using offset_type = typename tr1_type::index1_type;

    const auto n_modes = shape.size();
    std::vector<tr1_type> tr1s(n_modes);
    for(size_type mode_i = 0; mode_i < n_modes; ++mode_i) {
        tr1s[mode_i] = tr1_type{0, shape[mode_i]};
    }
    return TA::TiledRange(tr1s.begin(), tr1s.end());
}

template<typename VariantType>
bool OneBigTile<VariantType>::is_equal_(const base_type& rhs) const noexcept {
    const auto* prhs = dynamic_cast<const my_type*>(&rhs);
    if(!prhs) return false;
    return base_type::operator==(rhs);
}

template<typename VariantType>
void OneBigTile<VariantType>::hash_(pluginplay::Hasher& h) const {
    base_type::hash_(h);
}

} // namespace chemist::tensor
