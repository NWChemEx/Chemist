#pragma once
#include "libchemist/ta_helpers/detail_/contraction_dummy_annotation.hpp"
#include <TiledArray/expressions/contraction_helpers.h>
#include <memory>

namespace libchemist::orbital_space {

/** @brief Models an orbital space defined in terms of another orbital space.
 *
 *  In practice most orbital spaces are defined as a transformation from another
 *  space. For example the MOs are usually defined as a linear combination of
 *  the AOs.
 *
 *  @tparam TransformType Type of the object holding the transformation
 *                        coefficients from FromSpace to this space.
 *  @tparam FromSpace The type of the object that the transformation is from.
 *                    Must be default constructable.
 *  @tparam BaseType  Type of the class from which this class is derived.
 */
template<typename TransformType, typename FromSpace, typename BaseType>
class DerivedSpace_ : public BaseType {
private:
    /// Type of an instance of this class
    using my_type = DerivedSpace_<TransformType, FromSpace, BaseType>;

public:
    /// Type of the overlap matrix
    using overlap_type = typename BaseType::overlap_type;

    /// Type of the transformation matrix
    using transform_type = TransformType;

    /// Type of the orbital space this space is a transformation of
    using from_space_type = FromSpace;

    /// Type of a pointer to the from space
    using from_space_ptr = std::shared_ptr<const FromSpace>;

    /** @brief Creates a new DerivedSpace_ which has no transformation
     *         or from space.
     */
    DerivedSpace_() = default;

    /** @brief Creates a new DerivedSpace_ with the specified state.
     *
     *  Of note, this ctor will create an instance which owns the "from space"
     *  associated with it. Other spaces can alias this space if desired, but
     *  use of this ctor guarantees that the "from space" will not change for
     *  the duration of this instance.
     *
     * @tparam Args The types of the arguments which will be forwarded to the
     *              the base class's ctor.
     *
     * @param[in] C The transformation coefficients that take you from the
     *              orbital space `from_space` to this instance's orbital space.
     *              Default value is a default constructed instance of type
     *              `TransformType`.
     *
     * @param[in] from_space The orbital space in terms of which this space is
     *                       defined.
     * @param[in] args The arguments which will be forwarded to the base class's
     *                 ctor.
     *
     * @throw ??? If moving `C`, moving `from_space`, or fowarding `args`
     *            throws. Same throw guarantee.
     */
    template<typename... Args>
    DerivedSpace_(transform_type C, from_space_type from_space, Args&&... args);

    /** @brief Creates a new DerivedSpace_ with the specified state.
     *
     *  This ctor is designed to be used when the "from space" instance has
     *  already been created and is in use by another "to space". For exmaple
     *  one would use this ctor to construct the virtual MO space using the AO
     *  space stored in the already constructed occupied MO space. Note that if
     *  the "from space" is modified elsewhere the instance aliased within this
     *  class will reflect those changes.
     *
     * @tparam Args The types of the inputs which will be forwarded to the base
     *              class's ctor.
     * @param[in] C The transformation from the orbital space `*pfrom_space` to
     *              the current orbital space.
     * @param[in] pfrom_space A shared pointer to the "from space". The object
     *                        pointed at is read-only.
     * @param[in] args The arguments which will be forwarded to the base class's
     *                 ctor.
     *
     * @throw ??? If moving `C` or forwarding `args` throws. Same throw
     *            guarantee.
     */
    template<typename... Args>
    DerivedSpace_(transform_type C, from_space_ptr pfrom_space, Args&&... args);

    /** @brief Returns the transformation coefficients to go from `from_space`
     *         to the current space.
     *
     *  Assuming the coefficients are stored in a standard matrix, the
     *  convention is that the basis set for the rows is `from_space` and the
     *  current orbital space is the basis set for the columns of the
     *  transformation matrix. For more complicated tensors this convention is
     *  generalized.
     *
     *  @return The transformation coefficients to go from `from_space` to this
     *          space in a read/write format.
     *
     */
    auto& C() { return m_C_; }

