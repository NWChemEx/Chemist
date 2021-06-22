#pragma once
#include "libchemist/hamiltonian/hamiltonian.hpp"
#include <map>

namespace libchemist::detail_ {

/// Implementation of the public Hamiltonian API
class HamiltonianPIMPL {

    using operator_container_type = 
      std::multimap< std::type_index, std::shared_ptr<Operator> >;

public:

    /// Defaulted default ctor
    HamiltonianPIMPL();
    /// Copy ctor
    HamiltonianPIMPL( const HamiltonianPIMPL& other );
    /// Move ctor
    HamiltonianPIMPL( HamiltonianPIMPL&& other ) noexcept;
    /// Defaulted noexcept dtor
    ~HamiltonianPIMPL() noexcept;

    /// Copy assignment
    HamiltonianPIMPL& operator=( const HamiltonianPIMPL& other );
    /// Move assignment
    HamiltonianPIMPL& operator=( HamiltonianPIMPL&& other ) noexcept;


    /** @brief Add a term to the Hamiltonian state
     *
     *  Implements the type-erased backend for `Hamiltonian::add_term`
     *
     *  @param[in] index RTTI of the strong type associated with the Operator instance
     *  @param[in] op    `shared_ptr` containing the state of the Operator instance
     */
    void add_term( std::type_index index, std::shared_ptr<Operator>&& op ); 

    using get_return_type = Hamiltonian::get_return_type<Operator>;

    /** @brief Return the collection of operators with a particular RTTI specifier
     *
     *  Implements the type-erased backend for `Hamiltonian::get_terms`
     *
     *  @param[in] index RTTI specifier of the Operator type from which to select
     *                   the stored operator instances of the Hamiltonian
     *
     *  @return A `vector` of Operators instances which match the passed RTTI specifier 
     *          in @p index.
     */
    get_return_type get_terms( std::type_index index ) const; 

    /** @brief Check whether this Hamiltonian contains a certain operator type
     *
     *  Implements the type-erased backend for `Hamiltonian::has_term`
     *
     *  @param[in] index RTTI specifier of the Operator type to check.
     *
     *  @return `true` if this Hamiltonian contains an Operatoer with RTTI
     *          matching @p index, `false` otherwise.
     */
    bool has_term( std::type_index index ) const noexcept; 


    /** @brief Clone this Hamiltonian instance
     *
     *  Export a deep copy of this Hamiltonian instance for use with
     *  Hamiltonian copy ctor/assignment.
     *
     *  @return `shared_ptr` to the copy of this Hamiltonian
     */
    std::unique_ptr<HamiltonianPIMPL> clone() const;

private:

    /// Internal storage of operator instances which comprise this Hamiltonian
    operator_container_type terms_;

};

}
