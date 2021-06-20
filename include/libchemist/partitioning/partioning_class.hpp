#include pragma
#include "libchemist/partitioning/partition.hpp"

namespace libchemist::partioning {

template<typename Obj2Partition>
class Partitioning {
public:
    using value_type      = Partition<Obj2Partition>;
    using reference_type  = value_type&;
    using const_reference = const value_type&;

    explicit Partitioning(Obj2Partition obj);

    bool disjoint() const noexcept;
    reference_type new_partition();

    const auto& object() const { return m_obj_; }

private:
    std::vector<value_type> m_partitionings_;
    Obj2Partition m_obj_;
};

#define PARTITIONING Partitioning<Obj2Partition>

template<typename Obj2Partition>
typename PARTITIONING::reference_type PARTITIONING::new_partition() {
    m_partitionings_.push_back(value_type(*this));
    return m_partitionings_.back();
}

#undef PARTITIONING

} // namespace libchemist::partioning