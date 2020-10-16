#pragma once
#include "libchemist/sparse_map/detail_/domain_pimpl.hpp"
#include <tiledarray.h>
namespace libchemist::sparse_map::detail_ {

template<typename BaseType>
class TiledDomainPIMPL : public BaseType {
public:
    using my_type = TiledDomainPIMPL<BaseType>;
public:
    using value_type = typename BaseType::value_type;

    TiledDomainPIMPL() = default;

    template<typename...Args>
    explicit TiledDomainPIMPL(const TA::TiledRange& trange, Args&&...args);

    void set_trange(const TA::TiledRange& tr) { m_trange_ = tr; }

    const auto& trange() const {return m_trange_; }
private:
    using base_pimpl = DomainPIMPL<value_type>;

    std::unique_ptr<base_pimpl> clone_() const override;

    TA::TiledRange m_trange_;
};

//------------------------------------------------------------------------------
// Inline implementations
//------------------------------------------------------------------------------

template<typename BaseType>
template<typename...Args>
TiledDomainPIMPL<BaseType>::TiledDomainPIMPL(
  const TA::TiledRange& trange, Args&&...args) :
  BaseType(std::forward<args>(args)...), m_trange_(trange) {}

template<typename BaseType>
std::unique_ptr<typename TiledDomainPIMPL<BaseType>::base_pimpl>
TiledDomainPIMPL<BaseType>::clone_() const {
    return std::make_unique<my_type>(*this);
}

} // namespace libchemist::sparse_map::detail_
