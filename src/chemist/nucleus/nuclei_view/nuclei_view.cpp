#include "detail_/nuclei_view_pimpl.hpp"

namespace chemist {

// -----------------------------------------------------------------------------
// -- CTors, Assignment, and Dtor
// -----------------------------------------------------------------------------

NucleiView::NucleiView(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

NucleiView::~NucleiView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Protected and Private members
// -----------------------------------------------------------------------------

typename NucleiView::reference NucleiView::at_(size_type i) {
    return m_pimpl_->get_nuke(i);
}

typename NucleiView::const_reference NucleiView::at_(size_type i) const {
    return std::as_const(*m_pimpl_).get_nuke(i);
}

typename NucleiView::size_type NucleiView::size_() const noexcept {
    if(has_pimpl_()) return m_pimpl_->size();
    return 0;
}

bool NucleiView::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

} // namespace chemist