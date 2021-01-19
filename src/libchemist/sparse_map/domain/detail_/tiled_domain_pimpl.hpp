#pragma once
#include "domain_pimpl.hpp"
#include <tiledarray.h>

namespace libchemist::sparse_map::detail_ {

/** @brief A mix-in that adds a TiledRange to a DomainPIMPL
 *
 *  For the most part Domain<TileIndex> is the same as Domain<ElementIndex>
 *  except that the former also contains a TiledRange instance. This mix-in
 *  makes sure that the PIMPL does the right thing with the TiledRange instance.
 *  All index inspections/manipulations are punted to the base class.
 *
 *  @tparam BaseType Assumed to be DomainPIMPL or a class derived from
 *                   DomainPIMPL.
 */
template<typename BaseType>
class TiledDomainPIMPL : public BaseType {
public:
    /// Type of this class
    using my_type = TiledDomainPIMPL<BaseType>;

public:
    /// Type of the elements in this class
    using value_type = typename BaseType::value_type;

    /** @brief Creates an empty domain.
     *
     *  The Domain created from this ctor will be empty (it will contain no
     *  indices) and it will have an empty TiledRange. Note by convention empty
     *  Domains have a rank of 0 even though they do not contain any rank 0
     *  elements.
     */
    TiledDomainPIMPL() = default;

    /** @brief Creates an empty Domain with the specified TiledRange
     *
     *  @param[in] trange The TiledRange indices in this Domain are taken from.
     */
    explicit TiledDomainPIMPL(const TA::TiledRange& trange);

    /** @brief Allows the user to set the TiledRange associated with this
     *         Domain.
     *
     *  This function can be used to set the TiledRange of a default constructed
     *  instance or to change the TiledRange associated with the present
     *  instance. Note that if the instance already contains indices, those
     *  indices must be included in the provided TiledRange.
     *
     * @param[in] tr The TiledRange to associate with this Domain.
     *
     * @throw std::out_of_range if this Domain contains indices and those
     *                          indices are not in @p tr. Strong throw
     *                          guarantee.
     */
    void set_trange(const TA::TiledRange& tr);

    /** @brief Returns the TiledRange associated with this instance.
     *
     *  This function can be used to retrieve the TiledRange associated with
     *  the present instance.
     *
     *  @return The TiledRange associated with this instance.
     *
     *  @throw None No throw guarantee.
     */
    const auto& trange() const { return m_trange_; }

private:
    /// Type of the most basic base class
    using base_pimpl = DomainPIMPL<value_type>;

    /** @brief Thin wrapper around base's clone_, which also copies the
     *         TiledRange.
     *
     *  @return A deep copy of the current instance via a pointer to the
     *          DomainPIMPL base class.
     */
    std::unique_ptr<base_pimpl> clone_() const override;

    /** @brief Thin wrapper around base's insert_, which ensures index is in
     *         the TiledRange.
     *
     *  @param[in] idx The index we are inserting into the Domain.
     *
     *  @throw std::out_of_range if this instance has a TiledRange and @p idx is
     *                           not in it.
     */
    void insert_(value_type idx) override;

    /** @brief Wraps the base class's prod_assign_ additionally accounting for
     *         the TiledRange instance.
     *
     *  When taking the Cartesian product of two Domains, the resulting Domain's
     *  TiledRange should be the Cartesian product of the TiledRange instances
     *  of each Domain. When both this instance and @p other have TiledRange
     *  instances (notably this means @p other is a TiledDomainPIMPL instance)
     *  this function will set this instance's TiledRange to the Cartesian
     *  Product of its initial TiledRange and that of @p other. If both this
     *  instance and @p other don't have a TiledRange the result will also not
     *  have a TiledRange. Any other scenario will raise an exception.
     *
     *  @param[in] other The Domain we are taking the Cartesian product with.
     *
     *  @return The current instance set to the Cartesian product of its prior
     *          state with @p other.
     *
     *  @throw std::runtime_error if @p other can not be downcast to a
     *                            TiledRangePIMPL. Strong throw guarantee.
     */
    virtual base_pimpl& prod_assign_(const base_pimpl& other) override;

