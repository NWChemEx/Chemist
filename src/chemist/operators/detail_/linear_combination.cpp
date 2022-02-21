#include "linear_combination_pimpl.hpp"

namespace chemist::operators::detail_ {

LinearCombination::LinearCombination() noexcept = default;

LinearCombination::~LinearCombination() noexcept = default;

LinearCombination::LinearCombination(const LinearCombination& other) {
    *this = other;
}
LinearCombination::LinearCombination(LinearCombination&& other) noexcept {
    *this = std::move(other);
}

LinearCombination& LinearCombination::operator=(
  const LinearCombination& other) {
    if(other.m_pimpl_)
        this->m_pimpl_ = other.pimpl_().clone();
    else {
        m_pimpl_.reset();
        m_pimpl_ = nullptr;
    }
    return *this;
}
LinearCombination& LinearCombination::operator=(
  LinearCombination&& other) noexcept {
    this->m_pimpl_ = std::move(other.m_pimpl_);
    return *this;
}

bool LinearCombination::operator==(const LinearCombination& other) const {
    const bool lhs_pimpl = static_cast<bool>(m_pimpl_);
    const bool rhs_pimpl = static_cast<bool>(other.m_pimpl_);

    // Check if both have pimpl or both don't have a pimpl
    if(lhs_pimpl != rhs_pimpl) return false;

    // lhs_pimpl == rhs_pimpl, if false then they're the same
    if(!lhs_pimpl) return true;

    // otherwise we need to compare the PIMPLs
    return pimpl_() == other.pimpl_();
}

bool LinearCombination::operator!=(const LinearCombination& other) const {
    return !(*this == other);
}

LinearCombination::size_type LinearCombination::size() const noexcept {
    return m_pimpl_ ? pimpl_().size() : 0;
}

bool LinearCombination::is_equal_impl(
  const OperatorBase& other) const noexcept {
    auto pother = dynamic_cast<const LinearCombination*>(&other);
    return pother ? (*this) == (*pother) : false;
}

void LinearCombination::hash_impl(chemist::detail_::Hasher& h) const {
    if(m_pimpl_) return h(pimpl_());
}

std::string LinearCombination::as_string_impl() const { return "O\u0302"; }

LinearCombinationPIMPL& LinearCombination::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<LinearCombinationPIMPL>();
    return *m_pimpl_;
}

const LinearCombinationPIMPL& LinearCombination::pimpl_() const {
    if(!m_pimpl_) throw std::runtime_error("LinearCombination has no PIMPL!!!");
    return *m_pimpl_;
}

void LinearCombination::add_term_(rtti_type index,
                                  operator_pointer<OperatorBase> op) {
    pimpl_().add_term(index, std::move(op));
}

LinearCombination::const_type_erased_vector LinearCombination::get_terms_(
  rtti_type index) const {
    if(m_pimpl_)
        return pimpl_().get_terms(index);
    else
        return const_type_erased_vector();
}

LinearCombination::type_erased_vector LinearCombination::get_terms_(
  rtti_type index) {
    if(m_pimpl_)
        return pimpl_().get_terms(index);
    else
        return type_erased_vector();
}

bool LinearCombination::has_term_(rtti_type index) const noexcept {
    if(m_pimpl_) return pimpl_().has_term(index);
    return false;
}

} // namespace chemist::operators::detail_
