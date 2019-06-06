#pragma once

namespace libchemist::basis_set {

template<typename BasisSet>
class GaussianView {
public:
    GaussianView(BasisSet bs) : m_bs_(bs), m_size_(bs.nprims());
};

} // namespace libchemist::basis_set
