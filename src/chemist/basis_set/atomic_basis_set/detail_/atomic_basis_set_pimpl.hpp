/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chemist/basis_set/atomic_basis_set/atomic_basis_set.hpp>
#include <chemist/basis_set/atomic_basis_set/atomic_basis_set_traits.hpp>
#include <optional>

namespace chemist::basis_set::detail_ {

/** @brief Holds the state for an AtomicBasisSet object.
 *
 *  @tparam ShellType expected to be a non-cv qualified specialization of Shell.
 */
template<typename ShellType>
class AtomicBasisSetPIMPL {
public:
    /// Type *this is implementing
    using parent_type = AtomicBasisSet<ShellType>;

    using abs_traits = AtomicBasisSetTraits<parent_type>;

    using size_type = typename parent_type::size_type;

    using coefficient_type = typename abs_traits::coefficient_type;

    using exponent_type = typename abs_traits::exponent_type;

    AtomicBasisSetPIMPL() = default;

    explicit AtomicBasisSetPIMPL(
      typename abs_traits::center_type center) noexcept :
      m_center_(std::move(center)) {}

    AtomicBasisSetPIMPL(typename abs_traits::name_type name,
                        typename abs_traits::atomic_number_type atomic_n,
                        typename abs_traits::center_type center) noexcept :
      m_name_(std::move(name)),
      m_z_(std::move(atomic_n)),
      m_center_(std::move(center)) {}

    AtomicBasisSetPIMPL(
      typename abs_traits::center_type center,
      std::vector<typename abs_traits::const_shell_reference> shells);

    AtomicBasisSetPIMPL(
      typename abs_traits::name_type name,
      typename abs_traits::atomic_number_type atomic_n,
      typename abs_traits::center_type center,
      std::vector<typename abs_traits::const_shell_reference> shells);

    void add_shell(typename abs_traits::pure_type pure,
                   typename abs_traits::angular_momentum_type l,
                   typename abs_traits::const_cg_reference cg);

    typename abs_traits::name_reference basis_set_name() { return m_name_; }

    typename abs_traits::const_name_reference basis_set_name() const {
        return m_name_;
    }

    typename abs_traits::atomic_number_reference atomic_number() {
        return m_z_;
    }

    typename abs_traits::const_atomic_number_reference atomic_number() const {
        return m_z_;
    }

    typename abs_traits::center_reference center() { return m_center_; }

    typename abs_traits::const_center_reference center() const {
        return m_center_;
    }

    auto operator[](size_type i) {
        return typename abs_traits::shell_reference(pure(i), l(i), cg(i));
    }

    auto operator[](size_type i) const {
        return typename abs_traits::const_shell_reference(pure(i), l(i), cg(i));
    }

    typename abs_traits::angular_momentum_reference l(size_type i) noexcept {
        return m_l_[i];
    }

    typename abs_traits::const_angular_momentum_reference l(
      size_type i) const noexcept {
        return m_l_[i];
    }

    typename abs_traits::pure_reference pure(size_type i) noexcept {
        return m_pure_[i];
    }

    typename abs_traits::const_pure_reference pure(size_type i) const noexcept {
        return m_pure_[i];
    }

    auto cg(size_type i) {
        return typename abs_traits::cg_reference(
          m_prims_per_cg_[i], m_coefs_[m_cg_offsets_[i]],
          m_exps_[m_cg_offsets_[i]], m_center_);
    }

    auto cg(size_type i) const {
        return typename abs_traits::const_cg_reference(
          m_prims_per_cg_[i], m_coefs_[m_cg_offsets_[i]],
          m_exps_[m_cg_offsets_[i]], m_center_);
    }

    typename abs_traits::range_type primitive_range(size_type shell) const {
        if(shell >= size())
            throw std::runtime_error("Non-existent shell requested");
        const auto begin   = m_cg_offsets_[shell];
        const bool is_last = shell == (size() - 1);
        const auto end = (is_last ? n_primitives() : m_cg_offsets_[shell + 1]);
        return typename abs_traits::range_type{begin, end};
    }

    size_type prim2shell(size_type i) const { return m_prim2shell_[i]; }

    auto primitive(size_type i) {
        return typename abs_traits::primitive_reference(m_coefs_[i], m_exps_[i],
                                                        m_center_);
    }

