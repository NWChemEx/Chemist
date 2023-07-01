#include "detail_/atomic_basis_set_pimpl.hpp"
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_view.hpp>

namespace chemist {

#define ATOMIC_BS_TPARAMS template<typename AtomicBasisSetType>
#define ATOMIC_BS_VIEW AtomicBasisSetView<AtomicBasisSetType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::AtomicBasisSetView() noexcept = default;

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::AtomicBasisSetView(const AtomicBasisSetView& rhs) :
  AtomicBasisSetView(
    rhs.has_pimpl_() ? std::make_unique<pimpl_type>(*rhs.m_pimpl_) : nullptr) {}

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::AtomicBasisSetView(AtomicBasisSetView&& rhs) noexcept = default;

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW& ATOMIC_BS_VIEW::operator=(const AtomicBasisSetView& rhs) {
    if(&rhs != this) AtomicBasisSetView(rhs).swap(*this);
    return *this;
}

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW& ATOMIC_BS_VIEW::operator=(AtomicBasisSetView&& rhs) noexcept =
  default;

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::AtomicBasisSetView(
  name_reference name, atomic_number_reference atomic_n, size_type nshells,
  pure_reference pure_per_shell, angular_momentum_reference l_per_shell,
  const_size_reference prims_per_shell, coefficient_reference cs,
  exponent_reference exp, center_reference center) :
  AtomicBasisSetView(std::make_unique<pimpl_type>(
    &name, &atomic_n, nshells, &pure_per_shell, &l_per_shell, &prims_per_shell,
    &cs, &exp, center)) {}

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::AtomicBasisSetView(
  size_type nshells, pure_reference pure_per_shell,
  angular_momentum_reference l_per_shell, const_size_reference prims_per_shell,
  coefficient_reference cs, exponent_reference exp, center_reference center) :
  AtomicBasisSetView(std::make_unique<pimpl_type>(
    nullptr, nullptr, nshells, &pure_per_shell, &l_per_shell, &prims_per_shell,
    &cs, &exp, center)) {}

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::~AtomicBasisSetView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters and setters
// -----------------------------------------------------------------------------

ATOMIC_BS_TPARAMS
bool ATOMIC_BS_VIEW::has_name() const noexcept {
    if(is_null()) return false;
    return m_pimpl_->has_name();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::name_reference ATOMIC_BS_VIEW::basis_set_name() {
    assert_name_();
    return m_pimpl_->name();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::const_name_reference ATOMIC_BS_VIEW::basis_set_name()
  const {
    assert_name_();
    return m_pimpl_->name();
}

ATOMIC_BS_TPARAMS
bool ATOMIC_BS_VIEW::has_atomic_number() const noexcept {
    if(is_null()) return false;
    return m_pimpl_->has_atomic_number();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::atomic_number_reference
ATOMIC_BS_VIEW::atomic_number() {
    assert_atomic_number_();
    return m_pimpl_->atomic_number();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::const_atomic_number_reference
ATOMIC_BS_VIEW::atomic_number() const {
    assert_atomic_number_();
    return m_pimpl_->atomic_number();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::size_type ATOMIC_BS_VIEW::n_aos() const noexcept {
    if(is_null()) return 0;
    return m_pimpl_->n_aos();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::size_type ATOMIC_BS_VIEW::n_primitives()
  const noexcept {
    if(is_null()) return 0;
    return m_pimpl_->n_primitives();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::range_type ATOMIC_BS_VIEW::primitive_range(
  size_type shell) const {
    assert_shell_index_(shell);
    return m_pimpl_->primitive_range(shell);
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::size_type ATOMIC_BS_VIEW::primitive_to_shell(
  size_type primitive) const {
    assert_primtive_index_(primitive);
    return m_pimpl_->primitive_to_shell(primitive);
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::primitive_reference ATOMIC_BS_VIEW::primitive(
  size_type i) {
    assert_primitive_index_(i);
    return m_pimpl_->primitive(i);
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::const_primitive_reference ATOMIC_BS_VIEW::primitive(
  size_type i) const {
    assert_primitive_index_(i);
    return m_pimpl_->primitive(i);
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

ATOMIC_BS_TPARAMS
void ATOMIC_BS_VIEW::swap(AtomicBasisSetView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

ATOMIC_BS_TPARAMS
bool ATOMIC_BS_VIEW::is_null() const noexcept { return !has_pimpl_(); }

ATOMIC_BS_TPARAMS
bool ATOMIC_BS_VIEW::operator==(const AtomicBasisSetView& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both alias null
    return *m_pimpl_ == *rhs.m_pimpl_;
}

// -----------------------------------------------------------------------------
// -- Protected and private
// -----------------------------------------------------------------------------

ATOMIC_BS_TPARAMS
ATOMIC_BS_VIEW::AtomicBasisSetView(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

ATOMIC_BS_TPARAMS
void ATOMIC_BS_VIEW::assert_name_() const {
    if(has_name()) return;
    throw std::runtime_error(
      "The AtomicBasisSet aliased by this "
      "AtomicBasisSetView does not have a name assigned to it.");
}

ATOMIC_BS_TPARAMS
void ATOMIC_BS_VIEW::assert_atomic_number_() const {
    if(has_name()) return;
    throw std::runtime_error(
      "The AtomicBasisSet aliased by this "
      "AtomicBasisSetView does not have an atomic number assigned to it.");
}

ATOMIC_BS_TPARAMS
void ATOMIC_BS_VIEW::assert_non_null_() const {
    if(!is_null()) return;
    throw std::runtime_error("The AtomicBasisSet aliased by this "
                             "AtomicBasisSetView is null.");
}

ATOMIC_BS_TPARAMS
void ATOMIC_BS_VIEW::assert_primitive_index_(size_type i) const {
    if(i < n_primitives()) return;
    throw std::out_of_range("Primitive index i = " + std::to_string(i) +
                            " is not in the"
                            " range [0, n_primitives()). n_primitives() = " +
                            std::to_string(n_primitives()));
}

ATOMIC_BS_TPARAMS
void ATOMIC_BS_VIEW::assert_shell_index_(size_type i) const {
    if(i < size_()) return;
    throw std::out_of_range("Shell index i = " + std::to_string(i) +
                            " is not in the"
                            " range [0, size()). size() = " +
                            std::to_string(size_()));
}

ATOMIC_BS_TPARAMS
bool ATOMIC_BS_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::size_type ATOMIC_BS_VIEW::size_() const noexcept {
    if(is_null()) return 0;
    return m_pimpl_->size();
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::reference ATOMIC_BS_VIEW::at_(size_type i) {
    return (*m_pimpl_)[i];
}

ATOMIC_BS_TPARAMS
typename ATOMIC_BS_VIEW::const_reference ATOMIC_BS_VIEW::at_(
  size_type i) const {
    return (*m_pimpl_)[i];
}

#undef ATOMIC_BS_VIEW
#undef ATOMIC_BS_TPARAMS

} // namespace chemist