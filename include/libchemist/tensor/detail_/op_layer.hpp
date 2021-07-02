#pragma once

namespace libchemist::tensor::detail_ {

/** @brief Base class for operations in the operation layer.
 *
 *  This class basically exists so that we don't have to do a bunch of SFINAE to
 *  only enable fully templated versions of `operator+`, `operator-`, etc. for
 *  the operations in this component of libchemist, i.e., without this class we
 *  have:
 *
 *  @code
 *  template<typename LHS, typename RHS>
 *  auto operator+(LHS&& lhs, RHS&& rhs) {}
 *  @endcode
 *
 *  which requires SFINAE so tha t it's only enabled for choices of LHS and RHS
 *  like `MultOp<L, R>`. With this class we can do:
 *
 *  @code
 *  template<typename LHS, typename RHS>
 *  auto operator+(const OpLayer<LHS>& lhs, const OpLayer<RHS>& rhs){}
 *  @endcode
 *
 *  @tparam DerivedType The type of the class we're adding to the operations
 *                      layer. @p DerivedType is assumed to derive from
 *                      `OpLayer<DerivedType>`.
 */
template<typename DerivedType>
class OpLayer {
public:
    /** @brief Downcasts from the base class to the derived class.
     *
     *  This is used in the various implementations of `operator+` and the like
     *  to get the derived class instance back.
     *
     *  @return The current instance downcasted to a read-only reference of the
     *          derived class.
     *
     *  @throw None No throw guarantee.
     */
    const DerivedType& downcast() const noexcept;

    /** @brief Downcasts from the base class to the derived class.
     *
     *  This is used in the various implementations of `operator+` and the like
     *  to get the derived class instance back.
     *
     *  @return The current instance downcasted to a read/write reference of the
     *          derived class.
     *
     *  @throw None No throw guarantee.
     */
    DerivedType& downcast() noexcept;

protected:
    /** @brief  Defaulted default ctor
     *
     *  OpLayer has no state so this ctor is a no-op. The ctor is protected to
     *  avoid users instantiating instances of OpLayer. OpLayer instances are
     *  always created as part of a polymorphic object.
     *
     *  @throw None No throw gurantee.
     */
    OpLayer() = default;

    /** @brief Defaulted copy ctor.
     *
     *  OpLayer has no state so this ctor is a no-op. The ctor is protected to
     *  avoid users instantiating instances of OpLayer. OpLayer instances are
     *  always created as part of a polymorphic object.
     *
     *  @param[in] other The OpLayer object being copied.
     *
     *  @throw None No throw gurantee.
     */
    OpLayer(const OpLayer& other) = default;

    /** @brief Defaulted move ctor.
     *
     *  OpLayer has no state so this ctor is a no-op. The ctor is protected to
     *  avoid users instantiating instances of OpLayer. OpLayer instances are
     *  always created as part of a polymorphic object.
     *
     *  @param[in,out] rhs The OpLayer instance being moved from. Since OpLayer
     *                     has no state, the OpLayer part of @p rhs is in the
     *                     same state after the move.
     *
     *  @throw None No throw gurantee.
     */
    OpLayer(OpLayer&& rhs) = default;

    /** @brief Defaulted copy assignment.
     *
     *  OpLayer has no state so this ctor is a no-op. The ctor is protected to
     *  avoid users instantiating instances of OpLayer. OpLayer instances are
     *  always created as part of a polymorphic object.
     *
     *  @param[in] other The OpLayer object being copied.
     *
     *  @return The current instance after assigning a copy of @p other to it.
     *
     *  @throw None No throw gurantee.
     */
    OpLayer& operator=(const OpLayer& rhs) = default;

    /** @brief Defaulted move assignment.
     *
     *  OpLayer has no state so this ctor is a no-op. The ctor is protected to
     *  avoid users instantiating instances of OpLayer. OpLayer instances are
     *  always created as part of a polymorphic object.
     *
     *  @param[in,out] rhs The OpLayer instance being moved from. Since OpLayer
     *                     has no state, the OpLayer part of @p rhs is in the
     *                     same state after the move.
     *
     *  @return A reference to the current instance, after assigning @p rhs to
     *          it.
     *
     *  @throw None No throw gurantee.
     */
    OpLayer& operator=(OpLayer&& rhs) = default;
}; // class OpLayer<DerivedType>

// ----------------------------- Implementations -------------------------------

template<typename DerivedType>
const DerivedType& OpLayer<DerivedType>::downcast() const noexcept {
    return static_cast<const DerivedType&>(*this);
}

template<typename DerivedType>
DerivedType& OpLayer<DerivedType>::downcast() noexcept {
    return static_cast<DerivedType&>(*this);
}

} // namespace libchemist::tensor::detail_
