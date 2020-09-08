#pragma once
#include "libchemist/orbital_space/ao_space.hpp:
#include "libchemist/orbital_space/base_space.hpp"

namespace libchemist::orbital_space {

template<typename Traits, typename BaseType = AOSpace_<Traits>>
class DerivedSpace_ : public BaseSpace_<Traits> {
public:
    using transform_type = typename Traits::transform_type;
private:
    transform_type m_C_;
    std::shared_ptr<BaseType> m_base_;
};

} // namespace libchemist::orbital_space
