#include "detail_/nuclei_view_pimpl.hpp"

namespace chemist {

#define TPARAMS template<typename NucleiType>
#define NUCLEI_VIEW NucleiView<NucleiType>

// -----------------------------------------------------------------------------
// -- CTors, Assignment, and Dtor
// -----------------------------------------------------------------------------

TPARAMS
NUCLEI_VIEW::NucleiView() noexcept = default;

TPARAMS
NUCLEI_VIEW::NucleiView(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

TPARAMS
NUCLEI_VIEW::NucleiView(const NucleiView& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

TPARAMS
NUCLEI_VIEW::NucleiView(NucleiView&& other) noexcept = default;

TPARAMS
NUCLEI_VIEW& NUCLEI_VIEW::operator=(const NucleiView& rhs) {
    if(this != &rhs) NUCLEI_VIEW(rhs).swap(*this);
    return *this;
}

TPARAMS
NUCLEI_VIEW& NUCLEI_VIEW::operator=(NucleiView&& rhs) noexcept = default;

TPARAMS
NUCLEI_VIEW::~NucleiView() noexcept = default;

TPARAMS
void NUCLEI_VIEW::swap(NucleiView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool NUCLEI_VIEW::operator==(const nuclei_type& rhs) const noexcept {
    if(this->size() != rhs.size()) return false;
    for(size_type i = 0; i < this->size(); ++i) {
        if((*this)[i] != rhs[i]) return false;
    }
    return true;
}

TPARAMS
bool NUCLEI_VIEW::operator!=(const nuclei_type& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected and Private members
// -----------------------------------------------------------------------------

TPARAMS
typename NUCLEI_VIEW::reference NUCLEI_VIEW::at_(size_type i) {
    return m_pimpl_->get_nuke(i);
}

TPARAMS
typename NUCLEI_VIEW::const_reference NUCLEI_VIEW::at_(size_type i) const {
    return std::as_const(*m_pimpl_).get_nuke(i);
}

TPARAMS
typename NUCLEI_VIEW::size_type NUCLEI_VIEW::size_() const noexcept {
    if(has_pimpl_()) return m_pimpl_->size();
    return 0;
}

TPARAMS
bool NUCLEI_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

#undef NUCLEI_VIEW
#undef TPARAMS

template class NucleiView<Nuclei>;
template class NucleiView<const Nuclei>;

} // namespace chemist
