/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "chemist/point_charge/point_charge.hpp"
#include <cereal/types/string.hpp>
#include <iomanip>
#include <ios>

namespace chemist {

/** @brief A class for holding the details of an atomic unit.
 *
 *  Atoms are considered the first tier of input to a computational chemistry
 *  program.  That is, most types of computational chemistry algorithms need to
 *  know where in space the atoms are located and what their chemical identity
 *  is.  This class also additionally holds the mass of the atom and a string
 *  identifier.
 */
class Atom : public PointCharge<double> {
private:
    using base_type = PointCharge<double>;

public:
    /// The type of a charge
    using charge_type = typename base_type::scalar_type;

    /// The type of a counting number (i.e. atomic number)
    using size_type = std::size_t;

    /// The type the mass is stored as
    using mass_type = double;

    /// The type of the atomic coordinates input
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
    Atom() = default;

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
    Atom(const Atom& rhs)     = default;
    Atom(Atom&& rhs) noexcept = default;
    Atom& operator=(const Atom& rhs) = default;
    Atom& operator=(Atom&& rhs) noexcept = default;
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
     * - `[std::array<double, 3>]` the input coordinates.
     * - `[std::string]` the name of the Atom.
     * - `[std::size_t]` the atomic number of the Atom.
     * - `[double]` the atomic mass of the Atom
     *
     * When providing input to the CTor, the order of the groups is irrelevant.
     * The charge of the atom is set to the atomic number by default. The charge
     * can be altered after construction if desired.
     *
     *
     * @param[in] coords The Cartesian coordinates (x,y,z).
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
    template<typename... Args>
    explicit Atom(const coord_type& coords_in, Args&&... args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_carts = only_one<coord_type, Args...>;
        static_assert(!is_carts, "Please only provide one set of coordinates");
        this->x() = coords_in[0];
        this->y() = coords_in[1];
        this->z() = coords_in[2];
    }

    template<typename... Args>
    explicit Atom(const name_type& name_in, Args&&... args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_name = only_one<name_type, Args...>;
        static_assert(!is_name, "Please only provide one name");
        name() = name_in;
    }

    template<typename... Args>
    explicit Atom(size_type Z_in, Args&&... args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_Z = only_one<size_type, Args...>;
        static_assert(!is_Z, "Please only provide one atomic number");
        Z()            = Z_in;
        this->charge() = (charge_type)Z_in;
    }

    template<typename... Args>
    explicit Atom(const mass_type& mass_in, Args&&... args) :
      Atom(std::forward<Args>(args)...) {
        constexpr bool is_mass = only_one<mass_type, Args...>;
        static_assert(!is_mass, "Please only provide one mass");
        mass() = mass_in;
    }
    ///@}

    /// Default dtor
    ~Atom() noexcept = default;

    /**
     * @defgroup Z/Name/Mass setter/getter
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
    name_type& name() noexcept { return m_name_; }
    const name_type& name() const noexcept { return m_name_; }

    size_type& Z() noexcept { return m_Z_; }
    const size_type& Z() const noexcept { return m_Z_; }

    mass_type& mass() noexcept { return m_mass_; }
    const mass_type& mass() const noexcept { return m_mass_; }
    ///@}

    /** @brief Serializes the atom.
     *
     *  @param[in,out] ar The archive instance being used for serialization.
     *                    After this call @p ar will contain this instance's
     *                    serialized state.
     */
    template<typename Archive>
    void save(Archive& ar) const;

    /** @brief Deserializes the atom.
     *
     *  @param[in,out] ar The archive instance which contains the serialized
     *                    state for this instance. After this call @p ar will
     *                    no longer contain this instance's serialized state.
     */
    template<typename Archive>
    void load(Archive& ar);

private:
    /// The atomic number of the atom
    size_type m_Z_ = 0;

    /// The name of the atom
    name_type m_name_ = "";

    /// The mass of the atom
    mass_type m_mass_ = 0.0;

    /// Private member variable for static asserts
    template<typename T, typename... Args>
    static constexpr bool only_one =
      std::disjunction_v<std::is_same<std::decay_t<Args>, T>...>;

}; // End Atom

/**
 * @defgroup Atom comparison operators
 * @relates Atom
 * @brief Allows one to compare two atom instances for exact equality.
 *
 * Two atom instances are defined as equal if they have the same atomic
 * number, the same mass, and the same coordinates.  The name field is
 * considered metadata and is not considered in the comparison.  *N.B* that
 * floating-point comparisons are bit-wise with zero tolerance for
 * deviation, *i.e.*, 1.99999999999999 != 2.00000000000000
 *
 * @param[in] lhs The Atom instance on the left of the equivalence operation
 * @param[in] rhs The Atom instance on the right of the equivalence
 * operation
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

/**
 * @relates Atom
 * @brief Makes it so the Atom class can be printed out.
 *
 * @param os The output stream to print to.
 * @param ai The Atom instance to print to the stream.
 * @return The output stream containing the atom instance.
 * @throws std::ios_base::failure if anything goes wrong while writing. Weak
 *         throw guarantee.
 */
std::ostream& operator<<(std::ostream& os, const Atom& ai);

// ----------------------- Implementations -------------------------------------

template<typename Archive>
void Atom::save(Archive& ar) const {
    base_type::save(ar);
    ar& m_Z_;
    ar& m_name_;
    ar& m_mass_;
}

template<typename Archive>
void Atom::load(Archive& ar) {
    base_type::load(ar);
    ar& m_Z_;
    ar& m_name_;
    ar& m_mass_;
}

} // namespace chemist
