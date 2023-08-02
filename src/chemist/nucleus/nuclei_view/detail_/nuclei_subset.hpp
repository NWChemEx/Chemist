#pragma once

#include<chemist/nucleus/detail_/nuclei_view_pimpl.hpp>

namespace chemist::detail_ {

/** @brief Implements a NucleiView that is a subset of a Nuclei object.
 *
 * 
 *  @note we have declared the types in a manner which will work if the class
 *        needs to be templated down the line. Without templating we do not
 *        technically need to pull in the base class's types, nor do we need
 *        typename. 
*/
class NucleiSubset: public NucleiViewPIMPL {
private:
    /// Type of the base class
    using base_type = NucleiViewPIMPL;

public:
    /// Type the view is acting as a reference of
    using nuclei_type = typename base_type::nuclei_type;

    /// Type of a shared pointer to a nuclei_type object
    using nuclei_pointer = std::shared_ptr<nuclei_type>;

    /// Type of a pointer to the base PIMPL
    using pimpl_pointer = typename base_type::pimpl_pointer;

    /// Type of a Nucleus object
    using value_type = typename base_type::value_type;

    /// Type of a mutable reference to a Nucleus object
    using reference = typename base_type::reference;

    /// Type of a read-only reference to a Nucleus object
    using const_reference = typename base_type::const_reference;

    /// Type nuclei_type uses for indexing and offsets
    using size_type = typename nuclei_type::size_type;

    NucleiSubset() noexcept = default;

    template<typename BeginItr, typename EndItr>
    NucleiSubset(nuclei_pointer supersystem, BeginItr&& begin, EndItr&& end) : 
        m_nukes_(supersystem), 
        m_members_(std::forward<BeginItr>(begin), std::forward<EndItr>(end)) {}

    NucleiSubset(const NucleiSubset& other) = default;

    bool operator==(const NucleiSubset& rhs) const noexcept;

    bool is_null() const noexcept { return static_cast<bool>(m_nukes_); }
protected:
    /// Implementes clone
    pimpl_pointer clone_() const override {
        return std::make_shared<NucleiSubset>(*this);
    }

    /// Implements getting a mutable Nucleus
    reference get_nuke_(size_type i) override { 
        return *m_nukes_[m_members_[i]]; 
    }

    /// Implements getting a read-only Nucleus
    const_reference get_nuke_(size_type i) const override {
        return std::as_const(*m_nukes_)[m_members_[i]];
    }

    /// Impelments size
    size_type size_() const noexcept override {
        return m_members_.size();
    }

    /// Implements are_equal
    bool are_equal_(const base_type& rhs) const noexcept override {
        auto prhs = dynamic_cast<const NucleiSubset*>(&rhs);
        if(prhs == nullptr) return false;
        return (*this) == (*prhs);
    }

private:
    /// The full set of nuclei
    nuclei_pointer m_nukes_;

    /// The indices in *this
    std::vector<size_type> m_members_;
};

// -----------------------------------------------------------------------------
// -- Implementations
// -----------------------------------------------------------------------------

bool NucleiSubset::operator==(const NucleiSubset& rhs) const noexcept{
    if(m_members_ != rhs.m_members_) return false;

    // Try to avoid comparing the Nuclei by checking for the same address
    if(m_nukes_ == rhs.m_nukes_) return true;

    // Above works if they're both null, now see if only one is null
    if(is_null() || rhs.is_null()) return false;

    // Both are non-null, so compare the Nuclei
    return *m_nukes_ == *rhs.m_nukes_; 
}

}