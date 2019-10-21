#pragma once

namespace libchemist::detail_ {

template<bool is_const, typename AliasedType>
class ViewBase {
private:
    using my_type     = ViewBase<is_const, AliasedType>;
    using alias_ptr   = std::shared_ptr<AliasedType>;
    using const_alias = const AliasedType&;
    using alias_reference =
      std::conditional_t<is_const, const_alias, AliasedType&>;
    template<bool rhs_const>
    using other_base = ViewBase<rhs_const, AliasedType>;

public:
    explicit ViewBase(AliasedType pimpl);
    explicit ViewBase(alias_ptr ppimpl) : m_pimpl_(ppimpl) {}
    ViewBase(const my_type& rhs) = default;
    ViewBase& operator=(const my_type& rhs) = default;
    ViewBase(my_type&& rhs) noexcept        = default;
    ViewBase& operator=(my_type&& rhs) noexcept = default;
    virtual ~ViewBase()                         = default;
    operator const_alias() const noexcept { return *m_pimpl_; }
    bool operator==(const_alias rhs) const noexcept;
    template<bool rhs_const>
    bool operator==(const other_base<rhs_const>& rhs) const noexcept;
    bool operator!=(const_alias rhs) const noexcept;
    template<bool rhs_const>
    bool operator!=(const other_base<rhs_const>& rhs) const noexcept;

protected:
    alias_reference pimpl() noexcept { return *m_pimpl_; }
    const_alias pimpl() const noexcept { return *m_pimpl_; }

private:
    std::shared_ptr<AliasedType> m_pimpl_;
};

// --------------------------Implementations-----------------------------------
#define VIEW_BASE_TEMPLATE_PARAMS template<bool is_const, typename AliasedType>
#define VIEW_BASE ViewBase<is_const, AliasedType>

VIEW_BASE_TEMPLATE_PARAMS
bool operator==(const AliasedType& lhs, const VIEW_BASE& rhs) noexcept {
    return lhs == static_cast<const AliasedType&>(rhs);
}

VIEW_BASE_TEMPLATE_PARAMS
bool operator!=(const AliasedType& lhs, const VIEW_BASE& rhs) noexcept {
    return !(lhs == static_cast<const AliasedType&>(rhs));
}

VIEW_BASE_TEMPLATE_PARAMS
VIEW_BASE::ViewBase(AliasedType pimpl) :
  ViewBase(std::make_shared<AliasedType>(std::move(pimpl))) {}

VIEW_BASE_TEMPLATE_PARAMS
bool VIEW_BASE::operator==(const_alias rhs) const noexcept {
    return pimpl() == rhs;
}

VIEW_BASE_TEMPLATE_PARAMS
template<bool rhs_const>
bool VIEW_BASE::operator==(const other_base<rhs_const>& rhs) const noexcept {
    return pimpl() == static_cast<const AliasedType&>(rhs);
}

VIEW_BASE_TEMPLATE_PARAMS
bool VIEW_BASE::operator!=(const_alias rhs) const noexcept {
    return !((*this) == rhs);
}

VIEW_BASE_TEMPLATE_PARAMS
template<bool rhs_const>
bool VIEW_BASE::operator!=(const other_base<rhs_const>& rhs) const noexcept {
    return !((*this) == rhs);
}

#undef VIEW_BASE
#undef VIEW_BASE_TEMPLATE_PARAMS
} // namespace libchemist::detail_
