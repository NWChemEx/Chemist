#pragma once
#include "libchemist/hamiltonian/operator.hpp"
#include "libchemist/hamiltonian/type_traits.hpp"
#include <memory>

namespace libchemist {
namespace detail_ {
class HamiltonianPIMPL;
} // namespace detail_


/** @brief A class to store an manipulate molecular Hamiltonians.
 *
 *  This class provides a set of public APIs for the storage and
 *  manipulations of molecular Hamiltonians representing some 
 *  physical state of affairs. Hamiltonian instances are comprised
 *  of a collection of Operators which represent physical interactions
 *  between the particles of the system.
 *
 *  Example usage (Born-Oppenheimer Hamiltonian):
 *      Hamilton bo_hamiltonian( ElectronKinetic{}, ElectronNuclearCoulomb{} );
 *      bo_hamiltonian.add_term( ElectronElectronCoulomb{} );
 *      bo_hamiltonian.add_term( NuclearNuclearCoulomb{} );
 */
class Hamiltonian {

public:

    /** @brief Creates a new Hamiltonian instance containing no operators.
     *
     *  This ctor can be used to create a new Hamiltonian instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw gurantee.
     *
     *  Complexity: Constant
     */
    Hamiltonian() noexcept;

    /** @brief Creates a new Hamiltonian instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new Hamiltonian instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other Hamiltonian instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Hamiltonian instance. Strong exception gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    Hamiltonian( const Hamiltonian& other );

    /** @brief Creates a new Hamiltonian instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new Hamiltonian instance by taking ownership of
     *  @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to the
     *                       resulting instance. After the operation @p other will
     *                       not contain a PIMPL and will thus not be usable until
     *                       another PIMPL-containing Hamiltonian instance is
     *                       assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Hamiltonian( Hamiltonian&& other ) noexcept;

    /// Defaulted no-throw dtor
    ~Hamiltonian() noexcept;

    
    /** @brief Deep copy the state of another Hamiltonian instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be released.
     *
     *  @param[in] other The Hamiltonian instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep copy 
     *          of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Hamiltonian instance @p other. Strong exception 
     *                        gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    Hamiltonian& operator=( const Hamiltonian& other );

    /** @brief Transfer the ownership of the state of another Hamiltonian
     *         instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will 
     *  be released.
     *
     *  @param[in,out] other The Hamiltonian instance whose state is to be 
     *                       taken. @p other will no longer contain a valid
     *                       PIMPL and will need to be reassigned prior to
     *                       future usage.
     *
     *  @return The current instance after taking ownership of @p other's state.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Hamiltonian& operator=( Hamiltonian&& other ) noexcept;

    /** @brief Creates a new Hamiltonian instance from a collection of 
     *  Operator instances.
     *
     *  This ctor can be used to create a Hamiltonian instance from
     *  a collection of Operator instances. Additional terms may be
     *  added by invoking `add_term` or `add_terms`.
     *
     *  @tparam Args Parameter pack of passed Operator types, must all
     *               be derived from Operator.
     *  
     *  @param[in] args Operator instances from which to construct the Hamiltonian.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create the
     *                        PIMPL or to store the internal state of any Operator
     *                        instance. Basic exception gurantee.
     *
     *  Complexity: Linear in the size of @p args
     */
    template <typename... Args,
      typename = std::enable_if_t< detail_::all_are_operator_v<Args...> > 
    >
    Hamiltonian( Args&&... args ) : Hamiltonian() {
        add_terms( std::forward<Args>(args)... );
    }



    /** @brief Add an additonal operator to the Hamiltonian.
     *
     *  This function adds an operator to this Hamiltonian. If the Hamiltonian
     *  is in a PIMPL-less state, this function will generate a PIMPL instance. 
     *
     *  @tparam OpType The strong type of the operator to add to the 
     *                 Hamiltonian. Must be derived from Operator.
     *
     *  @param[in] op The operator to add to this Hamiltonian.
     *
     *  @return A reference to the current Hamiltonian instance.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create the
     *                        PIMPL or to store the internal state of the Operator
     *                        instance. Basic exception gurantee.
     *
     *  Complexity: Constant
     */
    template <typename OpType>
    detail_::enable_if_operator_t< OpType, Hamiltonian& > 
      add_term( const OpType& op ) {
        add_term_( typeid(OpType), std::make_shared<OpType>(op) );
        return *this;
    }

