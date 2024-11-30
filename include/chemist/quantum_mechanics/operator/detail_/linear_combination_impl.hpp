/*
 * Copyright 2024 NWChemEx-Project
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
#include <chemist/quantum_mechanics/operator/detail_/unpack_linear_combination.hpp>
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
namespace chemist::qm_operator::detail_ {

/** @brief Implements operators which are linear combinations of operators.
 *
 *  Several of the operators we care about such as the Hamiltonian, the core
 *  Hamiltonian, and the Fock operator are linear combinations of operators.
 *  This class factors out the code necessary to implement these operators.
 *
 *  @tparam DerivedType The operator *this is implementing.
 */
template<typename DerivedType>
class LinearCombinationImpl : public OperatorBase {
private:
    /// Type of *this
    using my_type = LinearCombinationImpl<DerivedType>;

protected:
    /// Type of an object that can unpack a linear combination of operators
    using parser_type = detail_::UnpackLinearCombination;

    /// Type parser_type unpacks into
    using container_type = typename parser_type::container_type;

public:
    /// Bring in base class types
    ///@{
    using base_type = OperatorBase;
    using typename base_type::base_pointer;
    using typename base_type::const_base_reference;
    using typename base_type::visitor_reference;
    ///@}

    /// How the container stores the coefficient/operator pairs
    using value_type = typename container_type::value_type;

    /// Type used for indexing and offsets
    using size_type = typename container_type::size_type;

    /// Type of the scalars
    using coefficient_type = typename value_type::first_type;

    /// Type of a read-only reference to an object of type coefficient_type
    using const_coefficient_reference = coefficient_type;

    /** @brief Returns the number of terms in *this.
     *
     *  @return The number of terms in this linear combination.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_terms_.size(); }

    /** @brief Returns the @p i-th coefficient of the linear combination.
     *
     *  @param i The offset of the requested coefficient. Must be in the range
     *           [0, size()).
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()).
     *                           Strong throw guarantee.
     */
    const_coefficient_reference coefficient(size_type i) const {
        return m_terms_.at(i).first;
    }

    /** @brief Returns the @p i-th operator in the linear combination.
     *
     *  @param i The offset of the requested operator. Must be in the range
     *           [0, size()).
     *
     *  @throw std::out_of_range if @p i is not in the range [0, size()).
     *                           Strong throw guarantee.
     */
    const_base_reference get_operator(size_type i) const {
        return *(m_terms_.at(i).second);
    }

    /** @brief Adds a term to the operator.
     *
     *  This method is used to add an operator to *this. The term is assumed
     *  to be added to the terms already contained in *this (i.e., make @p c
     *  negative if you want the term be subtracted).
     *
     *  @param[in] c  The weight of the term.
     *  @param[in] op The term.
     *
     *  @throw std::bad_alloc if there is a problem adding the term. Strong
     *                        throw guarantee.
     */
    void emplace_back(coefficient_type c, base_pointer op) {
        m_terms_.emplace_back(std::make_pair(c, std::move(op)));
    }

    /** @brief Is *this the same as @p rhs?
     *
     *  Two linear combinations are value equal if they:
     *
     *  - contain the same number of terms,
     *  - the coefficient of the i-th term is the same in each for all i, and
     *  - the operator of the i-th term is the same in each for all i.
     *
     *  @param[in] rhs The linear combination to compare to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const LinearCombinationImpl& rhs) const noexcept {
        if(size() != rhs.size()) return false;
        for(size_type i = 0; i < size(); ++i) {
            if(coefficient(i) != rhs.coefficient(i)) return false;
            if(get_operator(i).are_different(rhs.get_operator(i))) return false;
        }
        return true;
    }

    /** @brief Is *this different than @p rhs?
     *
     *  This class defines different as not value equal. See operator== for the
     *  definition of value equal.
     *
     *  @param[in] rhs The linear combination to compare to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const LinearCombinationImpl& rhs) const noexcept {
        return !((*this) == rhs);
    }

    void swap(LinearCombinationImpl& other) noexcept {
        m_terms_.swap(other.m_terms_);
    }

protected:
    /** @brief Initialize *this with an already unpacked abstract syntax tree.
     *
     *  @param[in] terms Pairs of coefficients and operators making up the
     *                   linear combination.
     *
     *  @throw None No throw guarantee.
     */
    explicit LinearCombinationImpl(container_type terms = {}) :
      m_terms_(std::move(terms)) {}

    /** @brief Initializes *this with a deep copy of @p other.
     *
     *  @param[in] other The linear combination to copy.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     */
    LinearCombinationImpl(const LinearCombinationImpl& other) {
        for(const auto& [c, op] : other.m_terms_)
            m_terms_.emplace_back(std::make_pair(c, op->clone()));
    }

    /** @brief Overwrites *this with a deep copy of @p rhs.
     *
     *  This method will release the state currently in *this and replace it
     *  with a deep copy of the state in @p rhs.
     *
     *  @param[in] rhs The object to copy.
     *
     *  @return *this after overwriting its state.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     */
    LinearCombinationImpl& operator=(const LinearCombinationImpl& rhs) {
        if(this != &rhs) LinearCombinationImpl(rhs).swap(*this);
        return *this;
    }

    /// Implements clone by downcasting to the derived class and copying
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Loops over terms calling visit on each term
    void visit_(visitor_reference visitor) override {
        for(auto& [c, op] : m_terms_) op->visit(visitor);
    }

    /// Loops over terms calling visit on each term
    void visit_(visitor_reference visitor) const override {
        for(auto& [c, op] : m_terms_) op->visit(visitor);
    }

    /// Implements are_equal by calling operator== in derived class
    bool are_equal_(const_base_reference other) const noexcept override {
        auto pother = dynamic_cast<const my_type*>(&other);
        if(pother == nullptr) return false; // Different types
        return downcast_() == pother->downcast_();
    }

private:
    /// Wraps downcasting *this to DerivedType&
    decltype(auto) downcast_() { return static_cast<DerivedType&>(*this); }

    /// Wraps downcast *this to const DerivedType&
    decltype(auto) downcast_() const {
        return static_cast<const DerivedType&>(*this);
    }

    /// The terms making up the linear combination
    container_type m_terms_;
};

} // namespace chemist::qm_operator::detail_