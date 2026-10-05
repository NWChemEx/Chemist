/*
 * Copyright 2026 NWChemEx-Project
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

#include <chemist/experimental/basis_set/molecular_basis_set_class.hpp>
#include <chemist/experimental/detail_/float_arithmetic.hpp>
#include <chemist/types/floating_point.hpp>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

namespace chemist::experimental {
namespace {

/// The concrete types the parameter arrays may hold; see PointSet::fp_types
using fp_types = chemist::types::floating_point_types;

/** @brief Throws if @p mine and @p theirs hold different concrete types.
 *
 *  A FloatBuffer holds one concrete type, so comparing one element of each
 *  buffer suffices. A buffer with no elements yet can take any type.
 *
 *  @throw std::runtime_error if the types differ. Strong throw guarantee.
 */
template<typename LHSType, typename RHSType>
void check_same_type(const LHSType& mine, const RHSType& theirs,
                     const std::string& what) {
    if(mine.type_info() == theirs.type_info()) return;
    throw std::runtime_error(
      "chemist::experimental::MolecularBasisSet: can not add the atomic basis "
      "set because its " +
      what +
      " are not of the same floating-point type as those already in the "
      "set.");
}

/// Wraps check_same_type for two parameter arrays, which may be empty
template<typename BufferType>
void check_same_buffer_type(const BufferType& mine, const BufferType& theirs,
                            const std::string& what) {
    if(mine.size() == 0 || theirs.size() == 0) return;
    check_same_type(mine.at(0), theirs.at(0), what);
}

} // namespace

void MolecularBasisSet::push_back(const_atomic_basis_set_view_type atom) {
    // Appending may reallocate the state of *this, so an atom aliasing that
    // state is copied out of it first.
    const auto* name_ptr = &atom.get_name();
    const std::less<const name_type*> before;
    if(!m_names_.empty() && !before(name_ptr, m_names_.data()) &&
       before(name_ptr, m_names_.data() + m_names_.size())) {
        const auto copy = atom.as_atomic_basis_set();
        push_back(copy);
        return;
    }

    // Everything which can fail for a reason other than running out of
    // memory is checked before *this is touched: the floating-point types
    // must match the ones already in *this, and must be ones chemist can
    // copy. A buffer holds one type, so copying one element of each checks
    // the latter.
    const auto cs = atom.get_coefficient_buffer();
    const auto es = atom.get_exponent_buffer();
    check_same_buffer_type(std::as_const(m_coefficients_).as_view(), cs,
                           "coefficients");
    check_same_buffer_type(std::as_const(m_exponents_).as_view(), es,
                           "exponents");
    if(cs.size() != 0) detail_::copy(cs.at(0));
    if(es.size() != 0) detail_::copy(es.at(0));

    auto center = atom.get_center().as_point();
    if(!m_centers_.empty()) {
        const auto& centers = std::as_const(m_centers_);
        check_same_type(centers.get_x_buffer().at(0), center.get_x(),
                        "center coordinates");
        check_same_type(centers.get_y_buffer().at(0), center.get_y(),
                        "center coordinates");
        check_same_type(centers.get_z_buffer().at(0), center.get_z(),
                        "center coordinates");
    }

    // Allocations which can be made without touching *this's state
    name_type name      = atom.get_name();
    const auto n_shells = atom.size();
    m_ls_.reserve(m_ls_.size() + n_shells);
    m_primitive_offsets_.reserve(m_primitive_offsets_.size() + n_shells);
    m_shell_offsets_.reserve(m_shell_offsets_.size() + 1);
    m_names_.reserve(m_names_.size() + 1);
    m_atomic_numbers_.reserve(m_atomic_numbers_.size() + 1);
    m_purities_.reserve(m_purities_.size() + 1);
    m_orderings_.reserve(m_orderings_.size() + 1);

    // The type-erased buffers can not reserve without knowing their concrete
    // type, nor be truncated, so a failed allocation here can not be undone.
    // Emptying *this at least leaves it consistent.
    try {
        for(size_type p = 0; p < cs.size(); ++p) {
            m_coefficients_.template push_back<fp_types>(
              detail_::copy(cs.at(p)));
            m_exponents_.template push_back<fp_types>(detail_::copy(es.at(p)));
        }
        m_centers_.push_back(center);
    } catch(...) {
        clear_();
        throw;
    }

    // The capacity for everything below was reserved, so nothing below throws
    const auto first_primitive = m_primitive_offsets_.back();
    for(size_type s = 0; s < n_shells; ++s) {
        m_ls_.push_back(atom.get_l(s));
        m_primitive_offsets_.push_back(first_primitive +
                                       atom.primitive_range(s).second);
    }
    m_shell_offsets_.push_back(m_shell_offsets_.back() + n_shells);
    m_names_.push_back(std::move(name));
    m_atomic_numbers_.push_back(atom.get_atomic_number());
    m_purities_.push_back(atom.purity());
    m_orderings_.push_back(atom.ordering());
}

void MolecularBasisSet::swap(MolecularBasisSet& other) noexcept {
    std::swap(m_coefficients_, other.m_coefficients_);
    std::swap(m_exponents_, other.m_exponents_);
    m_ls_.swap(other.m_ls_);
    m_primitive_offsets_.swap(other.m_primitive_offsets_);
    m_shell_offsets_.swap(other.m_shell_offsets_);
    m_centers_.swap(other.m_centers_);
    m_names_.swap(other.m_names_);
    m_atomic_numbers_.swap(other.m_atomic_numbers_);
    m_purities_.swap(other.m_purities_);
    m_orderings_.swap(other.m_orderings_);
}

void MolecularBasisSet::clear_() noexcept {
    m_coefficients_ = buffer_type{};
    m_exponents_    = buffer_type{};
    m_ls_.clear();
    // Erasing, rather than clearing and re-adding the leading 0, can not
    // allocate
    m_primitive_offsets_.erase(m_primitive_offsets_.begin() + 1,
                               m_primitive_offsets_.end());
    m_shell_offsets_.erase(m_shell_offsets_.begin() + 1,
                           m_shell_offsets_.end());
    m_centers_ = center_set_type{};
    m_names_.clear();
    m_atomic_numbers_.clear();
    m_purities_.clear();
    m_orderings_.clear();
}

} // namespace chemist::experimental
