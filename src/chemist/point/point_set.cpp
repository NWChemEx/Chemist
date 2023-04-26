#include "detail_/point_set_pimpl.hpp"
#include <stdexcept>
#include <string>

namespace chemist {

#define TEMPLATE_PARAMS template<typename PointType>
#define POINT_SET PointSet<PointType>

// -- CTors and DTor -----------------------------------------------------------

TEMPLATE_PARAMS
POINT_SET::PointSet() noexcept = default;

TEMPLATE_PARAMS
POINT_SET::PointSet(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

TEMPLATE_PARAMS POINT_SET::~PointSet() noexcept = default;

// -- Accessors ----------------------------------------------------------------

TEMPLATE_PARAMS
typename POINT_SET::reference POINT_SET::at(size_type i) {
    bounds_check_(i);
    return (*m_pimpl_)[i];
}

TEMPLATE_PARAMS
typename POINT_SET::const_reference POINT_SET::at(size_type i) const {
    bounds_check_(i);
    return std::as_const(*m_pimpl_)[i];
}

TEMPLATE_PARAMS
void POINT_SET::push_back(value_type r) {
    if(!has_pimpl_()) m_pimpl_ = std::make_unique<pimpl_type>();
    m_pimpl_->push_back(std::move(r));
}

TEMPLATE_PARAMS
typename POINT_SET::size_type POINT_SET::size() const noexcept {
    if(!has_pimpl_()) return 0;
    return m_pimpl_->size();
}

// -- Private ------------------------------------------------------------------

TEMPLATE_PARAMS
bool POINT_SET::has_pimpl_() const noexcept { return !(m_pimpl_ == nullptr); }

TEMPLATE_PARAMS
void POINT_SET::bounds_check_(size_type i) const {
    if(i < size()) return;
    throw std::out_of_range(std::to_string(i) + " is not in the range [0, " +
                            std::to_string(size()) + ").");
}

#undef POINT_SET
#undef TEMPLATE_PARAMS

template class PointSet<float>;
template class PointSet<double>;

} // namespace chemist
