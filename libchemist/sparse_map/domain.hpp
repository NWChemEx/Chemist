//#pragma once
//#include "libchemist/sparse_map/index.hpp"
//#include "libchemist/sparse_map/detail_/domain_base.hpp"
//
//#include <tiledarray.h>
//
//
//namespace libchemist::sparse_map {
//
///** @brief A set of element coordinate indices.
// *
// *  Practically speaking a domain collects the dependent element indices
// *  associated with a particular independent element index. The SparseMap class
// *  manages the mapping of independent element indices to Domain instances.
// *
// *  @tparam IndexType The type of indices in the Domain. Either ElementIndex or
// *                    TileIndex.
// */
//template<typename IndexType> class Domain;
//
///** @brief Domain containing ElementIndex instances.
// *
// *  This specialization of Domain is for when a Domain is filled with
// *  elemental indices. Relative to DomainBase the only added feature is the
// *  ability to create a Domain of elemental indices from a Domain of tile
// *  indices.
// */
//template<>
//class Domain<ElementIndex> :
//  public detail_::DomainBase<Domain<ElementIndex>, ElementIndex> {
//private:
//    /// Type of this class
//    using my_type     = Domain<ElementIndex>;
//
//    /// Type of the traits associated with this class
//    using traits_type = detail_::DomainTraits<ElementIndex>;
//public:
//    /// The type the indices are stored as
//    using value_type = typename traits_type::value_type;
//    //Pull in base class's ctors
//    using detail_::DomainBase<Domain<ElementIndex>, ElementIndex>::DomainBase;
//
//    /** @brief Converts a Domain of tile indices into a Domain of element
//     *         indices.
//     *
//     *  This ctor loops over tiles in @p other and adds all elements of the tile
//     *  to the current Domain.
//     *
//     *  @param[in] other The domain of tile indices being converted to a domain
//     *                   of element indices.
//     *
//     *  @throw std::bad_alloc if there is insufficient memory to create the new
//     *                        Domain. Strong throw guarantee.
//     */
//    explicit Domain(const Domain<TileIndex>& other);
//}; // class Domain<IndexType>
//
///** @brief A Domain filled with TileIndex instances.
// *
// *  Relative to DomainBase this class has an additional member, the TiledRange.
// *  The additional methods provide convenient means of initializing this extra
// *  state.
// *
// *  @todo insert needs to check that elements are in the TiledRange
// */
//template<>
//class Domain<TileIndex> :
//  public detail_::DomainBase<Domain<TileIndex>, TileIndex> {
//private:
//    /// Type of an instance of this class
//    using my_type     = Domain<TileIndex>;
//
//    /// Type of the base class
//    using base_type   = detail_::DomainBase<Domain<TileIndex>, TileIndex>;
//
//    /// Type of the struct holding the types for this class
//    using traits_type = detail_::DomainTraits<TileIndex>;
//public:
//    /// The type the indices are stored as
//    using value_type = typename traits_type::value_type;
//
//    /// Type of the tiled range stored in this instance
//    using tiled_range_type = TA::TiledRange;
//
//    // Pull in base class's ctors
//    using detail_::DomainBase<Domain<TileIndex>, TileIndex>::DomainBase;
//
//    /** @brief Creates a Domain associated with the specified tiled range.
//     *
//     *  @param[in] trange The tiled range the indices come from.
//     *
//     *  @throw std::bad_alloc if there is insufficient memory to create the new
//     *                        Domain. Strong throw guarantee.
//     */
//    explicit Domain(const tiled_range_type& trange) : m_trange_(trange) {}
//
//    /** @brief Creates a new Domain containing the specified tile indices.
//     *
//     *  This is a thin wrapper around the base class's initializer_list ctor,
//     *  which also allows you to set the TiledRange associated with the Domain.
//     *
//     * @param[in] trange The TiledRange from which the indices are taken.
//     * @param[in] il The initial tile indices in the Domain.
//     *
//     * @throw std::out_of_range if any of the indices in @p il are not valid
//     *                          tile indices for @p trange. Strong throw
//     *                          guarantee.
//     * @throw std::bad_alloc    if there is not sufficient memory to create the
//     *                          new instance. Strong throw guarantee.
//     */
//    Domain(const tiled_range_type& trange, std::initializer_list<TileIndex> il);
//
//    /** @brief Creates a new Domain containing the tile indices of the element
//     *         indices in another Domain.
//     *
//     * @param[in] trange The TiledRange to assign tile indices from.
//     * @param[in] d      The Domain whose elemental indices are being converted.
//     *
//     * @throw std::out_of_range if any of the indices in @p d are not in
//     *                          @p trange. Strong throw guarantee.
//     * @throw std::bad_alloc if there is insufficient memory to create the
//     *                       Domain. Strong throw guarantee.
//     */
//    Domain(const tiled_range_type& trange, const Domain<ElementIndex>& d);
//
//    /** @brief Allows a user to manually set the TiledRange
//     *
//     *  Particularly when using one of the base class's ctors, it is possible to
//     *  create a new Domain which does not have a TiledRange associated with it.
//     *  This method allows you to manually set the TiledRange.
//     *
//     *  @param[in] trange The TiledRange from which this Domain's indices are
//     *                    taken.
//     *
//     *  @throw std::out_of_range If any of the indices already in this Domain
//     *                           are not included in @p trange. Strong throw
//     *                           guarantee.
//     */
//    void set_trange(const tiled_range_type& trange);
//
//    /** @brief Returns the TiledRange associated with the indices in this
//     *        Domain.
//     *
//     * @return The TiledRange that the indices in this Domain are taken from.
//     *
//     * @throw std::bad_optional_access if the TiledRange has not been set.
//     *                                 Strong throw guarantee.
//     */
//    const tiled_range_type& trange() const { return m_trange_.value(); }
//private:
//    /// The TiledRange the indices are pulled from
//    std::optional<tiled_range_type> m_trange_;
//}; // class Domain<TileIndex>
//
///// Convenience typedef of a Domain filled with ElementIndex instances
//using ElementDomain = Domain<ElementIndex>;
//
///// Convenience typedef of a Domain filled with TileIndex instances
//using TileDomain    = Domain<TileIndex>;
//
////------------------------------------------------------------------------------
////             Forward Declaration of Template Instantiations
////------------------------------------------------------------------------------
//
//extern template class Domain<ElementIndex>;
//extern template class Domain<TileIndex>;
//
//} // namespace libchemist::sparse_map
