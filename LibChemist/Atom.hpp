#pragma once
#include <Utilities/SmartEnum.hpp>
#include <array>
#include <map>
#include <memory>
#include <string>

namespace LibChemist {
namespace detail_ {
class AtomPIMPL;
}

/// List of properties an Atom may have.
DECLARE_SmartEnum(AtomProperty, mass, isotope_mass, cov_radius, vdw_radius);

/** @brief A class for holding the details of an atomic unit.
 *
 *  Atoms are considered the first tier of input to a computational chemistry
 *  program.  That is, most types of computational chemistry algorithms need to
 *  know where in space the atoms are located and what their chemical identity
 *  is.  This class holds that information in addition to some basic lookup data
 *  about the atom such as its mass, chemical symbol, etc.
 *
 *  Like many of the classes in LibChemist, the Atom class is built using the
 *  PIMPL model.  That is by changing the underlying implementation it is
 *  possible to change how the class behaves under the hood.  The documentation
 *  for the Atom class assumes the default PIMPL implementation is used. Other
 *  backends must minimally adhere to this API, but some, like the one used
 *  within the Molecule class may provide additional guarantees (coordinates
 *  and properties are contiguous in the Molecule case).
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

    /**
     * @brief Makes a default constructed Atom instance.
     *
     * A default constructed Atom is centered at the origin and has no
     * properties.
     *
     * @throw None No throw guarantee.
     */
    Atom();

    /**
     * @defgroup Copy/Move CTors and Assignment Operators
     *
     * @brief CTors and assignment operators for transferring the state of an
     *        Atom instance.
     *
     * @param[in] rhs The Atom instance to copy/move.  For move operations
     *            @p rhs is in a valid, but otherwise undefined state.
     *
     * @throw std::bad_alloc The copy ctor/assignment operator throws if
     * thier is insufficent memory to perform the copy.
     *
     *
     */
    ///@{
    Atom(const Atom& rhs);
    Atom(Atom&& rhs) noexcept;
    Atom& operator=(const Atom& rhs);
    Atom& operator=(Atom&& rhs) noexcept;
    ///@}

    ///@
    template<typename... Args>
    Atom(const Property& prop, property_type value, Args... args) :
      Atom(prop_map_type{{prop, value}}, args...) {}

    ~Atom();



    size_type count(const Property& prop) const noexcept;

    /**
     * @name Coordinate getters/setters
     *
     * These functions allow one to access the Atom's coordinates either in a
     * read/write manner (non-const variants) or in a read-only variant (for
     * const versions).
     *
     * @param[in] i For the `operator[]` component variant returns the @p i -th
     *            component of the coordinate.  @p i in the range [0, 3) is
     *            corresponds to the "x", "y", and "z" coordinates respectively.
     * @warning the `operator[]` form of coordinate retrieval performs no bounds
     * checking.  This is because whether @p i greater than 2 is valid depends
     * on the semantics of the backend.
     *
     * @throws None No throw guarantee.
     */
    ///@{
    coord_type& coords() noexcept;
    const coord_type& coords() const noexcept {
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
    const property_type& property(const Property& prop) const {
        return const_cast<Atom&>(*this).property(prop);
    }
    property_type& operator[](const Property& prop) { return property(prop); }
    const property_type& operator[](const Property& prop) const {
        return property(prop);
    }
    ///@}

private:
    using prop_map_type = std::map<Property, double>;

    template<typename... Args>
    Atom(prop_map_type&& map, const Property& prop, property_type value,
         Args... args) :
      Atom([&]() { map[prop] = value;return map; }(), args...) {}


    /// Final ctor for the variadic ctors
    Atom(prop_map_type&& map);

    /// Actual implementation of the Atom class
    std::unique_ptr<detail_::AtomPIMPL> pimpl_;

}; // End Atom

} // namespace LibChemist