    /** @brief Wraps the base class's union_assign_ ensuring that TiledRanges
     *         of the two instances are compatible.
     *
     *  For union, Domains containing tile indices must have equal TiledRange
     *  instances. This wrapper makes sure that this is the case (or that both
     *  instances do not have TiledRange instances).
     *
     * @param[in] other The Domain we are taking the union with.
     *
     * @return The current instance set to the union of this instance's prior
     *         state with that of @p other.
     *
     * @throw std::runtime_error if @p other can not be downcast to
     *                           TiledDomainPIMPL. Strong throw guarantee.
     * @throw std::runtime_error if @p other has a TiledRange and this instance
     *                           does not (or vice versa). Strong throw
     *                           guarantee.
     * @throw std::out_of_range if both this instance and @p other have indices
     *                          and they are of different ranks. Strong throw
     *                          guarantee.
     * @throw std::bad_alloc if there is insufficient memory to store the new
     *                       state. Weak throw guarantee.
     */
    virtual base_pimpl& union_assign_(const base_pimpl& other) override;

    /** @brief Wraps the base class's int_assign_ ensuring that TiledRanges
     *         of the two instances are compatible.
     *
     *  For intersection, domains containing tile indices must have equal
     *  TiledRange instances. This wrapper makes sure that this is the case (or
     *  that both instances do not have TiledRange instances).
     *
     * @param[in] other The Domain we are taking the intersection with.
     *
     * @return The current instance set to the intersection of this instance's
     *         prior state with that of @p other.
     *
     * @throw std::runtime_error if @p other can not be downcast to
     *                           TiledDomainPIMPL. Strong throw guarantee.
     * @throw std::runtime_error if @p other has a TiledRange and this instance
     *                           does not (or vice versa). Strong throw
     *                           guarantee.
     * @throw std::out_of_range if both this instance and @p other have indices
     *                          and they are of different ranks. Strong throw
     *                          guarantee.
     * @throw std::bad_alloc if there is insufficient memory to store the new
     *                       state. Weak throw guarantee.
     */
    virtual base_pimpl& int_assign_(const base_pimpl& other) override;

    /** @brief Thin wrapper around the base class's equal_, which additionally
     *         ensures that the TiledRange instances are equal.
     *
     *  @param[in] other The Domain we are comparing to.
     *
     *  @return True if the two instances are equal and false otherwise.
     */
    bool equal_(const base_pimpl& other) const noexcept override;

    /// Attempts to downcast @p to my_type. Returns nullptr if fails.
    const my_type* downcast_(const base_pimpl& p) const;

    /// Code factorization for ensuring the instance has a TiledRange
    bool has_trange_() const { return m_trange_ != TA::TiledRange{}; }

    /// Code factorization for ensuring @p idx is in @p tr.
    void in_trange_(const TA::TiledRange& tr, const value_type& idx) const;

