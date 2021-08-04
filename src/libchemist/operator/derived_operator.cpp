#include "derived_operator_pimpl.hpp"

namespace libchemist {

DerivedOperator::DerivedOperator() noexcept : pimpl_(nullptr){};

DerivedOperator::~DerivedOperator() noexcept = default;

DerivedOperator::DerivedOperator(const DerivedOperator& other) {
    *this = other;
}
DerivedOperator::DerivedOperator(DerivedOperator&& other) noexcept {
    *this = std::move(other);
}

DerivedOperator& DerivedOperator::operator=(const DerivedOperator& other) {
    if(other.pimpl_)
        this->pimpl_ = other.pimpl_->clone();
    else {
        pimpl_.reset();
        pimpl_ = nullptr;
    }
    return *this;
}
DerivedOperator& DerivedOperator::operator=(DerivedOperator&& other) noexcept {
    this->pimpl_ = std::move(other.pimpl_);
    return *this;
}

bool DerivedOperator::operator==(const DerivedOperator& other) const {
    if(pimpl_ == other.pimpl_) {
        // This handles the case when both are null or the same object
        return true;
    } else if((pimpl_ and not other.pimpl_) or (not pimpl_ and other.pimpl_)) {
        // This handles the case when one has a PIMPL and the other doesnt
        return false;
    } else {
        // This does the actual comparisons for distinct objects
        return *pimpl_ == *other.pimpl_;
    }
}

bool DerivedOperator::operator!=(const DerivedOperator& other) const {
    return !(*this == other);
}

void DerivedOperator::add_term_(std::type_index index,
                                std::shared_ptr<OperatorBase>&& op) {
    // Create PIMPL on addition of first term
    if(not pimpl_) pimpl_ = std::make_unique<detail_::DerivedOperatorPIMPL>();
    pimpl_->add_term(index, std::move(op));
}

DerivedOperator::get_return_type<OperatorBase> DerivedOperator::get_terms_(
  std::type_index index) const {
    if(pimpl_)
        return pimpl_->get_terms(index);
    else
        return get_return_type<OperatorBase>();
}

bool DerivedOperator::has_term_(std::type_index index) const noexcept {
    if(pimpl_) return pimpl_->has_term(index);
    return false;
}

void DerivedOperator::hash(pluginplay::Hasher& h) const {
    if(pimpl_) return pimpl_->hash(h);
}

} // namespace libchemist
