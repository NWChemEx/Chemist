#pragma once
#include "sparse_map_pimpl.hpp"

namespace chemist::sparse_map::detail_ {

/** @brief Mix-in for adding a TiledRange to a SparseMapPIMPL.
 *
 *  This class is intended to be used as a mix-in to add a TA::TiledRange
 *  member to a SparseMapPIPL. The TA::TiledRange is associated with the
 *  independent indices (the Domain<TileIndex> class stores its own instance).
 *
 *  @tparam BaseType The class of the SparseMapPIMPL hierarchy that this mix-in
 *                   should inherit from.
 */
template<typename BaseType>
class TiledSparseMapPIMPL : public BaseType {
private:
    /// Type of an instance of this class
    using my_type = TiledSparseMapPIMPL<BaseType>;

public:
    /// Type of the independent indices (should be TileIndex)
    using key_type = typename BaseType::key_type;

    /// Type of the Domains the independent indices map to
    using mapped_type = typename BaseType::mapped_type;

    /// Creates a new empty SparseMap with an empty TiledRange
    TiledSparseMapPIMPL() = default;

    /** @brief Provides read-only access to the TiledRange associated with this
     *         instance.
     *
     *  @return The TiledRange associated with this instance.
     *
     *  @throw None No throw guarantee.
     */
    const auto& trange() const noexcept { return m_trange_; }

    /** @brief Sets the TiledRange associated with the independent indices to
     *         the provided value.
     *
     *  @param[in] trange The TiledRange to associate with the independent
     *                    indices in this SparseMap.
     *
     *  @throw std::out_of_range If the independent indices already stored in
     *                           this map are not in @p trange. Strong throw
     *                           guarantee.
     */
    void set_trange(const TA::TiledRange& trange);

protected:
    /// Type of the dependent indices
    using dep_index = typename mapped_type::value_type;

    /// Type of the most basic base class in the hierarchy
    using base_pimpl = SparseMapPIMPL<key_type, dep_index>;

    /// Wraps BaseType::add_to_domain_ and ensures @p ind is in TiledRange
    virtual void add_to_domain_(const key_type& ind,
                                const dep_index& dep) override;

    /** @brief Wraps the BaseType::dp_assign_ to account for TiledRange
     *
     *  This function wraps BaseType::dp_assign_ ensuring that:
     *
     *  - both instances have empty TiledRange instances, or
     *  - both instance have non-empty TiledRange instances
     *
     *  Any other permutation results in an error.
     *
     *  @param[in] rhs The instance we are taking the direct product with.
     *
     *  @return The current instance set to the direct product of its previous
     *          state with @p rhs.
     *
     *  @throw std::runtime_error if only one of this instance or @p rhs has a
     *                            TiledRange. Strong throw guarantee.
     *  @throw std::runtime_error if @p rhs is not a TiledSparseMapPIMPL. Strong
     *                            throw guarantee.
     */
    virtual base_pimpl& dp_assign_(const base_pimpl& rhs) override;

    /** @brief Wraps the BaseType::prod_assign_ to account for TiledRange
     *
     *  This function wraps BaseType::prod_assign_ method ensuring that:
     *
     *  - both instances have the same non-empty TiledRange, or
     *  - both instances have no TiledRange instances, or
     *  - one or both instances are empty
     *
     *  Any other permutation results in an error.
     *
     *  @param[in] rhs The instance we are taking the product with.
     *
     *  @return The current instance set to the direct product of its previous
     *          state with @p rhs.
     *
     *  @throw std::runtime_error if only one of this instance or @p rhs has a
     *                            TiledRange. Strong throw guarantee.
     *  @throw std::runtime_error if @p rhs is not a TiledSparseMapPIMPL. Strong
     *                            throw guarantee.
     */
    virtual base_pimpl& prod_assign_(const base_pimpl& rhs) override;

