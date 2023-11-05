#pragma once
#include "detail_/fragmented_pimpl.hpp"

namespace chemist::fragmenting {

template<typename TypeToFragment>
class Fragmented {
public:
    using pimpl_type    = detail_::FragmentedPIMPL<TypeToFragment>;
    
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// The conceptual type of a fragment
    using value_type = typename pimpl_type::value_type;

    /// Unsigned integer type used for indexing and offsets
    using size_type = typename pimpl_type::size_type;

    template<typename...Args>
    void add_fragment(Args&&...args) {
        assert_pimpl_();
        m_pimpl_->add_fragment(std::forward<Args>(args)...);
    }

    size_type size() const noexcept {
        return has_pimpl_() ? m_pimpl_->size() : size_type{0};
    }

    bool empty() const noexcept {
        return has_pimpl_() ? m_pimpl_->size() == 0 : true;
    }

private:
    /// False if m_pimpl_ is a null pointer, true otherwise
    bool has_pimpl_() const noexcept { return static_cast<bool>(m_pimpl_); }

    /// Throws if *this has no PIMPL
    void assert_pimpl_() const {
        if(has_pimpl_()) return;
        throw std::runtime_error("Fragmented object has no PIMPL");
    }

    pimpl_pointer m_pimpl_;
};

}