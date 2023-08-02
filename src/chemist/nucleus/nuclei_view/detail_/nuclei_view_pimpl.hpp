#pragma once
#include <chemist/nucleus/nuclei_view.hpp>

namespace chemist::detail_ {


/** @brief Defines the API all NucleiView PIMPLs must implement.
 * 
 *  To implement a new NucleiView derive from this class and implement:
 *  - pimpl_pointer clone() const
 *  - reference get_nukes(size_type)
 *  - const_reference get_nukes(size_type) const
 *  - size_type size() const noexcept
 *  - bool are_equal(const NucleiViewPIMPL&) const noexcept
 */
class NucleiViewPIMPL {
public:
    /// Type *this implements
    using nuclei_view_type = NucleiView;

    /// Type nuclei_view_type is a view of
    using nuclei_type = typename nuclei_view_type::nuclei_type;

    /// Type of a pointer to *this
    using pimpl_pointer = typename nuclei_view_type::pimpl_pointer;

    /// Mutable reference to a nucleus
    using reference = typename nuclei_view_type::reference;

    /// Read-only reference to a nucleus
    using const_reference = typename nuclei_view_type::const_reference;
 
    /// Type nuclei_view_type uses for indexing
    using size_type = typename nuclei_view_type::size_type;

    /// No-op because *this has no state
    NucleiViewPIMPL() noexcept = default;

    /// No-op polymorphic dtor,
    virtual ~NucleiViewPIMPL() noexcept = default;

    pimpl_pointer clone() const { return clone_(); }

    reference get_nuke(size_type i) { return get_nuke_(i); }

    const_reference get_nuke(size_type i) const {
        return get_nuke_(i);
    }

    size_type size() const noexcept { return size_(); }

    bool are_equal(const NucleiViewPIMPL& rhs)const noexcept{
        return are_equal_(rhs) && rhs.are_equal_(*this);
    }

protected:
    /// Derived class overrides to implement clone
    virtual pimpl_pointer clone_() const = 0;

    /// Derived class overrides to implement get_nuke()
    virtual reference get_nuke_(size_type i) = 0;

    /// Derived class overrides to implement get_nuke() const
    virtual const_reference get_nuke_(size_type i) const = 0;

    /// Derived class overrides to implement size
    virtual size_type size_() const noexcept = 0;

    /// Derived class overrides to implement are_equal
    virtual bool are_equal_() const noexcept = 0;
};

}