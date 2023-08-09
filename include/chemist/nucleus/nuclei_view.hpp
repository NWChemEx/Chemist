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
};

} // namespace chemist