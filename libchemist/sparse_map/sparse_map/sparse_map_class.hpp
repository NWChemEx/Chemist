#pragma once
#include "libchemist/sparse_map/sparse_map/detail_/sparse_map_base.hpp"
#include <utility>

namespace libchemist::sparse_map {

/** @brief Stores the sparsity relation among indices.
 *
 */
template<typename IndIndex, typename DepIndex>
class SparseMap;

template<>
class SparseMap<ElementIndex, ElementIndex> :
  public detail_::SparseMapBase<SparseMap<ElementIndex, ElementIndex>, ElementIndex, ElementIndex> {
private:
    using my_type = SparseMap<ElementIndex, ElementIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, ElementIndex, ElementIndex>::SparseMapBase;

    explicit SparseMap(const SparseMap<ElementIndex, TileIndex>& other);
    explicit SparseMap(const SparseMap<TileIndex, ElementIndex>& other);
    explicit SparseMap(const SparseMap<TileIndex, TileIndex>& other);

}; // class SparseMap

template<>
class SparseMap<ElementIndex, TileIndex> :
  public detail_::SparseMapBase<SparseMap<ElementIndex, TileIndex>, ElementIndex, TileIndex> {
private:
    using my_type = SparseMap<ElementIndex, TileIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, ElementIndex, TileIndex>::SparseMapBase;

    /** @brief Sets the TiledRange of the Domains
     *
     *  Since each Domain is a subspace of the same space, they all have the
     *  same TiledRange (the TiledRange governs the tiling of that common super
     *  space). This function allows the user to set/change the TiledRange for
     *  the Domains.
     *
     *  @param[in] tr The TiledRange for the Domains.
     */
    void set_domain_trange(const TA::TiledRange& tr);
}; // class SparseMap

template<>
class SparseMap<TileIndex, ElementIndex> :
  public detail_::SparseMapBase<SparseMap<TileIndex, ElementIndex>, TileIndex, ElementIndex> {
private:
    using my_type    = SparseMap<TileIndex, ElementIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, TileIndex, ElementIndex>::SparseMapBase;

    explicit SparseMap(const SparseMap<TileIndex, TileIndex>& other);

    /** @brief Creates a new instance from a SparseMap<ElementIndex, ElementIndex>
     *
     *  This ctor uses the provided TiledRange to map the independent element
     *  indices in the input SparseMap to their respective tile indices. The
     *  current instance is initialized to the SparseMap which maps from tile
     *  indices to @p other's element index domains.
     *
     * @param[in] trange The TiledRange to use to convert the indices in
     *                   @p other.
     * @param[in] other The SparseMap we are converting.
     *
     * @throw std::bad_alloc if there is insufficient memory to create this
     *                       instance. Strong throw guarantee.
     * @throw std::runtime_error if the rank of @p trange does not match the
     *                           rank of the independent indices in @p other.
     *                           Strong throw guarantee.
     */
    SparseMap(const TA::TiledRange& trange,
              const SparseMap<ElementIndex, ElementIndex>& other);

    void set_trange(const TA::TiledRange& trange);

    const TA::TiledRange& trange() const;
}; // class SparseMap

template<>
class SparseMap<TileIndex, TileIndex> :
  public detail_::SparseMapBase<SparseMap<TileIndex, TileIndex>, TileIndex, TileIndex> {
private:
    using my_type    = SparseMap<TileIndex, TileIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, TileIndex, TileIndex>::SparseMapBase;

    explicit SparseMap(const TA::TiledRange& trange) { set_trange(trange); }

    void set_trange(const TA::TiledRange& trange);

    const TA::TiledRange& trange() const;

    /** @brief Sets the TiledRange of the Domains
     *
     *  Since each Domain is a subspace of the same space, they all have the
     *  same TiledRange (the TiledRange governs the tiling of that common super
     *  space). This function allows the user to set/change the TiledRange for
     *  the Domains.
     *
     *  @param[in] tr The TiledRange for the Domains.
     */
    void set_domain_trange(const TA::TiledRange& tr);
}; // class SparseMap

extern template class SparseMap<ElementIndex, ElementIndex>;
extern template class SparseMap<ElementIndex, TileIndex>;
extern template class SparseMap<TileIndex, ElementIndex>;
extern template class SparseMap<TileIndex, TileIndex>;

} // end namespace
