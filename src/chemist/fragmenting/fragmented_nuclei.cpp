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

    /// How we store the fragments
    using nucleus_map_type = typename parent_type::nucleus_map_type;

    /// Type holding the caps
    using cap_set_type = typename parent_type::cap_set_type;

    /// Type used for indexing and offsets
    using size_type = typename parent_type::size_type;

    FragmentedNucleiPIMPL(nucleus_map_type frags, cap_set_type caps) :
      m_frags_(std::move(frags)), m_caps_(std::move(caps)) {}

    auto& frag(size_type i) { return m_frags_[i]; }

    const auto& frag(size_type i) const { return m_frags_[i]; }

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

private:
    nucleus_map_type m_frags_;

    cap_set_type m_caps_;
};

} // namespace detail_

#define TPARAMS template<typename NucleiType>
#define FRAGMENTED_NUCLEI FragmentedNuclei<NucleiType>

// -- CTors, assignment, and dtor
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
FRAGMENTED_NUCLEI::~FragmentedNuclei() noexcept = default;

// -- protected and private members

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

#undef FRAGMENTED_NUCLEI
#undef TPARAMS

template class FragmentedNuclei<Nuclei>;
template class FragmentedNuclei<const Nuclei>;

} // namespace chemist::fragmenting
