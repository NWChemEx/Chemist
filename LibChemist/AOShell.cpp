#include "LibChemist/AOShell.hpp"
#include "LibChemist/Implementations/AOShellPIMPL.hpp"

namespace LibChemist {

using size_type = typename AOShell::size_type;
using coord_type = typename AOShell::coord_type;

AOShell::AOShell() : pimpl_(std::make_unique<detail_::StandAloneAOShell>()) {}

AOShell::AOShell(const AOShell& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}

AOShell::AOShell(AOShell&& rhs) noexcept : pimpl_(std::move(rhs.pimpl_)){}

AOShell& AOShell::operator=(const AOShell& rhs) {
    rhs.pimpl_->clone().swap(pimpl_);
    return *this;
}

AOShell& AOShell::operator=(AOShell&& rhs) noexcept {
    pimpl_ = std::move(rhs.pimpl_);
    return *this;
}

AOShell::~AOShell() = default;

AOShell::AOShell(std::unique_ptr<detail_::AOShellPIMPL>&& pimpl) noexcept :
pimpl_(std::move(pimpl)){}

size_type AOShell::size() const { return pimpl_->size(); }
size_type AOShell::nprims() const noexcept { return pimpl_->nprims(); }
bool& AOShell::pure() noexcept { return pimpl_->pure(); }
size_type& AOShell::l() noexcept { return pimpl_->l(); }
coord_type& AOShell::center() noexcept { return pimpl_->center(); }
double& AOShell::coef(size_type i) noexcept { return pimpl_->coef(i); }

double& AOShell::alpha(size_type i) noexcept { return pimpl_->alpha(i); }

void AOShell::add_prim_(double alpha, double c) { pimpl_->add_prim(alpha, c); }

bool AOShell::operator==(const AOShell& rhs) const noexcept {
    if(nprims() != rhs.nprims() || pure() != rhs.pure()) return false;
    for(size_type i = 0; i < nprims(); ++i)
        if(coef(i) != rhs.coef(i) || alpha(i) != rhs.alpha(i)) return false;
    return true;
}

} // End namespace LibChemist
