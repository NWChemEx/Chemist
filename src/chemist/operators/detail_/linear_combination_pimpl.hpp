#pragma once
#include "chemist/operators/detail_/linear_combination.hpp"
#include <map>

namespace chemist::operators::detail_ {

/// Implementation of the public LinearCombination API
class LinearCombinationPIMPL {
public:
    using type_erased_pointer = typename LinearCombination::type_erased_pointer;
    using rtti_type           = typename LinearCombination::rtti_type;

    using get_return_type = typename LinearCombination::type_erased_vector;

    using get_const_return_type =
      typename LinearCombination::const_type_erased_vector;

    using size_type = typename LinearCombination::size_type;

    /// Defaulted default ctor
    LinearCombinationPIMPL();
    /// Copy ctor
    LinearCombinationPIMPL(const LinearCombinationPIMPL& other);
    /// Move ctor
    LinearCombinationPIMPL(LinearCombinationPIMPL&& other) noexcept;
    /// Defaulted noexcept dtor
    ~LinearCombinationPIMPL() noexcept;

    /// Copy assignment
    LinearCombinationPIMPL& operator=(const LinearCombinationPIMPL& other);
    /// Move assignment
    LinearCombinationPIMPL& operator=(LinearCombinationPIMPL&& other) noexcept;

    size_type size() const noexcept { return terms_.size(); }

    /** @brief Add a term to the LinearCombination state
     *
     *  Implements the type-erased backend for `LinearCombination::add_term`
     *
     *  @param[in] index RTTI of the strong type associated with the Operator
     * instance
     *  @param[in] op    `shared_ptr` containing the state of the Operator
     * instance
     */
    void add_term(rtti_type index, type_erased_pointer op);

    /** @brief Return the collection of operators with a particular RTTI
     * specifier
     *
     *  Implements the type-erased backend for `LinearCombination::get_terms`
     *
     *  @param[in] index RTTI specifier of the Operator type from which to
     * select the stored operator instances of the LinearCombination
     *
     *  @return A `vector` of Operators instances which match the passed RTTI
     * specifier in @p index.
     */
    get_const_return_type get_terms(rtti_type index) const;

    get_return_type get_terms(rtti_type index);

    /** @brief Check whether this LinearCombination contains a certain operator
     * type
     *
     *  Implements the type-erased backend for `LinearCombination::has_term`
     *
     *  @param[in] index RTTI specifier of the Operator type to check.
     *
     *  @return `true` if this LinearCombination contains an Operatoer with RTTI
     *          matching @p index, `false` otherwise.
     */
    bool has_term(rtti_type index) const noexcept;

    /** @brief Clone this LinearCombination instance
     *
     *  Export a deep copy of this LinearCombination instance for use with
     *  LinearCombination copy ctor/assignment.
     *
     *  @return `shared_ptr` to the copy of this LinearCombination
     */
    std::unique_ptr<LinearCombinationPIMPL> clone() const;

    bool operator==(const LinearCombinationPIMPL& other) const;
    bool operator!=(const LinearCombinationPIMPL& other) const;

    void hash(chemist::detail_::Hasher& h) const;

private:
    using operator_container_type =
      std::multimap<rtti_type, type_erased_pointer>;

    /// Internal storage of operator instances which comprise this
    /// LinearCombination
    operator_container_type terms_;
};

} // namespace chemist::operators::detail_
