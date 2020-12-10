#pragma once
#include "libchemist/sparse_map/sparse_map/detail_/sparse_map_base.hpp"
#include <utility>

namespace libchemist::sparse_map {

/** @brief Stores the sparsity relation among indices.
 *
 *  The primary template for the SparseMap class is not defined, instead we
 *  specialize it to the four possible instances:
 *
 *  - Independent indices are ElementIndex, Dependent indices are ElementIndex,
 *  - Independent indices are ElementIndex, Dependent indices are TileIndex,
 *  - Independent indices are TileIndex, Dependent indices are ElementIndex, and
 *  - Independent indices are TileIndex, Dependent indices are TileIndex
 *
 *  This is because, with the exception of the first instance, the API is
 *  slightly different depending on which permutation of tile indices the class
 *  has.
 */
template<typename IndIndex, typename DepIndex>
class SparseMap;

/** @brief Specializes SparseMap to the case where the independent and dependent
 *         indices are both for elements.
 *
 *  This class is a thin addition to SparseMapBase, which adds some useful
 *  conversions from other SparseMap types.
 */
template<>
class SparseMap<ElementIndex, ElementIndex> :
  public detail_::SparseMapBase<SparseMap<ElementIndex, ElementIndex>, ElementIndex, ElementIndex> {
private:
    /// Type of an instance of this class
    using my_type = SparseMap<ElementIndex, ElementIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, ElementIndex, ElementIndex>::SparseMapBase;

    /** @brief Creates a new instance by mapping the dependent tile indices in
     *         another instance to their respective element indices.
     *
     *  This ctor will create a new element-to-element sparse map by mapping
     *  the tile-index domains in another instance to element-index domains
     *  using the TiledRange associated with the Domains.
     *
     *  @param[in] other The SparseMap we are converting to a element-to-element
     *                   SparseMap.
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        instance. Strong throw guarantee.
     */
    explicit SparseMap(const SparseMap<ElementIndex, TileIndex>& other);

    /** @brief Creates a new instance by mapping the independent tile indices in
     *         another instance to their respective element indices.
     *
     *  This ctor will map the independent tile indices of another SparseMap
     *  instance to there respective element indices. The domain associated
     *  with each tile index in @p other will be associated with each element
     *  index in that tile.
     *
     *  @param[in] other The SparseMap we are mapping from.
     *
     *  @throw std::runtime_error if @p other's TiledRange is not set. Strong
     *                            throw guarantee.
     *  @throw std::bad_alloc if there is not enough memory to allocate the new
     *                        instance. Strong throw guarantee.
     */
    explicit SparseMap(const SparseMap<TileIndex, ElementIndex>& other);

    /** @brief Creates a new SparseMap by converting the independent and
     *         dependent tile indices in another instance.
     *
     *  This ctor creates a new SparseMap from a SparseMap whose independent and
     *  dependent indices are both tile indices. The conversion is done by using
     *  the TiledRange instances associated with the independent indices and the
     *  Domains in @p other.
     *
     *  @param[in] other The SparseMap we are converting.
     *
     *  @throw std::runtime_error if @p other contains independent indices of
     *                            rank 1 or greater, but does not contain a
     *                            TiledRange. Strong throw guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        instance. Strong throw guarantee.
     */
    explicit SparseMap(const SparseMap<TileIndex, TileIndex>& other);
}; // class SparseMap<ElementIndex, ElementIndex>

/** @brief Specialization for a SparseMap from element-to-tile indices.
 *
 *  This specialization of SparseMap has independent element indices, which map
 *  to Domains of tile indices.
 */
template<>
class SparseMap<ElementIndex, TileIndex> :
  public detail_::SparseMapBase<SparseMap<ElementIndex, TileIndex>, ElementIndex, TileIndex> {
private:
    /// Type of an instance of this class
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
     *
     *  @throw std::out_of_range if any of the Domains contain indices not found
     *                           in @p tr. Weak throw guarantee.
     */
    void set_domain_trange(const TA::TiledRange& tr);
}; // class SparseMap<ElementIndex, TileIndex>

/** @brief SparseMap from tile indices to element indices.
 *
 *  This SparseMap specialization implements the class when the independent tile
 *  indices map to domains of element indices.
 *
 */
template<>
class SparseMap<TileIndex, ElementIndex> :
  public detail_::SparseMapBase<SparseMap<TileIndex, ElementIndex>, TileIndex, ElementIndex> {
private:
    /// Type of an instance of this class
    using my_type    = SparseMap<TileIndex, ElementIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, TileIndex, ElementIndex>::SparseMapBase;

    /** @brief Creates a new tile-to-element SparseMap by expanding the
     *         dependent tile indices in a tile-to-tile SparseMap.
     *
     *  This ctor will create a new SparseMap by mapping the dependent tile
     *  index domains in @p other to dependent element index domains using the
     *  TiledRange associated with @p other's domains.
     *
     *  @param[in] other The SparseMap we mapping from.
     *
     *  @throw std::runtime_error if @p other does not have a TiledRange
     *                            associated with its Domains. Strong throw
     *                            guarantee.
     *
     *  @throw std::bad_alloc if there is insufficient memory to make the new
     *                        instance. Strong throw guarantee.
     */
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

    /** @brief Sets the TiledRange associated with the independent indices.
     *
     *  @param[in] trange The TiledRange to associate with the independent
     *                    indices.
     */
    void set_trange(const TA::TiledRange& trange);

    /** @brief Returns the TiledRange associated with the independent indices.
     *
     *  @return The TiledRange associated with this instance.
     */
    const TA::TiledRange& trange() const;
}; // class SparseMap<TileIndex, ElementIndex>

/** @brief Specializes SparseMap to independent tile indices mapping to
 *         domains of dependent tile indices.
 *
 *  This specialization covers the case when both the independent indices and
 *  the dependent indices are for tiles.
 */
template<>
class SparseMap<TileIndex, TileIndex> :
  public detail_::SparseMapBase<SparseMap<TileIndex, TileIndex>, TileIndex, TileIndex> {
private:
    /// Type of an instance of this class
    using my_type    = SparseMap<TileIndex, TileIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, TileIndex, TileIndex>::SparseMapBase;

    /** @brief Creates a new instance from a SparseMap<ElementIndex, TileIndex>
     *
     *  This ctor uses the provided TiledRange to map the independent element
     *  indices in the input SparseMap to their respective tile indices. The
     *  current instance is initialized to the SparseMap which maps from tile
     *  indices to @p other's tile index domains.
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
              const SparseMap<ElementIndex, TileIndex>& other);

    /** @brief Sets the TiledRange associated with the independent indices.
     *
     *  @param[in] trange The TiledRange to associate with the independent
     *                    indices.
     */
    void set_trange(const TA::TiledRange& trange);

    /** @brief Returns the TiledRange associated with the independent indices.
     *
     *  @return The TiledRange associated with the independent indices.
     */
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
}; // class SparseMap<TileIndex, TileIndex>

//------------------------------------------------------------------------------
//                         Explicit Instantiations
//------------------------------------------------------------------------------

extern template class SparseMap<ElementIndex, ElementIndex>;
extern template class SparseMap<ElementIndex, TileIndex>;
extern template class SparseMap<TileIndex, ElementIndex>;
extern template class SparseMap<TileIndex, TileIndex>;

} // namespace libchemist::sparse_map
