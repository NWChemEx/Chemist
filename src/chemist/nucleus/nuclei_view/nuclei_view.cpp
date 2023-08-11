#include "detail_/nuclei_view_pimpl.hpp"

namespace chemist {

// -----------------------------------------------------------------------------
// -- CTors, Assignment, and Dtor
// -----------------------------------------------------------------------------

NucleiView::NucleiView(pimpl_pointer pimpl) noexcept :
    m_pimpl_(std::move(pimpl)) {}

NucleiView::Nucleiview(const NucleiView& other) :
    m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

NucleiView::Nucleiview(NucleiView&& other) noexcept = default;

NucleiView& NucleiView::operator=(const NucleiView& rhs) {
    if(this != &rhs) NucleiView(rhs).swap(*this);
    return *this;
}

NucleiView& NucleiView::operator=(NucleiView&& rhs) noexcept = default;

NucleiView::~NucleiView() noexcept = default;

void NucleiView::swap(NucleiView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

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

// -- Free functions
// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const NucleiView& nukes) {
    for(const auto& nuke_i : nukes) os << nuke_i << std::endl;
    return os;
}

}