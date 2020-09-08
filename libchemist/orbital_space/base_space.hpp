#pragma once
#include <sde/detail_/memoization.hpp>

namespace libchemist::orbital_space::detail_ {

template<typename Traits>
class BaseSpace_ {
public:
    using overlap_type = typename Traits::overlap_type;

    BaseSpace_()

    auto& S() { return m_S_; }
    const auto& S() const { return m_S_; }

    void hash(sde::Hasher& h) const;
private:
    overlap_type m_S_;
};

}