    /** @brief Returns the transformation coefficients to go from `from_space`
     *         to the current space.
     *
     *  Assuming the coefficients are stored in a standard matrix, the
     *  convention is that the basis set for the rows is `from_space` and the
     *  current orbital space is the basis set for the columns of the
     *  transformation matrix. For more complicated tensors this convention is
     *  generalized.
     *
     *  @return The transformation coefficients to go from `from_space` to this
     *          space in a read-only format.
     */
    const auto& C() const { return m_C_; }

    /** @brief Returns the space that this orbital space is defined in terms of.
     *
     *
     *  @return The orbital space that this space is defined in terms of in a
     *          read-only manner.
     */
    const auto& from_space() const { return *m_pbase_; }

    /** @brief Returns the shared_ptr holding the from space
     *
     *  @return The shared_ptr holding the from space.
     */
    auto from_space_data() const { return m_pbase_; }

    /** @brief Returns a read-only reference to the density of the space
     *
     *  Returns the density matrix of the DerivedSpace in the basis set
     *  of the space it is derived from. Computes the density on the first call
     *  and stores it for later calls.
     *
     *  density(mu,nu) = C(mu,i) * Cdagger(i,nu)
     *
     * @return A reference to the density matrix.
     */
    const auto& density() const {
        if (!m_rho_.has_value()) m_rho_ = std::move(compute_density_());
        return m_rho_.value();
    }

    overlap_type transform(const overlap_type& t, const std::vector<std::size_t>& modes) const override;

protected:
    /// Include the transformation and the from space in the hash
    virtual void hash_(sde::Hasher& h) const override;

    virtual overlap_type& S_() override;
    virtual const overlap_type& S_() const override;

private:
    /// Computes the overlap matrix using the base space's overlap
    overlap_type compute_overlap_() const;

    /// The transformation coefficients to this orbital space
    transform_type m_C_;

    /// The basis set that this space is defined in terms of
    from_space_ptr m_pbase_;

    /// The density matrix in the from space basis set
    mutable std::optional<transform_type> m_rho_;

    /// Computes the density matrix in the from space basis set
    transform_type compute_density_() const;
};

template<typename LHSTransformType, typename LHSFromSpace, typename LHSBaseType,
         typename RHSTransformType, typename RHSFromSpace, typename RHSBaseType>
bool operator==(
  const DerivedSpace_<LHSTransformType, LHSFromSpace, LHSBaseType>& lhs,
  const DerivedSpace_<RHSTransformType, RHSFromSpace, RHSBaseType>& rhs) {
    auto& casted_lhs = static_cast<const LHSBaseType&>(lhs);
    auto& casted_rhs = static_cast<const RHSBaseType&>(rhs);

    // TODO: Actually compare the tensors
    const auto lhash = sde::hash_objects(lhs.C());
    const auto rhash = sde::hash_objects(rhs.C());

    return std::tie(casted_lhs, lhash, lhs.from_space()) ==
           std::tie(casted_rhs, rhash, rhs.from_space());
}

template<typename LHSTransformType, typename LHSFromSpace, typename LHSBaseType,
         typename RHSTransformType, typename RHSFromSpace, typename RHSBaseType>
bool operator!=(
  const DerivedSpace_<LHSTransformType, LHSFromSpace, LHSBaseType>& lhs,
  const DerivedSpace_<RHSTransformType, RHSFromSpace, RHSBaseType>& rhs) {
    return !(lhs == rhs);
}

// --------------------------- Implementations ---------------------------------

#define DERIVED_SPACE DerivedSpace_<TransformType, FromSpace, BaseType>

template<typename TransformType, typename FromSpace, typename BaseType>
template<typename... Args>
DERIVED_SPACE::DerivedSpace_(transform_type C, from_space_type base,
                             Args&&... args) :
  DerivedSpace_(std::move(C),
                std::make_shared<from_space_type>(std::move(base)),
                std::forward<Args>(args)...) {}

template<typename TransformType, typename FromSpace, typename BaseType>
template<typename... Args>
DERIVED_SPACE::DerivedSpace_(transform_type C, from_space_ptr pbase,
                             Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_C_(std::move(C)), m_pbase_(pbase) {}

template<typename TransformType, typename FromSpace, typename BaseType>
typename DERIVED_SPACE::overlap_type& DERIVED_SPACE::S_() {
    if(!this->has_overlap()) this->set_overlap_(compute_overlap_());
    return BaseType::S_();
}

template<typename TransformType, typename FromSpace, typename BaseType>
const typename DERIVED_SPACE::overlap_type& DERIVED_SPACE::S_() const {
    if(!this->has_overlap()) this->set_overlap_(compute_overlap_());
    return BaseType::S_();
}

template<typename TransformType, typename FromSpace, typename BaseType>
void DERIVED_SPACE::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_C_, *m_pbase_);
}

