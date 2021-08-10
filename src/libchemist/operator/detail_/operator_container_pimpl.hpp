#pragma once
#include "libchemist/operator/detail_/operator_container.hpp"
#include <map>

namespace libchemist::operators::detail_ {

/// Implementation of the public OperatorContainer API
class OperatorContainerPIMPL {
public:
    using type_erased_pointer = typename OperatorContainer::type_erased_pointer;
    using rtti_type           = typename OperatorContainer::rtti_type;

    using get_return_type = typename OperatorContainer::type_erased_vector;

    using get_const_return_type =
      typename OperatorContainer::const_type_erased_vector;

    /// Defaulted default ctor
    OperatorContainerPIMPL();
    /// Copy ctor
    OperatorContainerPIMPL(const OperatorContainerPIMPL& other);
    /// Move ctor
    OperatorContainerPIMPL(OperatorContainerPIMPL&& other) noexcept;
    /// Defaulted noexcept dtor
    ~OperatorContainerPIMPL() noexcept;

    /// Copy assignment
    OperatorContainerPIMPL& operator=(const OperatorContainerPIMPL& other);
    /// Move assignment
    OperatorContainerPIMPL& operator=(OperatorContainerPIMPL&& other) noexcept;

    /** @brief Add a term to the OperatorContainer state
     *
     *  Implements the type-erased backend for `OperatorContainer::add_term`
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
     *  Implements the type-erased backend for `OperatorContainer::get_terms`
     *
     *  @param[in] index RTTI specifier of the Operator type from which to
     * select the stored operator instances of the OperatorContainer
     *
     *  @return A `vector` of Operators instances which match the passed RTTI
     * specifier in @p index.
     */
    get_const_return_type get_terms(rtti_type index) const;

    get_return_type get_terms(rtti_type index);

    /** @brief Check whether this OperatorContainer contains a certain operator
     * type
     *
     *  Implements the type-erased backend for `OperatorContainer::has_term`
     *
     *  @param[in] index RTTI specifier of the Operator type to check.
     *
     *  @return `true` if this OperatorContainer contains an Operatoer with RTTI
     *          matching @p index, `false` otherwise.
     */
    bool has_term(rtti_type index) const noexcept;

    /** @brief Clone this OperatorContainer instance
     *
     *  Export a deep copy of this OperatorContainer instance for use with
     *  OperatorContainer copy ctor/assignment.
     *
     *  @return `shared_ptr` to the copy of this OperatorContainer
     */
    std::unique_ptr<OperatorContainerPIMPL> clone() const;

    bool operator==(const OperatorContainerPIMPL& other) const;
    bool operator!=(const OperatorContainerPIMPL& other) const;

    void hash(pluginplay::Hasher& h) const;

private:
    using operator_container_type =
      std::multimap<rtti_type, type_erased_pointer>;

    /// Internal storage of operator instances which comprise this
    /// OperatorContainer
    operator_container_type terms_;
};

} // namespace libchemist::operators::detail_