    auto primitive(size_type i) const {
        return typename abs_traits::const_primitive_reference(
          m_coefs_[i], m_exps_[i], m_center_);
    }

    size_type size() const noexcept { return m_pure_.size(); }

    size_type n_primitives() const noexcept { return m_exps_.size(); }

    bool operator==(const AtomicBasisSetPIMPL& rhs) const noexcept;

private:
    /// If set, will be the name of the basis set
    typename abs_traits::name_type m_name_;

    /// If set, will be the atomic number associated with this basis set
    typename abs_traits::atomic_number_type m_z_;

    /// Where *this is centered
    typename abs_traits::center_type m_center_;

    /// i-th element is number of primitives in cg i
    std::vector<size_type> m_prims_per_cg_;

    /// i-th element is the index of the first primitive in cg i
    std::vector<size_type> m_cg_offsets_;

    /// i-th element is the shell index of primitive i
    std::vector<size_type> m_prim2shell_;

    /// i-th element is the coefficient of the i-th primitive
    std::vector<coefficient_type> m_coefs_;

    /// i-th element is the exponent of the i-th primitive
    std::vector<exponent_type> m_exps_;

    /// i-th element is the purity of the i-th shell
    std::vector<typename abs_traits::pure_type> m_pure_;

    /// i-th element is the angular momentum of the i-th shell
    std::vector<typename abs_traits::angular_momentum_type> m_l_;
};

#define ATOMIC_BASIS_SET_PIMPL AtomicBasisSetPIMPL<ShellType>

template<typename ShellType>
ATOMIC_BASIS_SET_PIMPL::AtomicBasisSetPIMPL(
  typename abs_traits::center_type center,
  std::vector<typename abs_traits::const_shell_reference> shells) :
  m_center_(std::move(center)) {
    for(auto& i : shells) {
        this->add_shell(i.pure(), i.l(), i.contracted_gaussian());
    }
}

template<typename ShellType>
ATOMIC_BASIS_SET_PIMPL::AtomicBasisSetPIMPL(
  typename abs_traits::name_type name,
  typename abs_traits::atomic_number_type atomic_n,
  typename abs_traits::center_type center,
  std::vector<typename abs_traits::const_shell_reference> shells) :
  m_name_(std::move(name)),
  m_z_(std::move(atomic_n)),
  m_center_(std::move(center)) {
    for(auto& i : shells) {
        this->add_shell(i.pure(), i.l(), i.contracted_gaussian());
    }
}

template<typename ShellType>
void ATOMIC_BASIS_SET_PIMPL::add_shell(
  typename abs_traits::pure_type pure,
  typename abs_traits::angular_momentum_type l,
  typename abs_traits::const_cg_reference cg) {
    // This shell will have index...
    auto shell_i = size();

    // Add the purity/angular momentum
    m_pure_.push_back(pure);
    m_l_.push_back(l);

    // Record how many primitives the shell's cg has
    m_prims_per_cg_.push_back(cg.size());

    /// The first primitive will go at the end of cg
    m_cg_offsets_.push_back(m_coefs_.size());

    for(size_type i = 0; i < cg.size(); ++i) {
        m_coefs_.push_back(cg.at(i).coefficient());
        m_exps_.push_back(cg.at(i).exponent());
        m_prim2shell_.push_back(shell_i);
    }
}

template<typename ShellType>
bool ATOMIC_BASIS_SET_PIMPL::operator==(
  const AtomicBasisSetPIMPL& rhs) const noexcept {
    // N.B. cg_offsets and prim2shell exist for convenience. Their stati is
    // fully determined by prims_per_cg
    if(m_name_ != rhs.m_name_) return false;
    if(m_z_ != rhs.m_z_) return false;
    if(m_center_ != rhs.m_center_) return false;
    if(m_prims_per_cg_ != rhs.m_prims_per_cg_) return false;
    if(m_coefs_ != rhs.m_coefs_) return false;
    if(m_exps_ != rhs.m_exps_) return false;
    if(m_pure_ != rhs.m_pure_) return false;
    if(m_l_ != rhs.m_l_) return false;
    return true;
}

#undef ATOMIC_BASIS_SET_PIMPL

} // namespace chemist::basis_set::detail_