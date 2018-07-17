#pragma once
#include <Utilities/SmartEnum.hpp>
#include <array> // For the coordinates
#include <memory> // For unique pointer
#include <string> // For name of atom

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

    /// The type of the name of the Atom instance
    using name_type = std::string;

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

    /**
     * @defgroup State CTors
     *
     * @brief The CTors in this group allow a user to set the state of the Atom
     * *via* the CTor.
     *
     * The state of the Atom is set by parsing the list of input arguments by
     * type.  The following list first lists the "capture group" (in brackets)
     * followed by how that group will be interpreted.
     *
     * - `[double, double, double]`  the input coordinates.
     * - `[std::array<double, 3>]` the input coordinates.
     * - `[AtomProperty, double]` a property and its value
     * - `[std::string]` the name of the Atom.
     * - `[std::size_t]` the atomic number of the Atom.
     *
     * When providing input to the CTor, the order of the groups is irrelevant.
     *
     * @param[in] x The "x" Cartesian coordinate.
     * @param[in] y The "y" Cartesian coordinate.
     * @param[in] z The "z" Cartesian coordinate.
     * @param[in] prop The enum corresponding to the `AtomProperty` to set.
     * @param[in] value The value for the property
     * @param[in] name The name of the atom (typically the atomic symbol)
     * @param[in] Z The atomic number of the  Atom
     * @param[in] args The remaining arguments to be parsed
     *
     * @tparam Args The types of the remaining arguments to be parsed.
     *
     * @throw std::bad_alloc if there is insufficient memory to add a new
     *        property.  Strong throw guarantee.
     */
    ///@{
    template<typename...Args>
    Atom(double x, double y, double z, Args&&...args):
      Atom(std::forward<Args>(args)...) {
        coords()[0] = x; coords()[1] = y; coords()[2] = z;
    }

    template<typename...Args>
    explicit Atom(const coord_type& coords, Args&&...args) :
      Atom(coords[0], coords[1], coords[2], std::forward<Args>(args)...){}

    template<typename...Args>
    explicit Atom(const Property& prop, double value, Args&&...args) :
      Atom(std::forward<Args>(args)...) { set_property(prop, value); }

    template<typename...Args>
    explicit Atom(const name_type& da_name, Args&&...args) :
      Atom(std::forward<Args>(args)...) { name() = da_name; }

    template<typename...Args>
    explicit Atom(size_type Z, Args&&...args) :
      Atom(std::forward<Args>(args)...) { at_num() = Z;}
    ///@}


    /// Frees the PIMPL instance
    ~Atom();

    /**
     * @defgroup Z/Name setter/getter
     *
     * @brief Returns the atomic number/name of the atom.
     *
     * The returned value is read/write for non-const Atom instances and
     * read-only for const instances.
     *
     * @return The value of the atomic number/name.  If the name was not set
     *         then this returns an empty string.
     *
     * @throw None. No throw guarantee.
     */
    ///@{
    std::string& name()noexcept;
    const std::string& name() const noexcept {
        return const_cast<Atom&>(*this).name();
    }
    size_type& at_num() noexcept;
    const size_type& at_num() const noexcept{
        return const_cast<Atom&>(*this).at_num();
    }
    ///@}

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
     * @brief Returns the number of properties with the given name that were
     *        set for the Atom instance.
     *
     *
     * @param prop The enum for the property of interest.
     * @return The number of properties with the given name.  Will be in the
     *         range [0,1].
     * @throw None No throw guarantee.
     */
    size_type count(const Property& prop) const noexcept;

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

    /**
     * @brief Allows the creation of new properties
     *
     * Generally speaking I don't see a need for user's to define the properties
     * an Atom has outside of its ctor.  This function is used from the ctor to
     * set those properties.  The user may change their values through the
     * public accessors, but will not be able to add a new property (doing so
     * would throw an `std::out_of_range` exception).
     *
     * @param[in] prop The enum for the property to set
     * @param[in] value The value to set the property to
     *
     * @throw std::bad_alloc if there is insufficient memory to add the property
     *        to the instance.
     */
    void set_property(const Property& prop, property_type value);

    /// Actual implementation of the Atom class
    std::unique_ptr<detail_::AtomPIMPL> pimpl_;

}; // End Atom

} // namespace LibChemist

/**
 * @brief Makes it so the Atom class can be printed out.
 *
 * @param os The output stream to print to.
 * @param ai The Atom instance to print to the stream.
 * @return The output stream containing the atom instance.
 */
std::ostream& operator<<(std::ostream& os, const LibChemist::Atom& ai);
