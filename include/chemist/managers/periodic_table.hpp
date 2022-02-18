#pragma once
#include "chemist/molecule/atom.hpp"
#include <memory> //For unique_ptr
#include <vector>

namespace chemist {
namespace detail_ {
/// Forward declaration of the PIMPL struct
struct PeriodicTablePIMPL;
} // namespace detail_

/**
 * @brief A class to contain Atom instances.
 *
 * The PeriodicTable class manages Atom instances, which are populated at
 * runtime using the provided insertion methods. The class also serves as a
 * hub for retrieving other atomic data that is not typically stored with an
 * Atom instance.
 */
class PeriodicTable {
public:
    /// The type of a counting number
    using size_type = std::size_t;

    /// The type of a list of atomic numbers
    using Z_list = std::vector<size_type>;

    /// The type of a list of isotopes
    using isotope_list = std::vector<size_type>;

    /**
     * @brief Constructs an empty PeriodicTable instance
     *
     * Elements must be added through the provided insertion methods.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *                       PIMPL. Strong throw guarantee.
     */
    PeriodicTable();

    /**
     * @name Copy/Move CTors and Assignment Operators
     * @brief Functions for replicating the state of another PeriodicTable
     *        instance.
     *
     * Copy operations are deep copies and do not alias.
     *
     * @param[in] rhs The instance to copy/move from. If moved, @p rhs will be
     *                in a valid, but otherwise undefined state after the move.
     *
     * @return Copy/Move assignment operators return the current instance, but
     *         with its new state.
     *
     * @throw std::bad_alloc Copy ctor/assignment operators throw if there is
     *                       insufficient memory to allocate the new state.
     *                       Strong throw guarantee.
     * @throw None All move functions are no throw guarantee.
     */
    ///@{
    PeriodicTable(const PeriodicTable& rhs);
    PeriodicTable(PeriodicTable&& rhs) noexcept;
    PeriodicTable& operator=(const PeriodicTable& rhs);
    PeriodicTable& operator=(PeriodicTable&& rhs) noexcept;
    ///@}

    /// Default dtor
    ~PeriodicTable() noexcept;

    /**
     * @name Insertion Methods
     *
     * Methods to insert new Atom instances into the PeriodicTable.
     */
    ///@{
    /**
     * @brief Add a new element to the periodic table with the abundance-
     *        weighted mass.
     *
     * @param[in] Z Atomic number
     * @param[in] atom Atom object
     *
     * @throw std::runtime_error if element already exists with the given
     *                           atomic number. Strong throw guarantee.
     * @throw ??? if std::map::emplace throws an exception. Strong throw
     *            guarantee.
     */
    void insert(size_type Z, const Atom& atom);

    /**
     * @brief Add an isotope for the given element
     *
     * @param[in] Z Atomic number of the element
     * @param[in] mass_number Mass number of the isotope
     * @param[in] isotope Atom object for the isotope
     *
     * @throw std::out_of_range if an element with the given atomic number
     *                          does not exist. Strong throw guarantee.
     * @throw std::runtime_error Isotope with the given mass number already
     *                           exists. Strong throw guarantee.
     * @throw ??? if std::map::emplace throws an exception. Strong throw
     *            guarantee.
     */
    void add_isotope(size_type Z, size_type mass_number, const Atom& isotope);
    ///@}

    /**
     * @brief Returns a list of available mass numbers for a particular atom
     *
     * @param[in] Z The atomic number of the atom, for which mass numbers are
     *              wanted. Should be in range [0, max_Z()).
     *
     * @return A list of available mass numbers for the requested atom. If no
     *         isotope data is available for the particular value of @p Z then
     *         the list is empty.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *                       list. Strong throw guarantee.
     * @throw std::out_of_range if @p Z is not in the range [0, max_Z()).
     *                          Strong throw guarantee.
     * @throw ??? If an exception occurs in std::vector::push_back. Strong
     *            throw guarantee.
     */
    isotope_list isotopes(size_type Z) const;

