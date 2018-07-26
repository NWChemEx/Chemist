#include "LibChemist/AOShell.hpp"
#include "LibChemist/Implementations/AOShellPIMPL.hpp"
#include <Utilities/Mathematician/Combinatorics.hpp> //For binomial coefficient
#include <iomanip>                                   //for setprecision
#include <iostream>                                  //For endl

namespace LibChemist {

using size_type  = typename AOShell::size_type;
using coord_type = typename AOShell::coord_type;

AOShell::AOShell() : pimpl_(std::make_unique<detail_::ContiguousAOShell>()) {}

AOShell::AOShell(const AOShell& rhs) : pimpl_(rhs.pimpl_->clone()) {}

AOShell::AOShell(AOShell&& rhs) noexcept : pimpl_(std::move(rhs.pimpl_)) {}

// AOShell& AOShell::operator=(const AOShell& rhs) {
//    //Copy CTor will make new buffers don't want to do that
//    l() = rhs.l();
//    center() = rhs.center();
//
//    //Only support assignment if we have no primitives or the same number as
//    rhs if(!nprims()) {
//        for(size_type i = 0; i < rhs.nprims(); ++i)
//            add_prim_(rhs.alpha(i), rhs.coef(i));
//    }
//    else if(nprims() == rhs.nprims()) {
//        for(size_type i = 0; i < rhs.nprims(); ++i) {
//            alpha(i) = rhs.alpha(i);
//            coef(i) = rhs.coef(i);
//        }
//    }
//    else
//        throw std::logic_error("Incompatible number of primitives for "
//                               "assignment");
//    return *this;
//}

AOShell& AOShell::operator=(AOShell&& rhs) noexcept {
    pimpl_ = std::move(rhs.pimpl_);
    return *this;
}

AOShell::~AOShell() noexcept = default;

AOShell::AOShell(std::unique_ptr<detail_::AOShellPIMPL>&& pimpl) noexcept :
  pimpl_(std::move(pimpl)) {}

size_type AOShell::size() const {
    return pure() ? 2ul * l() + 1ul :
                    Utilities::binomial_coefficient<size_type>(2ul + l(), l());
}
size_type AOShell::nprims() const noexcept { return pimpl_->nprims(); }
bool& AOShell::pure() noexcept { return pimpl_->pure(); }
size_type& AOShell::l() noexcept { return pimpl_->l(); }
coord_type& AOShell::center() noexcept { return pimpl_->center(); }
double& AOShell::coef(size_type i) noexcept { return pimpl_->coef(i); }

double& AOShell::alpha(size_type i) noexcept { return pimpl_->alpha(i); }

void AOShell::add_prim_(double alpha, double c) { pimpl_->add_prim(alpha, c); }

bool operator==(const AOShell& lhs, const AOShell& rhs) noexcept {
    if(lhs.nprims() != rhs.nprims() || lhs.pure() != rhs.pure() ||
       lhs.center() != rhs.center())
        return false;

    for(size_type i = 0; i < lhs.nprims(); ++i)
        if(lhs.coef(i) != rhs.coef(i) || lhs.alpha(i) != rhs.alpha(i))
            return false;
    return true;
}

} // End namespace LibChemist

std::ostream& operator<<(std::ostream& os, const LibChemist::AOShell& shell) {
    os << std::fixed << std::setprecision(15) << "l: " << shell.l()
       << " Rx: " << shell.center()[0] << " Ry: " << shell.center()[1]
       << " Rz: " << shell.center()[2] << std::endl;
    for(auto i = 0; i < shell.nprims(); ++i)
        os << "c: " << shell.coef(i) << " alpha: " << shell.alpha(i)
           << std::endl;
    return os;
}
