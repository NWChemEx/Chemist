#pragma once
#include <chemist/dsl/dsl_fwd.hpp>

namespace chemist::dsl {

template<typename DerivedType>
class Term {
public:
    template<typename RHSType>
    auto operator+(RHSType&& rhs) {
        auto& lhs      = static_cast<DerivedType&>(*this);
        using no_ref_t = std::remove_reference_t<RHSType>;
        return Add<DerivedType, no_ref_t>(lhs, std::forward<RHSType>(rhs));
    }
};

} // namespace chemist::dsl