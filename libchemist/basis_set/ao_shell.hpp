#include "libchemist/basis_set/detail_/ao_shell_base.hpp"

namespace libchemist::basis_set {

template<typename Traits>
class AOShell : public AOShellBase<AOShell<Traits>> {
public:
    using traits_type   = Traits;
    using function_type = ;
    auto& operator[](type::size) const { return m_fxn_; }
    const auto& operator[](type::size) const { return m_fxn_; }

private:
    function_type m_fxn_;
};

} // namespace libchemist::basis_set
