#pragma once
#include "libchemist/orbital_space/base_space.hpp"

namespace libchemist::orbital_space {

template<typename Traits>
class AOSpace_ : public BaseSpace_<Traits> {
private:
    using base_type = BaseSpace_<Traits>;
public:
    using basis_type   = typename Traits::basis_set_type;
private:
    basis_type m_bs_;
};


}