    /** @brief Wraps the BaseType::union_assign_ to account for TiledRange
     *
     *  This function wraps BaseType::union_assign_ method ensuring that:
     *
     *  - both instances have empty TiledRange instances, or
     *  - both instances have the same non-empty TiledRange, or
     *  - one or both instances are empty
     *
     *  Any other permutation results in an error.
     *
     *  @param[in] rhs The instance we are taking the union with.
     *
     *  @return The current instance set to the union of its previous state with
     *          @p rhs.
     *
     *  @throw std::runtime_error if only one of this instance or @p rhs has a
     *                            TiledRange. Strong throw guarantee.
     *  @throw std::runtime_error if @p rhs is not a TiledSparseMapPIMPL. Strong
     *                            throw guarantee.
     */
    virtual base_pimpl& union_assign_(const base_pimpl& rhs) override;

    /** @brief Wraps the BaseType::int_assign_ to account for TiledRange
     *
     *  This function wraps BaseType::union_assign_ method ensuring that:
     *
     *  - both instances have empty TiledRange instances, or
     *  - both instances have the same non-empty TiledRange, or
     *  - one or both instances are empty
     *
     *  Any other permutation results in an error.
     *
     *  @param[in] rhs The instance we are taking the intersection with.
     *
     *  @return The current instance set to the intersection of its previous
     *          state with @p rhs.
     *
     *  @throw std::runtime_error if only one of this instance or @p rhs has a
     *                            TiledRange. Strong throw guarantee.
     *  @throw std::runtime_error if @p rhs is not a TiledSparseMapPIMPL. Strong
     *                            throw guarantee.
     */
    virtual base_pimpl& int_assign_(const base_pimpl& rhs) override;

    /// Wraps BaseType::equal_ so it also compares TiledRange instances
    virtual bool equal_(const base_pimpl& rhs) const noexcept override;

private:
    /// Makes a polymorphic clone of this instance
    std::unique_ptr<base_pimpl> clone_() const override;

    /// Returns true if the instance has a non-empty TiledRange
    bool has_trange_() const noexcept { return m_trange_ != TA::TiledRange{}; }

    /// Code factorization for ensuring @p idx is in @p tr
    void in_trange_(const TA::TiledRange& tr, const TileIndex& idx) const;

