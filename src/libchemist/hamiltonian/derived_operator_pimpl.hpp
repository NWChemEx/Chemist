#pragma once
#include "libchemist/hamiltonian/derived_operator.hpp"
#include <map>

namespace libchemist::detail_ {

/// Implementation of the public DerivedOperator API
class DerivedOperatorPIMPL {

    using operator_container_type = 
      std::multimap< std::type_index, std::shared_ptr<Operator> >;

public:

    /// Defaulted default ctor
    DerivedOperatorPIMPL();
    /// Copy ctor
    DerivedOperatorPIMPL( const DerivedOperatorPIMPL& other );
    /// Move ctor
    DerivedOperatorPIMPL( DerivedOperatorPIMPL&& other ) noexcept;
    /// Defaulted noexcept dtor
    ~DerivedOperatorPIMPL() noexcept;

    /// Copy assignment
    DerivedOperatorPIMPL& operator=( const DerivedOperatorPIMPL& other );
    /// Move assignment
    DerivedOperatorPIMPL& operator=( DerivedOperatorPIMPL&& other ) noexcept;


    /** @brief Add a term to the DerivedOperator state
     *
     *  Implements the type-erased backend for `DerivedOperator::add_term`
     *
     *  @param[in] index RTTI of the strong type associated with the Operator instance
     *  @param[in] op    `shared_ptr` containing the state of the Operator instance
     */
    void add_term( std::type_index index, std::shared_ptr<Operator>&& op ); 

    using get_return_type = DerivedOperator::get_return_type<Operator>;

    /** @brief Return the collection of operators with a particular RTTI specifier
     *
     *  Implements the type-erased backend for `DerivedOperator::get_terms`
     *
     *  @param[in] index RTTI specifier of the Operator type from which to select
     *                   the stored operator instances of the DerivedOperator
     *
     *  @return A `vector` of Operators instances which match the passed RTTI specifier 
     *          in @p index.
     */
    get_return_type get_terms( std::type_index index ) const; 

    /** @brief Check whether this DerivedOperator contains a certain operator type
     *
     *  Implements the type-erased backend for `DerivedOperator::has_term`
     *
     *  @param[in] index RTTI specifier of the Operator type to check.
     *
     *  @return `true` if this DerivedOperator contains an Operatoer with RTTI
     *          matching @p index, `false` otherwise.
     */
    bool has_term( std::type_index index ) const noexcept; 


    /** @brief Clone this DerivedOperator instance
     *
     *  Export a deep copy of this DerivedOperator instance for use with
     *  DerivedOperator copy ctor/assignment.
     *
     *  @return `shared_ptr` to the copy of this DerivedOperator
     */
    std::unique_ptr<DerivedOperatorPIMPL> clone() const;


    bool operator==( const DerivedOperatorPIMPL& other) const;
    bool operator!=( const DerivedOperatorPIMPL& other) const;

private:

    /// Internal storage of operator instances which comprise this DerivedOperator
    operator_container_type terms_;

};

}
