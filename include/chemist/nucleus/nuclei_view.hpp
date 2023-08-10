#pragma once
#include <chemist/nucleus/nuclei.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {
class NucleiViewPIMPL;
}

class NucleiView : public utilities::IndexableContainerBase<NucleiView> {
private:
    /// Type of *this
    using my_type = NucleiView;

    /// Type of the base
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    /// Type of the PIMPL
    using pimpl_type = detail_::NucleiViewPIMPL;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type used for indexing and offsets
    using size_type = typename base_type::size_type;

    // -- Nuclei/Nucleus types -------------------------------------------------

    /// Type *this is behaving like
    using nuclei_type = Nuclei;

    /// Type of an element in *this
    using value_type = typename nuclei_type::value_type;

    /// Mutable reference to an element in *this
    using reference = typename nuclei_type::reference;

    /// Read-only reference to an element in *this
    using const_reference = typename nuclei_type::const_reference;

    // -- PointCharge types ----------------------------------------------------

    /// Type used to store the charge
    using charge_type = typename nuclei_type::charge_type;

    // -- Ctors, Assignment, and dtor ------------------------------------------

    NucleiView(pimpl_pointer pimpl) noexcept;

    /// Default no throw dtor
    ~NucleiView() noexcept;
private:
    /// Allows the base class to access at_ and size_
    friend base_type;

    /// Implements retrieving read/write references
    reference at_(size_type i);

    /// Implements retrieving read-only references
    const_reference at_(size_type i) const;

    /// Used by base to determine the number of elements in *this
    size_type size_() const noexcept;

    /// Wraps determining if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// The object implementing *this
    pimpl_pointer m_pimpl_;

};

} // namespace chemist