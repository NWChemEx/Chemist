#include "detail_/charges_view_pimpl.hpp"

namespace chemist {

#define TPARAMS template<typename ChargesType>
#define CHARGES_VIEW ChargesView<ChargesType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

TPARAMS
CHARGES_VIEW::ChargesView() noexcept = default;

TPARAMS
CHARGES_VIEW::ChargesView(const ChargesView& other) :
  m_pimpl_(other.clone_pimpl_()) {}

TPARAMS
CHARGES_VIEW& CHARGES_VIEW::operator=(const ChargesView& rhs) {
    if(this != &rhs) rhs.clone_pimpl_().swap(m_pimpl_);
    return *this;
}

TPARAMS
CHARGES_VIEW::ChargesView(ChargesView&& other) noexcept = default;

TPARAMS
CHARGES_VIEW& CHARGES_VIEW::operator=(ChargesView&& rhs) noexcept = default;

TPARAMS
CHARGES_VIEW::~ChargesView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

TPARAMS
bool CHARGES_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
typename CHARGES_VIEW::pimpl_pointer CHARGES_VIEW::clone_pimpl_() const {
    return has_pimpl_() ? m_pimpl_->clone() : pimpl_pointer{};
}

TPARAMS
typename CHARGES_VIEW::reference CHARGES_VIEW::at_(size_type i) {
    return (*m_pimpl_)[i];
}

TPARAMS
typename CHARGES_VIEW::const_reference CHARGES_VIEW::at_(size_type i) const {
    return std::as_const(*m_pimpl_)[i];
}

TPARAMS
typename CHARGES_VIEW::size_type CHARGES_VIEW::size_() const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

#undef CHARGES_VIEW
#undef TPARAMS

template class ChargesView<Charges<float>>;
template class ChargesView<const Charges<float>>;
template class ChargesView<Charges<double>>;
template class ChargesView<const Charges<double>>;

} // namespace chemist