    /// The TiledRange associated with this Domain
    TA::TiledRange m_trange_;
}; // class TiledDomainPIMPL

//------------------------------------------------------------------------------
// Inline implementations
//------------------------------------------------------------------------------

#define TILEDDOMAINPIMPL TiledDomainPIMPL<BaseType>

template<typename BaseType>
TILEDDOMAINPIMPL::TiledDomainPIMPL(const TA::TiledRange& trange) :
  m_trange_(trange) {}

template<typename BaseType>
void TILEDDOMAINPIMPL::set_trange(const TA::TiledRange& tr) {
    for(std::size_t i = 0; i < this->size(); ++i) in_trange_(tr, this->at(i));
    m_trange_ = tr;
}

template<typename BaseType>
std::unique_ptr<typename TILEDDOMAINPIMPL::base_pimpl>
TILEDDOMAINPIMPL::clone_() const {
    return std::make_unique<my_type>(*this);
}

template<typename BaseType>
void TILEDDOMAINPIMPL::insert_(value_type idx) {
    if(has_trange_()) in_trange_(m_trange_, idx);
    BaseType::insert_(std::move(idx));
}

template<typename BaseType>
typename TILEDDOMAINPIMPL::base_pimpl& TILEDDOMAINPIMPL::prod_assign_(
  const base_pimpl& other) {
    // Make sure other is TiledDomainPIMPL
    const auto* pother = downcast_(other);
    if(pother == nullptr) {
        throw std::runtime_error("RHS of operator*= is not a TiledDomainPIMPL");
    }
    // If both have TiledRanges take the product
    if(has_trange_() && pother->has_trange_()) {
        std::vector<TA::TiledRange1> tr1s;
        for(std::size_t i = 0; i < this->rank(); ++i)
            tr1s.push_back(m_trange_.dim(i));
        for(std::size_t i = 0; i < other.rank(); ++i)
            tr1s.push_back(pother->m_trange_.dim(i));
        m_trange_ = TA::TiledRange(tr1s.begin(), tr1s.end());
    } else if(other.size() == 0 && other.rank() == 0) { // Other is empty
        m_trange_ = TA::TiledRange{};
    } else if(this->size() == 0 && this->rank() == 0) { // I'm empty
        return *this;
    } else if(has_trange_()) {
        throw std::runtime_error(
          "LHS of operator*= has TiledRange, but RHS doesn't");
    } else if(pother->has_trange_()) {
        throw std::runtime_error(
          "RHS of operator*= has TiledRange, but LHS doesn't");
    }

    BaseType::prod_assign_(other);
    return *this;
}

template<typename BaseType>
typename TILEDDOMAINPIMPL::base_pimpl& TILEDDOMAINPIMPL::union_assign_(
  const base_pimpl& other) {
    // Make sure other is TiledDomainPIMPL
    const auto* pother = downcast_(other);
    if(pother == nullptr) {
        throw std::runtime_error("RHS of operator+= is not a TiledDomainPIMPL");
    }
    if(this->size() == 0 && this->rank() == 0) {
        m_trange_ = pother->m_trange_;
    } else if(other.size() == 0 && other.rank() == 0) {
        return *this;
    } else if(trange() != pother->trange()) {
        throw std::runtime_error(
          "LHS and RHS of operator+= do not have the same TiledRange");
    }

    BaseType::union_assign_(other);
    return *this;
}

template<typename BaseType>
typename TILEDDOMAINPIMPL::base_pimpl& TILEDDOMAINPIMPL::int_assign_(
  const base_pimpl& other) {
    // Make sure other is TiledDomainPIMPL
    const auto* pother = downcast_(other);
    if(pother == nullptr) {
        throw std::runtime_error("RHS of operator+= is not a TiledDomainPIMPL");
    }
    if(this->size() == 0 && this->rank() == 0) {
        m_trange_ = TA::TiledRange{};
    } else if(other.size() == 0 && other.rank() == 0) {
        m_trange_ = TA::TiledRange{};
    } else if(trange() != pother->trange()) {
        throw std::runtime_error(
          "LHS and RHS of operator+= do not have the same TiledRange");
    }

    BaseType::int_assign_(other);
    return *this;
}

template<typename BaseType>
bool TILEDDOMAINPIMPL::equal_(const base_pimpl& other) const noexcept {
    const auto* pother = downcast_(other);
    if(pother == nullptr) return false;
    return (m_trange_ == pother->m_trange_) && BaseType::equal_(other);
}

template<typename BaseType>
const TILEDDOMAINPIMPL* TILEDDOMAINPIMPL::downcast_(const base_pimpl& p) const {
    return dynamic_cast<const my_type*>(&p);
}
template<typename BaseType>
void TILEDDOMAINPIMPL::in_trange_(const TA::TiledRange& tr,
                                  const value_type& idx) const {
    if(!tr.tiles_range().includes(idx)) {
        std::stringstream ss;
        ss << "Index: " << idx << " is not in range: " << tr;
        throw std::out_of_range(ss.str());
    }
}

#undef TILEDDOMAINPIMPL

} // namespace libchemist::sparse_map::detail_
