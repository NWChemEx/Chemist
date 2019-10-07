#pragma once
#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"

namespace libchemist::detail_ {

template<typename T>
class ShellPIMPL {
public:
    std::unique_ptr<AliasingCGTOPIMPL<T>> cgto(size_type i) {
        auto& old_cs      = coefs(i);
        auto& old_as      = exps(i);
        const auto nprims = old_cs.size();
        std::vector<T*> cs(nprims);
        std::vector<T*> as(nprims);
        for(decltype(nprims) i = 0; i < nprims; ++i) {
            cs[i] = &old_cs[i];
            as[i] = &old_as[i];
        }
        return std::make_unique<AliasingCGTOPIMPL<T>>(std::move(cs),
                                                      std::move(as));
    }

private:
    utilities::OwnOrBorrow<bool> m_pure_;
    utilities::OwnOrBorrow<int> m_l_;
};

template<typename T, template<typename> Container>
class ShellPIMPL_ {
private:
    Container<MathSet<T>> m_coefs_;
    Container<MathSet<T>> m_exps_;
};

template<

} // namespace libchemist::detail_
