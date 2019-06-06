#include "libchemist/ao_basis_set.hpp"
#include "libchemist/detail_/utility.hpp"
#include <algorithm> //For max element
#include <sde/detail_/memoization.hpp>

namespace libchemist {

using value_type      = typename AOBasisSet::value_type;
using reference       = typename AOBasisSet::reference;
using const_reference = typename AOBasisSet::const_reference;
using size_type       = typename AOBasisSet::size_type;
using iterator        = typename AOBasisSet::iterator;
using const_iterator  = typename AOBasisSet::const_iterator;

AOBasisSet::AOBasisSet() :
  pimpl_(std::make_unique<detail_::ContiguousBasisSet>()) {}

AOBasisSet::AOBasisSet(std::initializer_list<value_type> il) : AOBasisSet() {
    for(auto& shelli : il) push_back(shelli);
}

AOBasisSet::AOBasisSet(const AOBasisSet& rhs) : AOBasisSet() {
    for(size_type i = 0; i < rhs.nshells(); ++i) push_back(rhs[i]);
}

AOBasisSet::AOBasisSet(AOBasisSet&& rhs) noexcept :
  pimpl_(std::move(rhs.pimpl_)) {}

AOBasisSet& AOBasisSet::operator=(const AOBasisSet& rhs) {
    return *this = std::move(AOBasisSet(rhs));
}

AOBasisSet& AOBasisSet::operator=(AOBasisSet&& rhs) noexcept {
    pimpl_.swap(rhs.pimpl_);
    return *this;
}

AOBasisSet::~AOBasisSet() noexcept = default;

void AOBasisSet::push_back(value_type da_shell) {
    return pimpl_->push_back(std::move(da_shell));
}

size_type AOBasisSet::nshells() const noexcept { return pimpl_->size(); }
reference AOBasisSet::at(std::size_t i) noexcept { return pimpl_->at(i); }

iterator AOBasisSet::begin() noexcept { return pimpl_->begin(); }
iterator AOBasisSet::end() noexcept { return pimpl_->end(); }
const_iterator AOBasisSet::begin() const noexcept { return pimpl_->begin(); }
const_iterator AOBasisSet::end() const noexcept { return pimpl_->end(); }

void AOBasisSet::hash(bphash::Hasher& h) const {
    for(const auto& shelli : *this) h(shelli);
}

std::pair<size_type, iterator> max_l(AOBasisSet& bs) noexcept {
    auto itr = std::max_element(
      bs.begin(), bs.end(), [](const AOShell& shelli, const AOShell& shellj) {
          return shelli.l() < shellj.l();
      });
    return itr != bs.end() ? std::make_pair(itr->l(), itr) :
                             std::make_pair(0ul, itr);
};

std::pair<size_type, const_iterator> max_l(const AOBasisSet& bs) noexcept {
    auto itr = std::max_element(
      bs.begin(), bs.end(), [](const AOShell& shelli, const AOShell& shellj) {
          return shelli.l() < shellj.l();
      });
    return itr != bs.end() ? std::make_pair(itr->l(), itr) :
                             std::make_pair(0ul, itr);
};

std::ostream& operator<<(std::ostream& os, const AOBasisSet& bs) {
    for(const auto& shelli : bs) os << shelli;
    return os;
}

} // namespace libchemist
