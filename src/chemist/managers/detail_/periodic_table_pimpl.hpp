#pragma once
#include "chemist/managers/periodic_table.hpp"
#include "utilities/containers/case_insensitive_map.hpp"

namespace chemist::detail_ {

/**
 * @brief PIMPL to hide details for the PeriodicTable class
 *
 * Atoms should be added to the PeriodicTable through the API defined by the
 * PeriodicTable class. That API redirects calls to this struct.
 */
struct PeriodicTablePIMPL {
    /// @name Forward typedefs from PeriodicTable
    ///@{
    using size_type    = typename PeriodicTable::size_type;
    using Z_list       = typename PeriodicTable::Z_list;
    using isotope_list = typename PeriodicTable::isotope_list;
    ///@}

    /// Map of atomic numbers to Atom objects
    using atom_map = std::map<size_type, Atom>;

    /// Mapping atomic numbers to a map of mass numbers to Atom objects
    using isotope_map = std::map<size_type, atom_map>;

    /// Symbol to atomic number map
    using sym_map = utilities::CaseInsensitiveMap<size_type>;

    /**
     * @name PeriodicTablePIMPL Public API
     *
     * @brief Functions comprising the public API of the PIMPL
     */
    ///@{

    /// Deep copy function
    std::unique_ptr<PeriodicTablePIMPL> clone() const {
        return std::make_unique<PeriodicTablePIMPL>(*this);
    }

    /**
     * @brief Add a new element to the periodic table with the abundance-
     *        weighted mass.
     *
     * @param[in] Z Atomic number
     * @param[in] atom Atom object
     *
     * @throws std::runtime_error If element already exists with the given
     *                            atomic number. Strong throw guarantee.
     * @throws ??? If std::map::emplace throws an exception. Strong throw
     *             guarantee.
     */
    void insert(size_type Z, const Atom& atom);

    /**
     * @brief Add an isotope for the given element
     *
     * @param[in] Z Atomic number of the element
     * @param[in] mass_number Mass number of the isotope
     * @param[in] isotope Atom object for the isotope
     *
     * @throws ??? If std::map::emplace throws an exception. Strong throw
     *             guarantee.
     * @throws std::out_of_range If an element with the given atomic number
     *                           does not exist. Strong throw guarantee.
     * @throws std::runtime_error Isotope with the given mass number already
     *                            exists. Strong throw guarantee.
     */
    void add_isotope(size_type Z, size_type mass_number, const Atom& isotope);

    /**
     * @brief Retrieves a list of mass numbers for isotopes of the element
     *
     * @param[in] Z Atomic number
     *
     * @return List of mass numbers for isotopes of the element specified by
     *         the given atomic number.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *                       list. Strong throw guarantee.
     * @throw std::out_of_range if element with given atomic number does not
     *                          exist. Strong throw guarantee.
     * @throw ??? If an exception occurs in std::vector::push_back. Strong
     *            throw guarantee.
     */
    isotope_list isotopes(size_type Z) const;

    /**
     * @brief Get the highest atomic number in this instance.
     *
     * @return Highest atomic number in this instance
     *
     * @throw None No throw guarantee.
     */
    size_type max_Z() const noexcept { return m_max_Z; }

    /**
     * @brief Converts a given atomic symbol to an atomic number
     *
     * @param[in] sym Atomic symbol (case insensitive)
     *
     * @return Atomic number
     *
     * @throws std::out_of_range Unrecognized atomic symbol. Strong throw
     *                           guarantee.
     */
    size_type sym_2_Z(const std::string& sym) const;

    /**
     * @brief Get an Atom of the element specified
     *
     * @param[in] Z Atomic number
     *
     * @return Atom of the specified element
     */
    Atom get_atom(size_type Z) const;

    /**
     * @brief Get an isotope
     *
     * @param[in] Z Atomic number of isotope
     * @param[in] mass_num Mass number of isotope
     *
     * @return Atom object for specified Z and mass number
     *
     * @throw std::out_of_range if Atom with atomic number (Z) or mass number
     *                          is not present. Strong throw guarantee.
     */
    Atom get_isotope(size_type Z, size_type mass_num) const;
    ///@}

