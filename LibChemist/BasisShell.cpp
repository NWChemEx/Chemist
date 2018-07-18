#include "LibChemist/BasisShell.hpp"
#include "LibChemist/Implementations/AOShellPIMPL.hpp"

namespace LibChemist {

using size_type = typename AOShell::size_type;
using coord_type = typename AOShell::coord_type;

AOShell::AOShell() : pimpl_(std::make_unique<StandAloneAOShell>()) {}

AOShell::AOShell(const AOShell& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}

AOShell::AOShell(AOShell&& rhs) : pimpl_(std::move(rhs.pimpl_)){}

AOShell& AOShell::operator=(const AOShell& rhs) {
    rhs.pimpl_->clone().swap(pimpl_);
    return *this;
}

AOShell& AOShell::operator=(AOShell&& rhs) {
    pimpl_ = std::move(rhs.pimpl_);
    return *this;
}

AOShell::~AOShell() = default;

size_type AOShell::size() const { return pimpl_->size(); }
size_type AOShell::nprims() const noexcept { return pimpl_->nprims(); }
bool& AOShell::pure() noexcept { return pimpl_->pure(); }
size_type& AOShell::l() noexcept { return pimpl_->l(); }
coord_type& AOShell::center() noexcept { return pimpl_->center(); }
double& AOShell::coef(size_type i) noexcept { return pimpl_->coef(i); }
double& AOShell::alpha(size_type i) noexcept { return pimpl_->alpha(i); }

void add_prim_(double alpha, double c) { pimpl_->add_prim_(alpha, c); }

} // End namespace LibChemist
