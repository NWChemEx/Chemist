#include <utilities/strings/string_tools.hpp>

namespace libchemist::orbital_space::detail_ {

// Transforming a tensor by a tensor
inline auto make_tensor_tensor_indices(const std::string& lhs,
                                       const std::string& rhs,
                                       std::size_t mode) {
    using namespace utilities::strings;
    auto parsed_lhs = split_string(lhs, ",");

    if(parsed_lhs.size() != 2)
        throw std::runtime_error("Expected trasformation to be matrix");

    // Assume column vectors
    const std::string& new_idx  = parsed_lhs[1];
    const std::string& cont_idx = parsed_lhs[0];
    std::string idx2replace     = "j" + std::to_string(mode - 1);
    auto rhs_idx                = replace(rhs, idx2replace, cont_idx);
    auto result_idx             = replace(rhs, idx2replace, new_idx);
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
    std::string idx2replace = "j" + std::to_string(mode);
    new_rhs += replace(idx2replace, ldep_idx[0], rtot_idx[1]);
    result += replace(idx2replace, ldep_idx[1], rtot_idx[1]);

    return std::make_pair(result, new_rhs);
}

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

} // namespace libchemist::orbital_space::detail_