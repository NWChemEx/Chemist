#pragma once
#include <chemist/nucleus/nuclei_view.hpp>

namespace chemist::detail_ {

class NuceliViewPIMPL {
public:
    /// Type *this implements
    using nuclei_view_type = NucleiView;

    /// Type of a pointer to *this
    using pimpl_pointer = typename nuclei_view_type::pimpl_pointer;

    /// Mutable reference to a nucleus
    using reference = typename nuclei_view_type::reference;

    /// Read-only reference to a nucleus
    using const_reference = typename nuclei_view_type::const_reference;
 
    /// Type nuclei_view_type uses for indexing
    using size_type = typename nuclei_view_type::size_type;

    virutal ~NucleiViewPIMPL() noexcpet = default;

    pimpl_pointer clone() const { return clone_(); }

    reference get_nuke(size_type i) { return get_nuke_(i); }

    const_reference get_nuke(size_type i) const {
        return get_nuke_(i);
    }

    size_type size() const noexcept { return size_(); }

protected:
    /// Derived class overrides to implement clone
    virtual clone_() const = 0;

    /// Derived class overrides to implement get_nuke()
    virtual reference get_nuke_(size_type i) = 0;

    /// Derived class overrides to implement get_nuke() const
    virtual const_reference get_nuke_(size_type i) const = 0;

    /// Derived class overrides to implement size
    virtual size_type size_() const noexcept = 0;
};

}