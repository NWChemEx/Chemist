#pragma once
#include <LibChemist/Defaults/PropertyTypes.hpp>
#include <SDE/ModuleBase.hpp>

template<typename element_type = double>
tamm::Tensor<element_type> zero_dim_tens(element_type val) {
    tamm::ProcGroup pg{GA_MPI_Comm()};
    auto *pMM = tamm::MemoryManagerGA::create_coll(pg);
    tamm::Distribution_NW dist;
    tamm::ExecutionContext ec(pg, &dist, pMM);

    tamm::Tensor<element_type> T{};
    tamm::Scheduler(ec).allocate(T)
    (T() = val)
    .execute();
    return T;
}

template<std::size_t NBases = 2, typename element_type = double>
class DummyInts : public SDE::ModuleBase {
public:
    using prop_type = LibChemist::AOIntegral<NBases,element_type>;

    DummyInts() : SDE::ModuleBase(this) {
        satisfies_property_type<prop_type>();
    }
private:
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map submods) const override {

        const auto [mol, bs_arr, deriv] = prop_type::unwrap_inputs(inputs);
        auto T = zero_dim_tens<element_type>(7.0);
        auto result = results();
        result = prop_type::wrap_results(result,T);
        return result;
    }
};

template<typename element_type = double>
class DummyCore : public SDE::ModuleBase {
public:
    using prop_type = LibChemist::CoreHamiltonian<element_type>;

    DummyCore() : SDE::ModuleBase(this) {
        satisfies_property_type<prop_type>();
    }
private:
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map submods) const override {

        const auto [mol, bra, ket, deriv] = prop_type::unwrap_inputs(inputs);
        auto T = zero_dim_tens<element_type>(7.0);
        auto result = results();
        result = prop_type::wrap_results(result,T);
        return result;
    }
};

template<typename element_type = double>
class DummyJK : public SDE::ModuleBase {
public:
    using prop_type = LibChemist::JKMatrices<element_type>;

    DummyJK() : SDE::ModuleBase(this) {
        satisfies_property_type<prop_type>();
    }
private:
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map submods) const override {

        const auto [mol, orbs, bra, ket, deriv] = prop_type::unwrap_inputs(inputs);
        auto J = zero_dim_tens<element_type>(7.0);
        auto K = zero_dim_tens<element_type>(8.0);
        auto result = results();
        result = prop_type::wrap_results(result,J,K);
        return result;
    }
};

template<typename element_type = double>
class DummyFock : public SDE::ModuleBase {
public:
    using prop_type = LibChemist::FockBuilder<element_type>;

    DummyFock() : SDE::ModuleBase(this) {
        satisfies_property_type<prop_type>();
    }
private:
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map submods) const override {

        const auto [mol, orbs, bra, ket, deriv] = prop_type::unwrap_inputs(inputs);
        auto T = zero_dim_tens<element_type>(7.0);
        auto result = results();
        result = prop_type::wrap_results(result,T);
        return result;
    }
};

template<typename element_type = double>
class DummE : public SDE::ModuleBase {
public:
    using prop_type = LibChemist::Energy<element_type>;

    DummE() : SDE::ModuleBase(this) {
        satisfies_property_type<prop_type>();
    }
private:
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map submods) const override {

        const auto [mol, deriv] = prop_type::unwrap_inputs(inputs);
        auto T = zero_dim_tens<element_type>(7.0);
        auto result = results();
        result = prop_type::wrap_results(result,T);
        return result;
    }
};
