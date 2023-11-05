#pragma once
#include "fragmented_pimpl.hpp"
#include <chemist/nucleus/nuclei.hpp>

namespace chemist::fragmenting::detail_ {

template<>
class FragmentedPIMPL<Nuclei> {
public:
    using value_type    = Nuclei;
    using value_pointer = std::shared_ptr<const Nuclei>;
    using cap_set_type  = CapSet;

    /// Type Type2Fragment uses to refer to indices/offsets
    using size_type     = value_type::size_type;

    explicit FragmentedPIMPL(value_pointer nuclei,
                             cap_set_type caps = {}) : 
        m_nuclei_(nuclei), m_caps_(std::move(caps)), m_fragments_() {}    


    template<typename...Args>
    void add_fragment(Args&&...args) {
        check_arg_types_<Args...>();
        m_fragments_.push_back(fragment_type{std::forward<Args>(args)...});
    }


    size_type size() const noexcept {
        return !is_null_() ? m_nuclei_->size() : size_type{0};
    }

private:
    /// Type storing the indices belonging to a specific fragment
    using fragment_type = boost::flat_set<size_type>;

    /// Type holding all of the fragments
    using fragment_container = std::vector<fragment_type>;
    
    bool is_null_() const noexcept {
        return !static_cast<bool>(m_nuclei_);
    }

    template<size_type I, typename...Args>
    static void constexpr check_arg_types_() {
        if constexpr(I == sizeof...(Args)){ return; }
        else {
            using tuple_t = std::tuple<Args...>;
            using type_i = std::tuple_element_t<I, tuple_t>;
            static constexpr bool is_convertible = 
                std::is_convertible_v<type_i, size_type>;
            static_assert(is_convertible,
            "Nuclei indices must be convertible to size_type");
            check_arg_types_<I + 1, Args...>();
        }
    }


    /// The superset of Nuclei
    value_pointer m_nuclei_;

    cap_set_type m_caps_;

    fragment_container m_fragments_;
};


}