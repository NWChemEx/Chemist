#pragma once
#include "libchemist/types.hpp"

namespace libchemist {

template<typename ParentType>
class AOCenter {
public:
    type::size nshells() const noexcept { return m_nshells_; }
    type::coord& private : const AOBasisSet& m_parent_;
    type::size m_shell_start_;
    type::size m_nshells_;
};

template<>
  class AOCenter < ParentType&

  class CenterView {
public:
    CenterView(AOBasisSet& bs) {}
    type::size nshells() const noexcept { return m_shells_.size(); }

private:
    std::vector<type::size> m_shells_;
};

class BasisFunctionView {
public:
    BasisFunctionView(AOBasisSet& bs);
};

} // namespace libchemist
