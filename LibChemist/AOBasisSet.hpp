#pragma once
#include "LibChemist/AOShell.hpp"

namespace LibChemist {
namespace detail_ {
class AOBasisSetPIMPL;
}

class AOBasisSet {
public:
    using size_type = std::size_t;

    AOBasisSet();
    AOBasisSet(const AOBasisSet& rhs);
    AOBasisSet(AOBasisSet&& rhs) noexcept;
    AOBasisSet& operator=(const AOBasisSet& rhs);
    AOBasisSet& operator=(AOBasisSet&& rhs) noexcept;

    ~AOBasisSet();

    template<typename...Args>
    AOBasisSet(const AOShell& shell, Args&&...args) :
        AOBasisSet(std::forward<Args>(args)...) {
        add_shell_(shell);
    }

    AOShell& shell(size_type i);
    AOShell& operator[](size_type i) { return shell(i); }

private:
    void add_shell_(const AOShell& shell);
    std::unique_ptr<detail_::AOBasisSetPIMPL> pimpl_;
};

}
