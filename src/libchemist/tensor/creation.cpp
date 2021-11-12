#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/tensor/creation.hpp"

namespace libchemist::tensor {

ScalarTensorWrapper concatenate(const ScalarTensorWrapper& lhs,
                                const ScalarTensorWrapper& rhs,
                                std::size_t dim) {
    const auto& C_lhs = lhs.get<TA::TSpArrayD>();
    const auto& C_rhs = rhs.get<TA::TSpArrayD>();
    const auto rank   = C_lhs.trange().rank();

    if(rank != C_rhs.trange().rank())
        throw std::runtime_error("Must have the same rank");

    if(dim >= rank) throw std::runtime_error("dim must be in [0, rank)");

    std::vector<int> lhs_lo, lhs_hi, rhs_lo, rhs_hi;
    std::vector<TA::TiledRange1> out_tr1;
    for(std::size_t i = 0; i < rank; ++i) {
        const auto& lhs_tr1 = C_lhs.trange().dim(i);
        const auto& rhs_tr1 = C_rhs.trange().dim(i);

        const auto nlhs = lhs_tr1.tile_extent();
        const auto nrhs = rhs_tr1.tile_extent();
        lhs_lo.push_back(0);
        if(i == dim) {
            lhs_hi.push_back(nlhs);
            rhs_lo.push_back(nlhs);
            rhs_hi.push_back(nlhs + nrhs);
            out_tr1.push_back(TA::concat(lhs_tr1, rhs_tr1));
            continue;
        } else if(lhs_tr1 != rhs_tr1)
            throw std::runtime_error("Must have the same shape");

        lhs_hi.push_back(nlhs);
        rhs_lo.push_back(0);
        rhs_hi.push_back(nlhs);
        out_tr1.push_back(lhs_tr1);
    }

    TA::TiledRange out_tr(out_tr1.begin(), out_tr1.end());
    TA::TSpArrayD C_out(C_lhs.world(), out_tr);
    auto idx                         = lhs.make_annotation();
    C_out(idx).block(lhs_lo, lhs_hi) = C_lhs(idx);
    C_out(idx).block(rhs_lo, rhs_hi) = C_rhs(idx);

    const auto total = out_tr1[dim].extent();
    TA::TiledRange1 new_tr1(0, total);
    out_tr1[dim] = new_tr1;
    TA::TiledRange new_tr(out_tr1.begin(), out_tr1.end());
    C_out = TA::retile(C_out, new_tr);

    return ScalarTensorWrapper(C_out);
}

TensorOfTensorsWrapper concatenate(const TensorOfTensorsWrapper& lhs,
                                   const TensorOfTensorsWrapper& rhs,
                                   std::size_t dim) {
    throw std::runtime_error("NYI");
    // Guts of the ToT matrix concatenate copy/pasted from MP2
    //     const auto& Cocc = occ.C();
    //     const auto& Cpno = pno.C();

    //     using tot_type = std::decay_t<decltype(Cocc)>;
    //     using namespace libchemist::ta_helpers;

    //     if(Cocc.trange() != Cpno.trange()) {
    //         throw std::runtime_error("TiledRanges are assumed the same");
    //     }

    //     auto l = [=](auto& t, const TA::Range& r) {
    //         using tile_type   = std::decay_t<decltype(t)>;
    //         using tensor_type = typename tile_type::value_type;

    //         auto tile_idx        = get_block_idx(Cocc.trange(), r);
    //         auto occ_block       = Cocc.find(tile_idx).get();
    //         auto pno_block       = Cpno.find(tile_idx).get();
    //         auto& occ_tile_range = occ_block.range();

    //         if(occ_tile_range != pno_block.range())
    //             throw std::runtime_error("Huh? Thought we already checked
    //             this...");

    //         t = tile_type(r);
    //         for(auto& ij : occ_tile_range) {
    //             auto& occ_ij = occ_block(ij);
    //             auto& pno_ij = pno_block(ij);
    //             auto naos    = occ_ij.range().extent(0);
    //             if(naos != pno_ij.range().extent(0))
    //                 throw std::runtime_error("Expected the same number of
    //                 AOs");

    //             auto npno = pno_ij.range().extent(1);
    //             TA::Range new_range(naos, 1 + npno);
    //             tensor_type inner(new_range, 0.0);

    //             using size_type = decltype(naos);
    //             for(size_type mu = 0; mu < naos; ++mu) {
    //                 inner({mu, size_type{0}}) = occ_ij({mu});

    //                 for(decltype(npno) a = 0; a < npno; ++a)
    //                     inner({mu, 1 + a}) = pno_ij({mu, a});
    //             }
    //             t(ij) = inner;
    //         }
    //         return t.norm();
    //     };

    //     auto Corb = TA::make_array<tot_type>(Cocc.world(), Cocc.trange(),
    //     l); return mp2::type::sparse_derived<double>(Corb,
    //     occ.from_space());
}

ScalarTensorWrapper grab_diagonal(const ScalarTensorWrapper& t) {
    const auto& t_ta = t.get<TA::TSpArrayD>();

    return ScalarTensorWrapper(ta_helpers::grab_diagonal(t_ta));
}

} // namespace libchemist::tensor
