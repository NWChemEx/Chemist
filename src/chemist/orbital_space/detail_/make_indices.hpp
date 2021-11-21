#include <utilities/strings/string_tools.hpp>

namespace chemist::orbital_space::detail_ {

// Transforming a tensor by a tensor
inline auto make_tensor_tensor_indices(const std::string& lhs,
                                       const std::string& rhs,
                                       std::size_t mode) {
    using namespace utilities::strings;

    // The letter used in RHS's dummy indices
    const std::string dummy_idx = "j";

    // Check mode is valid for rhs
    const auto rhs_rank = split_string(rhs, ",").size();
    if(rhs_rank <= mode) {
        throw std::out_of_range("Can not transform zero-based mode " +
                                std::to_string(mode) + " of a tensor with ;" +
                                std::to_string(rhs_rank) + "modes.");
    }

    // Check that lhs is a matrix
    auto parsed_lhs = split_string(lhs, ",");
    if(parsed_lhs.size() != 2)
        throw std::runtime_error("Expected transformation to be matrix");

    // Assume column vectors
    const std::string& new_idx  = parsed_lhs[1];
    const std::string& cont_idx = parsed_lhs[0];
    std::string idx2replace     = dummy_idx + std::to_string(mode);
    auto rhs_idx                = replace(idx2replace, cont_idx, rhs);
    auto result_idx             = replace(idx2replace, new_idx, rhs);
    return std::make_pair(result_idx, rhs_idx);
}

// Transform a ToT by a tensor
inline auto make_tensor_tot_indices(const std::string& lhs,
                                    const std::string& rhs, std::size_t mode) {
    using namespace utilities::strings;

    auto tot_idx = split_string(rhs, ";");
    auto ind_idx = split_string(tot_idx[0], ",");

    if(mode >= ind_idx.size())
        throw std::runtime_error("tensor*ToT must transform independent mode");

    return make_tensor_tensor_indices(lhs, rhs, mode);
}

/// Transforms a ToT by a ToT
inline auto make_tot_tot_indices(const std::string& lhs, const std::string& rhs,
                                 std::size_t mode) {
    // For C we can have:
    // - "i;m" (or "i,j;m" if there's more independent indices), or
    // - "i;m,a" (or "i,j;m,a" with more independent indices)
    // We assume that C's independent indices are a subset of t's and that we're
    // not contracting over the independent indices then:
    // - "i;m" * "i,j;m,n,l,s" becomes "i,j;n,l,s"
    // - "i,j;m" * "i,j;m,n,l,s" becomes "i,j;n,l,s"
    // - "i;m,a" * "i,j;m,n,l,s" becomes "i,j;a,n,l,s"

    using namespace utilities::strings;

    // The letter used in RHS's dummy indices
    const std::string rhs_dummy = "j";

    auto ltot_idx = split_string(lhs, ";");
    auto lind_idx = split_string(ltot_idx[0], ",");
    auto ldep_idx = split_string(ltot_idx[1], ",");
    auto rtot_idx = split_string(rhs, ";");
    auto rind_idx = split_string(rtot_idx[0], ",");
    auto rdep_idx = split_string(rtot_idx[1], ",");

    if(mode < rind_idx.size())
        throw std::runtime_error("ToT * ToT must transform dependent index.");

    if(ldep_idx.size() == 1)
        throw std::runtime_error("C transforming to independent space NYI");

    if(lind_idx.size() != rind_idx.size())
        throw std::runtime_error("Mixing independent ranks NYI");

    // The new rhs index is Hadamard-ed over lhs and rhs ind indices, and has
    // mode "mode" replaced with ldep_idx[0]. The former is accomplished by
    // making rhs's ind indices a copy of lhs's ind indices
    std::string new_rhs = join_string(lind_idx, ",");
    new_rhs += ";";
    std::string result(new_rhs);
    std::string idx2replace = rhs_dummy + std::to_string(mode);
    new_rhs += replace(idx2replace, ldep_idx[0], rtot_idx[1]);
    result += replace(idx2replace, ldep_idx[1], rtot_idx[1]);

    return std::make_pair(result, new_rhs);
}

/** @brief The main API for working out the indices for a transformation.
 *
 *  This function encapsulates all of the logic required to figure out how to
 *  transform a tensor to an orbital space. I relies on the kernels defined
 *  above depending on whether @p lhs and @p rhs are tensor-of-tensor or
 *  non-hierarchical tensors.
 *
 *  This function is envisioned as being used with the TensorWrapper class like:
 *
 *  @code
 *  // lhs is the transformation, rhs is the tensor, result is rhs transformed
 *  TensorWrapper lhs, rhs, result;
 *  auto lhs_idx = lhs.make_annotation();
 *  // The "j" part is hard-coded in the logic of make_indices at the moment
 *  auto rhs_idx = rhs.make_annotation("j");
 *  auto [result_idx, new_rhs_idx] = make_indices(lhs_idx, rhs_idx);
 *  result(result_idx) = lhs(lhs_idx) * rhs(new_rhs_idx);
 *  @endcode
 *
 *  @param[in] lhs The annotation on the transform.
 *  @param[in] rhs The initial annotation on the tensor to be transformed.
 *  @param[in] mode The zero-based index of the tensor to transform.
 *
 *  @return A pair whose first element is the annotation for the tensor to use
 *          when contracting it with the transformation; the second element is
 *          the annotation for the tensor after the contraction has been
 *          performed.
 */
inline auto make_indices(const std::string& lhs, const std::string& rhs,
                         std::size_t mode) {
    // Determine if we're working with ToTs
    const bool lhs_is_not_tot = lhs.find(";") == std::string::npos;
    const bool lhs_is_tot     = !lhs_is_not_tot;
    const bool rhs_is_not_tot = rhs.find(";") == std::string::npos;
    const bool rhs_is_tot     = !rhs_is_not_tot;

    if(lhs_is_not_tot && rhs_is_not_tot)
        return make_tensor_tensor_indices(lhs, rhs, mode);
    else if(lhs_is_not_tot && rhs_is_tot)
        return make_tensor_tot_indices(lhs, rhs, mode);
    else if(lhs_is_tot && rhs_is_not_tot)
        throw std::runtime_error("Can't transform a tensor by a ToT");

    return make_tot_tot_indices(lhs, rhs, mode);
}

} // namespace chemist::orbital_space::detail_