    /// The TiledRange instance associated with this instance
    TA::TiledRange m_trange_;
}; // class TiledSparseMapPIMPL

//------------------------------------------------------------------------------
//                            Inline Implementations
//------------------------------------------------------------------------------

#define TSMPIMPL TiledSparseMapPIMPL<BaseType>

template<typename BaseType>
void TSMPIMPL::set_trange(const TA::TiledRange& trange) {
    bool nonempty = this->size() != 0;
    if(nonempty && (trange.rank() != this->ind_rank()))
        throw std::out_of_range("Rank of input TiledRange is not compatible");
    for(std::size_t i = 0; i < this->size(); ++i)
        in_trange_(trange, this->at(i).first);
    m_trange_ = trange;
}

template<typename BaseType>
void TSMPIMPL::add_to_domain_(const key_type& ind, const dep_index& dep) {
    if(has_trange_()) in_trange_(m_trange_, ind);
    BaseType::add_to_domain_(ind, dep);
}

template<typename BaseType>
typename TSMPIMPL::base_pimpl& TSMPIMPL::dp_assign_(const base_pimpl& rhs) {
    const auto* pother = dynamic_cast<const my_type*>(&rhs);

    if(pother == nullptr) {
        throw std::runtime_error(
          "RHS of direct_product is not a TiledSparseMapPIMPL");
    }
    // If both have TiledRanges take the product
    if(has_trange_() && pother->has_trange_()) {
        std::vector<TA::TiledRange1> tr1s;
        for(std::size_t i = 0; i < this->ind_rank(); ++i)
            tr1s.push_back(m_trange_.dim(i));
        for(std::size_t i = 0; i < rhs.ind_rank(); ++i)
            tr1s.push_back(pother->m_trange_.dim(i));
        m_trange_ = TA::TiledRange(tr1s.begin(), tr1s.end());
    } else if(rhs.size() == 0 && rhs.ind_rank() == 0) { // Other is empty
        m_trange_ = TA::TiledRange{};
    } else if(this->size() == 0 && this->ind_rank() == 0) { // I'm empty
        return *this;
    } else if(has_trange_()) {
        throw std::runtime_error(
          "LHS of direct_product has TiledRange, but RHS doesn't");
    } else if(pother->has_trange_()) {
        throw std::runtime_error(
          "RHS of direct_product has TiledRange, but LHS doesn't");
    }

    return BaseType::dp_assign_(rhs);
}

template<typename BaseType>
typename TSMPIMPL::base_pimpl& TSMPIMPL::prod_assign_(const base_pimpl& rhs) {
    // Make sure other is TiledDomainPIMPL
    const auto* pother = dynamic_cast<const my_type*>(&rhs);
    if(pother == nullptr) {
        throw std::runtime_error("RHS of operator*= is not a TiledDomainPIMPL");
    }

    if(this->size() == 0 || rhs.size() == 0)
        m_trange_ = TA::TiledRange{};
    else if(trange() != pother->trange()) {
        throw std::runtime_error(
          "LHS and RHS of operator*= do not have the same TiledRange");
    }

    BaseType::prod_assign_(rhs);
    return *this;
}

template<typename BaseType>
typename TSMPIMPL::base_pimpl& TSMPIMPL::union_assign_(const base_pimpl& rhs) {
    // Make sure other is TiledDomainPIMPL
    const auto* pother = dynamic_cast<const my_type*>(&rhs);
    if(pother == nullptr) {
        throw std::runtime_error("RHS of operator+= is not a TiledDomainPIMPL");
    }

    if(this->size() == 0)
        m_trange_ = pother->m_trange_;
    else if(rhs.size() == 0)
        return *this;
    else if(trange() != pother->trange()) {
        throw std::runtime_error(
          "LHS and RHS of operator+= do not have the same TiledRange");
    }

    BaseType::union_assign_(rhs);
    return *this;
}

template<typename BaseType>
typename TSMPIMPL::base_pimpl& TSMPIMPL::int_assign_(const base_pimpl& rhs) {
    // Make sure other is TiledDomainPIMPL
    const auto* pother = dynamic_cast<const my_type*>(&rhs);
    if(pother == nullptr) {
        throw std::runtime_error("RHS of operator^= is not a TiledDomainPIMPL");
    }

    if(this->size() == 0 || rhs.size() == 0)
        m_trange_ = TA::TiledRange{};
    else if(trange() != pother->trange()) {
        throw std::runtime_error(
          "LHS and RHS of operator^= do not have the same TiledRange");
    }

    BaseType::int_assign_(rhs);
    return *this;
}

template<typename BaseType>
bool TSMPIMPL::equal_(const base_pimpl& rhs) const noexcept {
    auto prhs = dynamic_cast<const my_type*>(&rhs);

    if(prhs == nullptr)
        return false;
    else if(m_trange_ != prhs->m_trange_)
        return false;

    return BaseType::equal_(rhs);
}

template<typename BaseType>
std::unique_ptr<typename TSMPIMPL::base_pimpl> TSMPIMPL::clone_() const {
    auto temp = new my_type(*this);
    return std::unique_ptr<base_pimpl>(temp);
}

template<typename BaseType>
void TSMPIMPL::in_trange_(const TA::TiledRange& tr,
                          const TileIndex& idx) const {
    if(!tr.tiles_range().includes(idx)) {
        std::stringstream ss;
        ss << "Index: " << idx << " is not in range: " << tr;
        throw std::out_of_range(ss.str());
    }
}

#undef TSMPIMPL

} // namespace chemist::sparse_map::detail_
