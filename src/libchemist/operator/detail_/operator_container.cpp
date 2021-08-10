#include "operator_container_pimpl.hpp"

namespace libchemist::operators::detail_ {

OperatorContainer::OperatorContainer() noexcept = default;

OperatorContainer::~OperatorContainer() noexcept = default;

OperatorContainer::OperatorContainer(const OperatorContainer& other) {
    *this = other;
}
OperatorContainer::OperatorContainer(OperatorContainer&& other) noexcept {
    *this = std::move(other);
}

OperatorContainer& OperatorContainer::operator=(
  const OperatorContainer& other) {
    if(other.m_pimpl_)
        this->m_pimpl_ = other.pimpl_().clone();
    else {
        m_pimpl_.reset();
        m_pimpl_ = nullptr;
    }
    return *this;
}
OperatorContainer& OperatorContainer::operator=(
  OperatorContainer&& other) noexcept {
    this->m_pimpl_ = std::move(other.m_pimpl_);
    return *this;
}

bool OperatorContainer::operator==(const OperatorContainer& other) const {
    const bool lhs_pimpl = static_cast<bool>(m_pimpl_);
    const bool rhs_pimpl = static_cast<bool>(other.m_pimpl_);

    // Check if both have pimpl or both don't have a pimpl
    if(lhs_pimpl != rhs_pimpl) return false;

    // lhs_pimpl == rhs_pimpl, if false then they're the same
    if(!lhs_pimpl) return true;

    // otherwise we need to compare the PIMPLs
    return pimpl_() == other.pimpl_();
}

bool OperatorContainer::operator!=(const OperatorContainer& other) const {
    return !(*this == other);
}

bool OperatorContainer::is_equal_impl(
  const OperatorBase& other) const noexcept {
    auto pother = dynamic_cast<const OperatorContainer*>(&other);
    return pother ? (*this) == (*pother) : false;
}

void OperatorContainer::hash_impl(pluginplay::Hasher& h) const {
    if(m_pimpl_) return h(pimpl_());
}

std::string OperatorContainer::as_string_impl() const { return "O\u0302"; }

OperatorContainerPIMPL& OperatorContainer::pimpl_() {
    if(!m_pimpl_) m_pimpl_ = std::make_unique<OperatorContainerPIMPL>();
    return *m_pimpl_;
}

const OperatorContainerPIMPL& OperatorContainer::pimpl_() const {
    if(!m_pimpl_) throw std::runtime_error("OperatorContainer has no PIMPL!!!");
    return *m_pimpl_;
}

void OperatorContainer::add_term_(rtti_type index,
                                  operator_pointer<OperatorBase> op) {
    pimpl_().add_term(index, std::move(op));
}

OperatorContainer::const_type_erased_vector OperatorContainer::get_terms_(
  rtti_type index) const {
    if(m_pimpl_)
        return pimpl_().get_terms(index);
    else
        return const_type_erased_vector();
}

OperatorContainer::type_erased_vector OperatorContainer::get_terms_(
  rtti_type index) {
    if(m_pimpl_)
        return pimpl_().get_terms(index);
    else
        return type_erased_vector();
}

bool OperatorContainer::has_term_(rtti_type index) const noexcept {
    if(m_pimpl_) return pimpl_().has_term(index);
    return false;
}

} // namespace libchemist::operators::detail_
