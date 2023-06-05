// This file meant only for inclusion from nucleus_view.hpp

namespace chemist {

// -- Inline implementations ---------------------------------------------------

#define NUCLEUS_VIEW NucleusView<NucleusType>

template<typename NucleusType>
NUCLEUS_VIEW::NucleusView(nucleus_reference nuke) :
  NucleusView(nuke.name(), nuke.Z(), nuke.mass(), charge_view_type(nuke)) {}

template<typename NucleusType>
NUCLEUS_VIEW::NucleusView(name_reference name, atomic_number_reference Z,
                          mass_reference m, charge_view_type q) :
  charge_view_type(q), m_pname_(&name), m_pZ_(&Z), m_pmass_(&m) {}

template<typename NucleusType>
bool NUCLEUS_VIEW::operator==(const_nucleus_reference rhs) const noexcept {
    const charge_view_type& plhs = *this;
    const_charge_view prhs(rhs);

    return std::tie(name(), Z(), mass(), plhs) ==
           std::tie(rhs.name(), rhs.Z(), rhs.mass(), prhs);
}

template<typename NucleusType>
template<typename T>
bool NUCLEUS_VIEW::operator==(const NucleusView<T>& rhs) const noexcept {
    const charge_view_type& plhs = *this;
    const_charge_view prhs(rhs.charge(), rhs.x(), rhs.y(), rhs.z());

    return std::tie(name(), Z(), mass(), plhs) ==
           std::tie(rhs.name(), rhs.Z(), rhs.mass(), prhs);
}

template<typename NucleusType>
typename NUCLEUS_VIEW::nucleus_type NUCLEUS_VIEW::as_nucleus() const {
    return nucleus_type(name(), Z(), mass(), this->x(), this->y(), this->z(),
                        this->charge());
}

template<typename NucleusType>
NUCLEUS_VIEW::operator NucleusView<const nucleus_type>() const noexcept {
    return NucleusView<const nucleus_type>(
      name(), Z(), mass(),
      const_charge_view(this->charge(), this->x(), this->y(), this->z()));
}

#undef NUCLEUS_VIEW

} // namespace chemist