    /**
     * @brief Returns the maximum atomic number for which an Atom instance can
     *        be made.
     *
     * For the most part retrieving an Atom instance amounts to specifying the
     * mass and atomic symbol for the atom. Atom instances are added at runtime
     * through the insertion methods, so it is not guaranteed that an Atom for
     * each atomic number in the range [1, max_Z()) exists. It should also be
     * noted that some properties may not be available for all atomic numbers
     * in the range [1, max_Z()).
     *
     * @return The largest atomic number for which an Atom instance exists
     *
     * @throw None No throw guarantee.
     */
    size_type max_Z() const noexcept;

    /**
     * @brief Convenience function for converting an atomic
     *        symbol to an atomic number.
     *
     * The reverse (atomic number to atomic symbol) should be done by using the
     * name of the atom returned by `get_atom(Z)`.
     *
     * @param[in] sym The symbol to convert (case-insensitive)
     *
     * @return The atomic number of the requested atom
     *
     * @throw std::out_of_range if @p sym is not a recognized symbol. Strong
     *                          throw guarantee.
     */
    size_type sym_2_Z(const std::string& sym) const;

    /**
     * @name Atom Retrieval Functions
     * @brief Returns an Atom instance pre-loaded with the data for the
     *        specified atomic number.
     *
     * There are two masses one can reasonably use for an atom: the abundance-
     * weighted mass and the isotope mass. We provide two different functions
     * to distinguish between these use cases. `get_atom` returns the
     * abundance-weighted variant, whereas `get_isotope` returns an Atom
     * whose mass is for a particular isotope.
     *
     * @param[in] Z The atomic number of the atom to create. Should be in the
     *              range [1, max_Z()).
     * @param[in] sym The atomic symbol of the atom to create. Symbol is
     *                case-insensitive.
     * @param[in] mass_num The mass number of the isotope to create.
     *
     * @return The requested Atom
     *
     * @throw std::out_of_range if @p Z is not in the range [1, max_Z()), @p
     *                          sym is not a recognized atomic symbol, or no
     *                          isotope data is available. Strong throw
     *                          guarantee.
     */
    ///@{
    Atom get_atom(size_type Z) const;
    Atom get_atom(const std::string& sym) const {
        return get_atom(sym_2_Z(sym));
    }
    Atom get_isotope(size_type Z, size_type mass_num) const;
    Atom get_isotope(const std::string& sym, size_type mass_num) const {
        return get_isotope(sym_2_Z(sym), mass_num);
    }
    ///@}

    /**
     * @name Comparison Operators
     *
     * @param rhs PeriodicTable on the right-hand side of the operator
     * @return Truth of comparison operator
     */
    ///@{
    bool operator==(const PeriodicTable& rhs) const;
    bool operator!=(const PeriodicTable& rhs) const;
    ///@}

private:
    /**
     * @name PIMPL Interaction
     * @brief Methods used to interact with the PIMPL
     *
     * @return detail_::PeriodicTablePIMPL&
     */
    ///@{
    /**
     * @brief Returns the PIMPL instance, creating it if it does not exist
     *
     * @return Existing PIMPL or newly created PIMPL if one did not exist yet

     * @throw std::bad_alloc There was not enough memory to create the new
     *                       PIMPL. Strong throw guarantee.
     */
    detail_::PeriodicTablePIMPL& pimpl_();

    /**
     * @brief Returns the PIMPL instance, but throws if it does not exist
     *
     * @return Existing PIMPL
     *
     * @throw std::runtime_error A PIMPL does not exist. Strong throw
     *                           guarantee.
     */
    const detail_::PeriodicTablePIMPL& pimpl_() const;
    ///@}

    /// The instance actually implementing the class.
    std::unique_ptr<detail_::PeriodicTablePIMPL> m_pimpl_;
};

} // namespace chemist
