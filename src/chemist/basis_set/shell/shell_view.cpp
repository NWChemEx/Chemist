/*
 * Copyright 2022 NWChemEx-Project
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

#include "../../point/detail_/point_pimpl.hpp"
#include "chemist/basis_set/shell_view.hpp"
#include "shell_pimpl.hpp"

namespace chemist {
namespace detail_ {

template<typename ShellType>
class ShellViewPIMPL {
public:
    /// Type *this is implementing
    using parent_type = ShellView<ShellType>;

    /// Pull in types from the parent class
    using contracted_gaussian_reference =
      typename parent_type::contracted_gaussian_reference;

    using angular_momentum_pointer =
      typename parent_type::angular_momentum_pointer;

    using angular_momentum_reference =
      typename parent_type::angular_momentum_reference;

    using pure_reference = typename parent_type::pure_reference;

    using pure_pointer = typename parent_type::pure_pointer;

    ShellViewPIMPL(pure_reference pure, angular_momentum_reference l,
                   contracted_gaussian_reference cg) :
      m_ppure(&pure), m_pl(&l), m_cg(std::move(cg)) {}

    bool operator==(const ShellViewPIMPL& rhs) const noexcept {
        auto lstate = std::tie(*m_ppure, *m_pl, m_cg);
        auto rstate = std::tie(*rhs.m_ppure, *rhs.m_pl, rhs.m_cg);
        return lstate == rstate;
    }

    pure_pointer m_ppure;

    angular_momentum_pointer m_pl;

    contracted_gaussian_reference m_cg;
}

} // namespace detail_

template<typename ShellType, typename... Args>
auto make_shell_pimpl(Args&&... args) {
    using pimpl_type = typename ShellView<ShellType>::pimpl_type;
    return std::make_unique<pimpl_type>(std::forward<Args>(args)...);
}

#define SHELL_VIEW ShellView<ShellType>

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

template<typename ShellType>
SHELL_VIEW::ShellView() noexcept = default;

template<typename ShellType>
SHELL_VIEW::ShellView(shell_reference shell2alias) :
  ShellView(!shell2alias.is_null() ?
              make_shell_pimpl(shell2alias.pure(), shell2alias.l(),
                               contracted_gaussian_reference(
                                 shell2alias.contracted_gaussian())) :
              nullptr) {}

template<typename ShellType>
SHELL_VIEW::ShellView(pure_reference ao_type, angular_momentum_reference l,
              contracted_gaussian_reference cg) :
  m_pimpl_(make_shell_pimpl(ao_type, l, std::move(cg)) {}

template<typename ShellType>
SHELL_VIEW::ShellView(const ShellView& other) :
  ShellView(other.has_pimpl_() ? make_shell_pimpl(*other.m_pimpl_) : nullptr) {}

template<typename ShellType>
SHELL_VIEW::ShellView(ShellView&& other) noexcept = default;

template<typename ShellType>
SHELL_VIEW& SHELL_VIEW::operator=(const ShellView& rhs) {
    if(&rhs != this) ShellView(rhs).swap(*this);
    return *this;
}

template<typename ShellType>
SHELL_VIEW& SHELL_VIEW::operator=(ShellView&& rhs) noexcept = default;

template<typename ShellType>
SHELL_VIEW::~ShellView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters/setters
// -----------------------------------------------------------------------------

template<typename ShellType>
typename SHELL_VIEW::pure_reference SHELL_VIEW::pure() {
    assert_non_null_();
    return *m_pimpl_->m_ppure;
}

template<typename ShellType>
typename SHELL_VIEW::const_pure_reference SHELL_VIEW::pure() const {
    assert_non_null_();
    return *m_pimpl_->m_ppure;
}

template<typename ShellType>
typename SHELL_VIEW::angular_momentum_reference SHELL_VIEW::l() {
    assert_non_null_();
    return *m_pimpl_->m_pl;
}

template<typename ShellType>
typename SHELL_VIEW::const_angular_momentum_reference SHELL_VIEW::l() const {
    assert_non_null_();
    return *m_pimpl_->m_pl;
}

template<typename ShellType>
typename SHELL_VIEW::contracted_gaussian_reference
SHELL_VIEW::contracted_gaussian() {
    assert_non_null_();
    return *m_pimpl_->m_cg;
}

template<typename ShellType>
typename SHELL_VIEW::const_cg_reference SHELL_VIEW::contracted_gaussian() const {
    assert_non_null_();
    return *m_pimpl_->m_cg;
}

template<typename ShellType>
typename SHELL_VIEW::size_type SHELL_VIEW::n_primitives() const noexcept {
    if(is_null()) return 0;
    return *m_pimpl_->m_cg.size();
}

template<typename ShellType>
typename SHELL_VIEW::primitive_reference SHELL_VIEW::primitive(size_type i) {
    if(has_pimpl_()) return m_pimpl_->m_cg.at(i);
    throw std::out_of_range("Index i is not in the range [0, n_primitives()))");
}

template<typename ShellType>
typename SHELL_VIEW::const_primitive_reference
SHELL_VIEW::primitive(size_type i) const {
    if(has_pimpl_()) return m_pimpl_->m_cg.at(i);
    throw std::out_of_range("Index i is not in the range [0, n_primitives()))");
}


template<typename ShellType>
typename SHELL_VIEW::size_type SHELL_VIEW::size() const noexcept {
    return size_();
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

template<typename ShellType>
void SHELL_VIEW::swap(ShellView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

template<typename ShellType>
bool SHELL_VIEW::is_null() const noexcept {
    return !has_pimpl_();
}

template<typename ShellType>
bool SHELL_VIEW::operator==(const ShellView& rhs) const noexcept {
    if(is_null() != rhs.is_null()) return false;
    if(is_null()) return true; // Both are views of the null shell
    return *m_pimpl_ == *rhs.m_pimpl_;
}

// -----------------------------------------------------------------------------
// -- Protected/Private functions
// -----------------------------------------------------------------------------

template<typename ShellType>
bool SHELL_VIEW::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

template<typename ShellType>
bool assert_non_null_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("ShellView instance is a view of a null Shell.");
}

template<typename ShellType>
typename SHELL_VIEW::size_type size_() const noexcept {
    if(is_null()) return 0;
    const auto L     = l();
    const auto p_val = 2ul * L + 1ul;
    // closed form for 2+L choose 2
    const auto c_val   = (L * L + 3ul * L + 2ul) / 2ul;
    const bool is_pure = pure() == ShellType::pure;
    return is_pure ? p_val : c_val;
}


namespace detail_ {
    template class ShellViewPIMPL<ShellD>;
    template class ShellViewPIMPL<const ShellD>;
    template class ShellViewPIMPL<ShellF>;
    template class ShellViewPIMPL<const ShellF>;

}

template class ShellView<ShellD>;
template class ShellView<const ShellD>;
template class ShellView<ShellF>;
template class ShellView<const ShellF>;

} // namespace chemist
