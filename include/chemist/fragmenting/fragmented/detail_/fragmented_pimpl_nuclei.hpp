#pragma once
#include "fragmented_pimpl.hpp"
#include <boost/container/flat_set.hpp>
#include <chemist/nucleus/nuclei.hpp>
#include <chemist/fragmenting/capping/cap_set.hpp>

namespace chemist::fragmenting::detail_ {

template<>
class FragmentedPIMPL<Nuclei> {
private:
    /// Type of *this, only used internally 
    using my_type = FragmentedPIMPL<Nuclei>;
public:
    /// Type *this fragments, and the type of the fragments
    using value_type    = Nuclei;

    /// Type of a pointer to the superset, note superset is not mutable
    using value_pointer = std::shared_ptr<const Nuclei>;

    /// Type of the container holding Cap objects
    using cap_set_type  = fragmenting::CapSet;

    /// Type Type2Fragment uses to refer to indices/offsets
    using size_type     = value_type::size_type;

    explicit FragmentedPIMPL(value_pointer nuclei,
                             cap_set_type caps = {}) : 
        m_nuclei_(nuclei), 
        m_caps_(std::move(caps)), 
        m_fragments_() {}    

    template<typename BeginItr, typename EndItr>
    FragmentedPIMPL(value_pointer nuclei, BeginItr b, EndItr e, cap_set_type caps = {}):
        m_nuclei_(nuclei), m_caps_(std::move(caps)), m_fragments_{} {
            for(; b != e; ++b){
                add_fragment(b->begin(), b->end());
            }
        }

    template<typename BeginItr, typename EndItr>
    void add_fragment(BeginItr&& b, EndItr&& e){
        m_fragments_.emplace_back(fragment_type(b, e));
    }

    size_type size() const noexcept { return m_fragments_.size(); }

    bool operator==(const my_type& rhs) const noexcept {
        if(is_null_() != rhs.is_null_()) return false;
        if(is_null_()) return true;
        if(*m_nuclei_ != (*rhs.m_nuclei_)) return false;
        if(m_caps_ != rhs.m_caps_) return false;
        return m_fragments_ == rhs.m_fragments_;
    }

private:
    /// Type storing the indices belonging to a specific fragment
    using fragment_type = boost::container::flat_set<size_type>;

    /// Type holding all of the fragments
    using fragment_container = std::vector<fragment_type>;
    
    bool is_null_() const noexcept {
        return !static_cast<bool>(m_nuclei_);
    }

    /// The superset of Nuclei
    value_pointer m_nuclei_;

    cap_set_type m_caps_;

    fragment_container m_fragments_;
};


}