template<typename TransformType, typename FromSpace, typename BaseType>
typename DERIVED_SPACE::overlap_type DERIVED_SPACE::compute_overlap_() const {
    const auto& S_ao   = from_space().S();
    const auto& C_ao2x = C();
    overlap_type S_deriv;

    using tile_type             = typename FromSpace::overlap_type::value_type;
    constexpr bool tile1_is_tot = TA::detail::is_tensor_of_tensor_v<tile_type>;

    using tile_type2 = typename std::decay_t<decltype(C_ao2x)>::value_type;
    constexpr bool tile2_is_tot = TA::detail::is_tensor_of_tensor_v<tile_type2>;

    static_assert(
      tile1_is_tot == tile2_is_tot,
      "C and S are assumed to either both be tensors or both be ToTs.");

    if constexpr(tile1_is_tot) {
        auto n_ind = S_ao.trange().rank() / 2;
        std::string lidx, ridx;
        for(std::size_t i = 0; i < n_ind; ++i) {
            if(i > 0) {
                lidx += ",";
                ridx += ",";
            }
            lidx += "i_" + std::to_string(i);
            ridx += "j_" + std::to_string(i);
        }
        std::string tot_idx = lidx + "," + ridx;

        auto ij_an = tot_idx + ";a,nu";
        auto ij_ab = tot_idx + ";a,b";
        auto ij_mn = tot_idx + ";mu,nu";
        auto i_ma  = lidx + ";mu,a";
        auto j_nb  = ridx + ";nu,b";

        typename FromSpace::overlap_type buffer;
        TA::expressions::einsum(buffer(ij_an), C_ao2x(i_ma), S_ao(ij_mn));
        TA::expressions::einsum(S_deriv(ij_ab), buffer(ij_an), C_ao2x(j_nb));
    } else {
        overlap_type buffer;
        buffer("i,nu") = C_ao2x("mu,i") * S_ao("mu,nu");
        S_deriv("i,j") = buffer("i,nu") * C_ao2x("nu,j");
    }
    return S_deriv;
}

template<typename TransformType, typename FromSpace, typename BaseType>
typename DERIVED_SPACE::transform_type DERIVED_SPACE::compute_density_() const {
    const auto& C_ao2x = C();
    transform_type rho;

    using tile_type             = typename transform_type::value_type;
    constexpr bool tile_is_tot = TA::detail::is_tensor_of_tensor_v<tile_type>;

    if constexpr(tile_is_tot) {
        throw std::runtime_error("Can't compute density for ToT case");
    } else {
        rho("mu,nu") = C_ao2x("mu,i") * C_ao2x("nu,i");
    }
    return rho;
}

template<typename TransformType, typename FromSpace, typename BaseType>
typename DERIVED_SPACE::overlap_type DERIVED_SPACE::transform(const overlap_type& t,
                               const std::vector<std::size_t>& modes) const {

    using tile_type             = typename transform_type::value_type;
    constexpr bool tile_is_tot = TA::detail::is_tensor_of_tensor_v<tile_type>;

    overlap_type rv(t);
    const auto& C_ao2x = C();

    if constexpr(tile_is_tot) {
        throw std::runtime_error("Can't use transform() for ToT case");
    } else {
        auto n_modes = rv.range().rank();
        for(const auto& i : modes) {
            auto[start, finish, change] =
               ta_helpers::detail_::contraction_dummy_annotations(rv.trange().rank(), i);
            rv(finish) = rv(start) * C_ao2x(change);
        }
    }
    return rv;
}

#undef DERIVED_SPACE

} // namespace libchemist::orbital_space