    /** @brief Add several additonal operators to the Hamiltonian.
     *
     *  This function adds several operators to this Hamiltonian. If the Hamiltonian
     *  is in a PIMPL-less state, this function will generate a PIMPL instance. 
     *
     *  @tparam Ops The strong types of the operators to add to the 
     *              Hamiltonian. All must be derived from Operator.
     *
     *  @param[in] ops The operators to add to this Hamiltonian.
     *
     *  @return A reference to the current Hamiltonian instance.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create the
     *                        PIMPL or to store the internal state of any Operator
     *                        instance. Basic exception gurantee.
     *
     *  Complexity: Linear in the size of @p ops.
     */
    template <typename... Ops>
    std::enable_if_t< detail_::all_are_operator_v<Ops...>, Hamiltonian& >
      add_terms( Ops&&... ops ) {
        (add_term( std::forward<Ops>(ops) ),...);
        return *this;
    }

    template <typename OpType>
    using get_return_type = typename std::vector<std::shared_ptr<OpType>>;


    /** @brief Returns the collection of operator terms matching a specific 
     *         operator type contained in this Hamiltonian.
     *
     *  Obtains the unique collection of operator terms contained in the
     *  current Hamiltonian instance which match a specific Operator strong
     *  type. If the Operator is not represented in this Hamiltonian or if
     *  the Hamiltonian is in a PIMPL-less state, an empty collection is 
     *  returned.
     *
     *  @tparam OpType The Operator type from which to query the Hamiltonian
     *
     *  @returns A vector of shared_ptr<OpType> which contains all of the
     *           operators in this Hamiltonian that match OpType.
     *
     *  @throw std::bad_alloc if there is insufficient memroy to allocate
     *                        the return value
     *
     *  Complexity: Linear in the number of operators contained in this
     *              Hamiltonian.
     */
    template <typename OpType>
    detail_::enable_if_operator_t< OpType, get_return_type<OpType> > 
      get_terms() const {
        auto type_erased_terms = get_terms_( typeid(OpType) );
        get_return_type<OpType> ret_terms( type_erased_terms.size() );
        std::transform( type_erased_terms.begin(), type_erased_terms.end(),
          ret_terms.begin(), std::dynamic_pointer_cast<OpType,Operator> );
        return ret_terms;
    }

    /** @brief Return whether an Operator type is represented in this Hamiltonian.
     *  
     *  Return a boolean which indicates whether or not a particlar Operator type
     *  is represented in this Hamiltonian. If this Hamiltonian is in a PIMPL-less
     *  state, `false` is returned.
     *
     *  @tparam OpType The Operator type from which to query the Hamiltonian
     *
     *  @returns `true` if `OpType` is represented in this Hamiltonian, `false`
     *           otherwise.
     *
     *  @throw None No throw gurantee
     *
     *  Complexity: Linear in the number of operators contained in this
     *              Hamiltonian.
     */
    template <typename OpType>
    detail_::enable_if_operator_t< OpType, bool > has_term() const noexcept {
        return has_term_( typeid(OpType) );
    }
private:

    /// Type erased private API for `add_term` which delegates to 
    /// HamiltonanPIMPL::add_term
    void add_term_( std::type_index index, std::shared_ptr<Operator>&& );

    /// Type erased private API for `get_terms` which delegates to 
    /// HamiltonanPIMPL::get_terms
    get_return_type<Operator> get_terms_( std::type_index index ) const;

    /// Type erased private API for `has_term` which delegates to 
    /// HamiltonanPIMPL::has_term
    bool has_term_( std::type_index index ) const noexcept;

    /// The instance actually implementing the API
    std::unique_ptr<detail_::HamiltonianPIMPL> pimpl_;

};


} // namespace libchemist
