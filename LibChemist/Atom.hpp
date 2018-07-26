#pragma once
#include <array> // For the coordinates
#include <memory> // For unique pointer
#include <string> // For name of atom

namespace LibChemist {
namespace detail_ {
class AtomPIMPL;
}

/** @brief A class for holding the details of an atomic unit.
 *
 *  Atoms are considered the first tier of input to a computational chemistry
 *  program.  That is, most types of computational chemistry algorithms need to
 *  know where in space the atoms are located and what their chemical identity
 *  is.  This class also additionally holds the mass of the atom and a string
 *  identifier.
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
    /// The type of a counting number and the atomic number
    using size_type = std::size_t;

    /// The type the mass is stored as
    using mass_type = double;

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
     * there is insufficient memory to perform the copy.
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
    explicit Atom(const coord_type& coords_in, Args&&...args) :
      Atom(std::forward<Args>(args)...){
        constexpr bool is_carts =
          std::disjunction_v<std::is_same<std::decay_t<Args>, coord_type>...>;
        static_assert(!is_carts, "Please only provide one set of coordinates");
        coords() = coords_in;
    }

    template<typename...Args>
    explicit Atom(const name_type& da_name, Args&&...args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_name =
          std::disjunction_v<std::is_same<std::decay_t<Args>, name_type>...>;
        static_assert(!is_name, "Please only provide one name");
        name() = da_name;
    }

    template<typename...Args>
    explicit Atom(const mass_type& mass_in, Args&&...args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_mass =
          std::disjunction_v<std::is_same<std::decay_t<Args>, mass_type>...>;
        static_assert(!is_mass, "Please only provide one mass");
        mass() = mass_in;
    }

    template<typename...Args>
    explicit Atom(size_type Z_in, Args&&...args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_Z =
          std::disjunction_v<std::is_same<std::decay_t<Args>, size_type>...>;
        static_assert(!is_Z, "Please only provide one atomic number");
        Z() = Z_in;
    }

    explicit Atom(std::unique_ptr<detail_::AtomPIMPL> pimpl);
    ///@}


    /// Frees the PIMPL instance
    ~Atom() noexcept;

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

    size_type& Z() noexcept;
    const size_type& Z() const noexcept{ return const_cast<Atom&>(*this).Z(); }

    coord_type& coords() noexcept;
    const coord_type& coords() const noexcept {
        return const_cast<Atom&>(*this).coords();
    }
    auto& operator[](size_type i) noexcept { return coords()[i]; }
    const auto& operator[](size_type i) const noexcept { return coords()[i]; }

    mass_type& mass() noexcept;
    const mass_type& mass() const noexcept {
        return const_cast<Atom&>(*this).mass();
    }
    ///@}

private:
    /// Actual implementation of the Atom class
    std::unique_ptr<detail_::AtomPIMPL> pimpl_;

}; // End Atom

/**
 * @defgroup Atom comparison operators
 * @relates Atom
 * @brief Allows one to compare two atom instances for exact equality.
 *
 * Two atom instances are defined as equal if they have the same atomic number,
 * the same mass, and the same coordinates.  The name field is considered
 * metadata and is not considered in the comparison.  *N.B* that floating-point
 * comparisons are bit-wise with zero tolerance for deviation, *i.e.*,
 * 1.99999999999999 != 2.00000000000000
 *
 * @param[in] lhs The Atom instance on the left of the equivalence operation
 * @param[in] rhs The Atom instance on the right of the equivalence operation
 * @return Whether the two atoms obey the requested equivalence relation.
 *
 * @throw none all comparisons are no throw guarantee.
 */
///@{
bool operator==(const Atom& lhs, const Atom& rhs) noexcept;
inline bool operator!=(const Atom& lhs, const Atom& rhs) noexcept {
    return !(lhs == rhs);
}
///@}

} // namespace LibChemist

/**
 * @brief Makes it so the Atom class can be printed out.
 *
 * @param os The output stream to print to.
 * @param ai The Atom instance to print to the stream.
 * @return The output stream containing the atom instance.
 * @throws std::ios_base::failure if anything goes wrong while writing.  Weak
 *         throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const LibChemist::Atom& ai);
