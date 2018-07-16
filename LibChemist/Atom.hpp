#pragma once
#include <Utilities/SmartEnum.hpp>
#include <memory>
#include <array>
#include <map>
#include <string>

namespace LibChemist {
namespace detail_ {class AtomPIMPL;}

/// List of properties an Atom may have.
DECLARE_SmartEnum(AtomProperty, mass, isotope_mass, cov_radius, vdw_radius);

/*! \brief A point with some associated properties
 *
 * The majority of computations in electronic structure theory assume that
 * the nuclei are point-like and the electrons move around in the resulting
 * field.  The current class aggregates the properties one typically
 * associates with a nucleus or other point-like object (e.g. ghost atom or
 * point-charge).  In order to keep the definition of various point-like
 * objects consistent users are encouraged to use the free functions
 * create_ghost, create_charge, and create_dummy to turn existing atoms into
 * ghost, point-charges, and dummy atoms respectively.
 *
 */
class Atom {
public:
    /// The type of an Atom's Property enum
    using Property = AtomProperty;

    /// The type the properties are stored as
    using property_type = double;

    /// The type of a counting number
    using size_type = std::size_t;

    /// The type of the atomic coordinates
    using coord_type = std::array<double, 3>;

    Atom();
    Atom(const Atom& rhs);
    Atom(Atom&& rhs) noexcept;
    Atom& operator=(Atom rhs) noexcept;
    Atom& operator=(Atom&& rhs) noexcept;
    ~Atom();


    template<typename...Args>
    Atom(const Property& prop, property_type value, Args...args):
        Atom(prop_map_type{{prop, value}}, args...){}

    size_type count(const Property& prop)const noexcept;

    /**
     * @name Coordinate getters/setters
     *
     *
     *
     * @throws
     */
    ///@{
    coord_type& coords() noexcept;
    const coord_type& coords()const noexcept {
        return const_cast<Atom&>(*this).coords();
    }
    typename coord_type::value_type& operator[](size_type i) noexcept {
        return coords()[i];
    }
    const typename coord_type::value_type& operator[](size_type i) const
    noexcept {
        return coords()[i];
    }
    ///@}


    /**
     * @name Property getters/setters
     *
     * The functions in this group can be used to access the properties of the
     * atom.  For non-const Atom instances the returned value is read/write,
     * whereas for const Atom instances it is read-only.  For convenience we
     * also support a map-like syntax where operator[] is overloaded to return
     * properties when given an enum.
     *
     * @param[in] prop The enumeration of the requested property.
     *
     * @return The requested property in a read/write state for non-const
     * versions or in a read-only state for const versions.
     *
     * @throws std::out_of_range if the atom does not contain the requested
     * property.  Strong throw guarantee.
     */
    ///@{
    property_type& property(const Property& prop);
    const property_type& property(const Property& prop)const {
        return const_cast<Atom&>(*this).property(prop);
    }
    property_type& operator[](const Property& prop){ return property(prop);}
    const property_type& operator[](const Property& prop)const {
        return property(prop);
    }
    ///@}

private:
    using prop_map_type = std::map<Property, double>;

    template<typename...Args>
    Atom(prop_map_type&& map, const Property& prop, property_type value,
         Args...args): Atom([&](){map[prop]=value; return map;}(), args...){}

    Atom(prop_map_type&& map);

    std::unique_ptr<detail_::AtomPIMPL> pimpl_;

}; //End Atom

} // namespace LibChemist
