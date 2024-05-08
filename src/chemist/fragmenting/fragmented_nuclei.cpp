/*
 * Copyright 2024 NWChemEx-Project
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

#include <chemist/fragmenting/fragmented_nuclei.hpp>

namespace chemist::fragmenting {
namespace detail_ {
namespace {} // namespace

template<typename NucleiType>
class FragmentedNucleiPIMPL {
public:
    /// Type *this implements
    using parent_type = FragmentedNuclei<NucleiType>;

    /// Type of a reference to a fragment
    using reference = typename parent_type::reference;

    /// Type of a read-only reference to a fragment
    using const_reference = typename parent_type::const_reference;

    /// Storage format for a fragment
    using nucleus_index_set = typename parent_type::nucleus_index_set;

    /// How we store the fragments
    using nucleus_map_type = typename parent_type::nucleus_map_type;

    /// Type holding the caps
    using cap_set_type = typename parent_type::cap_set_type;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    FragmentedNucleiPIMPL() = default;

    FragmentedNucleiPIMPL(nucleus_map_type frags, cap_set_type caps) :
      m_frags_(std::move(frags)), m_caps_(std::move(caps)) {}

    auto& frag(size_type i) { return m_frags_[i]; }

    const auto& frag(size_type i) const { return m_frags_[i]; }

    void add_fragment(nucleus_index_set frag) {
        m_frags_.emplace_back(std::move(frag));
    }

    auto& cap_set() { return m_caps_; }
    const auto& cap_set() const { return m_caps_; }

    size_type size() const noexcept { return m_frags_.size(); }

    reference cap_nuclei(size_type i) {
        auto& frag_i = frag(i);
        if constexpr(std::is_same_v<std::decay_t<NucleiType>, NucleiType>) {
            return cap_set().get_cap_nuclei(frag_i.begin(), frag_i.end());
        } else {
            return std::as_const(cap_set()).get_cap_nuclei(frag_i.begin(),
                                                           frag_i.end());
        }
    }

    const_reference cap_nuclei(size_type i) const {
        const auto& frag_i = frag(i);
        return cap_set().get_cap_nuclei(frag_i.begin(), frag_i.end());
    }

    bool operator==(const FragmentedNucleiPIMPL& rhs) const noexcept {
        return std::tie(m_frags_, m_caps_) ==
               std::tie(rhs.m_frags_, rhs.m_caps_);
    }

private:
    nucleus_map_type m_frags_;

    cap_set_type m_caps_;
};

} // namespace detail_

#define TPARAMS template<typename NucleiType>
#define FRAGMENTED_NUCLEI FragmentedNuclei<NucleiType>

// -- CTors, assignment, and dtor ----------------------------------------------
TPARAMS
FRAGMENTED_NUCLEI::FragmentedNuclei() noexcept = default;

TPARAMS
FRAGMENTED_NUCLEI::FragmentedNuclei(supersystem_type supersystem) :
  FragmentedNuclei(std::move(supersystem), nucleus_map_type{}) {}

TPARAMS
FRAGMENTED_NUCLEI::FragmentedNuclei(supersystem_type supersystem,
                                    nucleus_map_type frags) :
  FragmentedNuclei(std::move(supersystem), std::move(frags), cap_set_type{}) {}

TPARAMS
FRAGMENTED_NUCLEI::FragmentedNuclei(supersystem_type supersystem,
                                    nucleus_map_type frags, cap_set_type caps) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(frags), std::move(caps))),
  base_type(std::move(supersystem)) {}

TPARAMS
FRAGMENTED_NUCLEI::FragmentedNuclei(const FragmentedNuclei& other) :
  m_pimpl_(!this->is_null() ? std::make_unique<pimpl_type>(*other.m_pimpl_) :
                              nullptr),
  base_type(other) {}

TPARAMS
FRAGMENTED_NUCLEI::FragmentedNuclei(FragmentedNuclei&& other) noexcept =
  default;

TPARAMS
FRAGMENTED_NUCLEI& FRAGMENTED_NUCLEI::operator=(const FragmentedNuclei& rhs) {
    if(this != &rhs) FragmentedNuclei(rhs).swap(*this);
    return *this;
}

TPARAMS
FRAGMENTED_NUCLEI& FRAGMENTED_NUCLEI::operator=(
  FragmentedNuclei&& rhs) noexcept = default;

TPARAMS
FRAGMENTED_NUCLEI::~FragmentedNuclei() noexcept = default;

// -- Getters and setters ------------------------------------------------------

TPARAMS
void FRAGMENTED_NUCLEI::insert(const_reference nuclei) {
    this->assert_supersystem_();
    nucleus_index_set nuclei2;
    nuclei2.reserve(nuclei.size());
    const auto& ss = this->supersystem();
    for(const auto& ni : nuclei) {
        for(size_type i = 0; i < ss.size(); ++i) {
            if(ss[i] == ni) {
                nuclei2.push_back(i);
                break;
            }
            if(i + 1 == ss.size())
                throw std::runtime_error("Nucleus not in supersystem");
        }
    }
    insert(std::move(nuclei2));
}

TPARAMS
void FRAGMENTED_NUCLEI::insert(nucleus_index_set nuclei) {
    this->assert_supersystem_();
    for(const auto& n : nuclei)
        if(n >= this->supersystem().size())
            throw std::runtime_error(
              "Index not in range [0, supersystem().size())");
    if(!has_pimpl_()) std::make_unique<pimpl_type>().swap(m_pimpl_);
    m_pimpl_->add_fragment(std::move(nuclei));
}

TPARAMS
typename FRAGMENTED_NUCLEI::cap_set_reference FRAGMENTED_NUCLEI::cap_set() {
    this->assert_supersystem_();
    if(!has_pimpl_()) std::make_unique<pimpl_type>().swap(m_pimpl_);
    return m_pimpl_->cap_set();
}

TPARAMS
typename FRAGMENTED_NUCLEI::const_cap_set_reference FRAGMENTED_NUCLEI::cap_set()
  const {
    this->assert_supersystem_();
    assert_pimpl_();
    return std::as_const(*m_pimpl_).cap_set();
}

// -- Utility ------------------------------------------------------------------

TPARAMS
void FRAGMENTED_NUCLEI::swap(FragmentedNuclei& other) noexcept {
    base_type::swap(other);
    m_pimpl_.swap(other.m_pimpl_);
}

TPARAMS
bool FRAGMENTED_NUCLEI::operator==(
  const FragmentedNuclei& other) const noexcept {
    if(this->is_null() != other.is_null()) return false;
    if(this->is_null()) return true;
    if(!base_type::operator==(other)) return false;
    return (*m_pimpl_) == (*other.m_pimpl_);
}

TPARAMS
bool FRAGMENTED_NUCLEI::operator!=(const FragmentedNuclei& rhs) const noexcept {
    return !(*this == rhs);
}

// -- protected and private members -------------------------------------------

TPARAMS
typename FRAGMENTED_NUCLEI::reference FRAGMENTED_NUCLEI::at_(size_type i) {
    reference real(this->supersystem(), m_pimpl_->frag(i));
    auto caps = m_pimpl_->cap_nuclei(i);

    return caps.size() ? reference(std::vector<reference>{real, caps}) : real;
}

TPARAMS
typename FRAGMENTED_NUCLEI::const_reference FRAGMENTED_NUCLEI::at_(
  size_type i) const {
    const_reference real(this->supersystem(), m_pimpl_->frag(i));
    const_reference caps = std::as_const(*m_pimpl_).cap_nuclei(i);
    using vec_t          = std::vector<const_reference>;
    return caps.size() ? const_reference(vec_t{real, caps}) : real;
}

TPARAMS
typename FRAGMENTED_NUCLEI::size_type FRAGMENTED_NUCLEI::size_()
  const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

TPARAMS
bool FRAGMENTED_NUCLEI::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

TPARAMS
void FRAGMENTED_NUCLEI::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error(
      "FragmentedNuclei was not initialized with a superset");
}

#undef FRAGMENTED_NUCLEI
#undef TPARAMS

// -- Explicit template instantiations -----------------------------------------

template class FragmentedNuclei<Nuclei>;
template class FragmentedNuclei<const Nuclei>;

} // namespace chemist::fragmenting