    /**
     * @name Comparison Operators
     *
     * @param[in] rhs PeriodicTablePIMPL on the right-hand side of the operator
     * @return Truth of comparison operator
     */
    ///@{
    bool operator==(const PeriodicTablePIMPL& rhs) const;
    bool operator!=(const PeriodicTablePIMPL& rhs) const;
    ///@}

    /**
     * @name PeriodicTable State
     *
     * @brief This section defines the state of the PeriodicTable class.
     */
    ///@{
    /// Map of atomic symbols to atomic numbers
    sym_map m_sym_2_Z;

    /// Map of atomic numbers to an abundance-weighted Atom
    atom_map m_atoms;

    /// Maps atomic numbers and mass numbers to an isotope Atom
    isotope_map m_isotopes;

    /// Highest atomic number (Z) of an Atom in this instance
    size_type m_max_Z;
    ///@}
};

inline void PeriodicTablePIMPL::insert(size_type Z, const Atom& atom) {
    if(m_atoms.count(Z))
        throw std::runtime_error("Element already exists with Z = " +
                                 std::to_string(Z));

    m_atoms.emplace(Z, std::move(atom));
    m_isotopes.emplace(Z, atom_map());

    // Update max Z and symbol to Z mappings
    if(Z > m_max_Z) m_max_Z = Z;
    m_sym_2_Z.emplace(atom.name(), Z);
}

inline void PeriodicTablePIMPL::add_isotope(size_type Z, size_type mass_number,
                                            const Atom& isotope) {
    // Check for valid element
    if(!m_isotopes.count(Z))
        throw std::runtime_error("Element does not exist with Z = " +
                                 std::to_string(Z));

    // Check if isotope already exists
    if(m_isotopes.at(Z).count(mass_number))
        throw std::runtime_error("Isotope Z = " + std::to_string(Z) +
                                 ", A = " + std::to_string(mass_number) +
                                 " already exists");

    m_isotopes.at(Z).emplace(mass_number, std::move(isotope));
}

inline typename PeriodicTablePIMPL::isotope_list PeriodicTablePIMPL::isotopes(
  size_type Z) const {
    if(!m_isotopes.count(Z))
        throw std::out_of_range("Element does not exist with Z = " +
                                std::to_string(Z));

    isotope_list list;

    for(const auto& [mass_number, atom] : m_isotopes.at(Z)) {
        list.push_back(mass_number);
    }

    return list;
}

inline typename PeriodicTablePIMPL::size_type PeriodicTablePIMPL::sym_2_Z(
  const std::string& sym) const {
    if(!m_sym_2_Z.count(sym))
        throw std::out_of_range("Unrecognized atomic symbol: " + sym);

    return m_sym_2_Z.at(sym);
}

inline Atom PeriodicTablePIMPL::get_atom(size_type Z) const {
    if(!m_atoms.count(Z))
        throw std::out_of_range("Element does not exist with Z = " +
                                std::to_string(Z));

    return m_atoms.at(Z);
}

inline Atom PeriodicTablePIMPL::get_isotope(size_type Z,
                                            size_type mass_num) const {
    if(!m_isotopes.count(Z))
        throw std::out_of_range("Element does not exist with Z = " +
                                std::to_string(Z));

    if(!m_isotopes.at(Z).count(mass_num))
        throw std::out_of_range("No isotope data for Z = " + std::to_string(Z) +
                                ", A = " + std::to_string(mass_num));

    return m_isotopes.at(Z).at(mass_num);
}

inline bool PeriodicTablePIMPL::operator==(
  const PeriodicTablePIMPL& rhs) const {
    return m_sym_2_Z == rhs.m_sym_2_Z && m_atoms == rhs.m_atoms &&
           m_isotopes == rhs.m_isotopes;
}

inline bool PeriodicTablePIMPL::operator!=(
  const PeriodicTablePIMPL& rhs) const {
    return !(*this == rhs);
}

} // namespace chemist::detail_
