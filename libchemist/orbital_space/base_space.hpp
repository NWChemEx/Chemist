#pragma once
#include "libchemist/types.hpp"
#include <sde/detail_/memoization.hpp>

namespace libchemist::orbital_space {

/** @brief Models the most fundamental properties of an orbital space
 *
 *  The BaseSpace_ class contains the properties all orbital spaces have in
 *  common. For the moment this amounts to orthonormality. The orthonormality
 *  information is stored by means of the overlap matrix.
 *
 *  @note This class's name ends in an underscore because we intend for users to
 *        declare instances of it with typdefs. 
 *
 *  @tparam OverlapType The type of the tensor holding the overlap matrix.
 */
template<typename OverlapType>
class BaseSpace_ {
public:
    /// Type of the tensor holding the overlap matrix
    using overlap_type = OverlapType;

    /** @brief Creates a base space with the specified overlap matrix
     *
     *  @param[in] s the value of the overlap matrix. Default value is a default
     *               initialized instance of type overlap_type.
     *
     *  @throw ??? if moving `s` throws. Same exception guarantee as
     *             overlap_type's move ctor.
     */
    BaseSpace_(overlap_type s = {}) : m_S_(std::move(s)) {}

    /** @brief The overlap matrix in read/write format
     *
     *
     *  @return The contained overlap matrix in read/write format
     */
    auto& S() { return m_S_; }

    /** @brief The overlap matrix in read-only format
     *
     * @return The contained overlap matrix in read-only format
     */
    const auto& S() const { return m_S_; }

    /** @brief Hashes the current instance.
     *
     *  @param[in,out] h The hasher instance to use for hashing. The internal
     *                   state of h will be modified so that its internal hash
     *                   includes state information about this BaseSpace_
     *                   instance.
     */
    virtual void hash(sde::Hasher& h) const { hash_(h); }
protected:
    /// Actually implements hash. Should be overridden by derived classes
    virtual void hash_(sde::Hasher& h) const { h(m_S_); }
private:
    /// The actual instance of the overlap matrix
    overlap_type m_S_;
};

template<typename OverlapType>
bool operator==(const BaseSpace_<OverlapType>& lhs,
                const BaseSpace_<OverlapType>& rhs) {
    // TODO: Actually compare the tensors
    return sde::hash_objects(lhs) == sde::hash_objects(rhs);
}

template<typename OverlapType>
bool operator!=(const BaseSpace_<OverlapType>& lhs,
                const BaseSpace_<OverlapType>& rhs) {
    return !(lhs == rhs);
}

/// BaseSpace that uses a normal tensor for the overlap matrix
template<typename T> using BaseSpace  = BaseSpace_<type::tensor<T>>;

/// BaseSpace that uses a tensor-of-tensors for the overlap matrix
template<typename T> using SparseBase = BaseSpace_<type::tensor_of_tensors<T>>;

} // namespace libchemist::orbital_space
