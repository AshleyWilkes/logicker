#pragma once
#include "condition.hpp"
#include "grid.hpp"
#include <vector>

//some preprocessing should be present in this (most importantly excluding empties,
//i.e. not putting empties into values array)
namespace logicker::core {
  template<class FieldType, class Topology>
  class condition_instance {
    public:
      condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<typename Topology::coords>& fields);
      bool is_satisfied_by(const grid<FieldType, Topology>& grid) const;
    private:
      const condition<typename FieldType::value_type>& cond_;
      const std::vector<typename Topology::coords> fields_;
  };

  template<class FieldType, class Topology>
  condition_instance<FieldType, Topology>::condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<typename Topology::coords>& fields) : cond_{cond}, fields_{fields} {}

  //this assumes each solution field contains exactly 1 value
  //that's clearly not general enough, we can have empties
  template<class FieldType, class Topology>
  bool
  condition_instance<FieldType, Topology>::is_satisfied_by(const grid<FieldType, Topology>& grid) const {
    std::vector<typename FieldType::value_type> vals;
    //std::transform(fields_.begin(), fields_.end(), std::back_inserter(vals),
    //    [grid](const Topology::coords& coord) { return grid.get_field(coord).get(); });
    for (auto coord : fields_) {
      vals.push_back(grid.get_field(coord).get());
    }
    return cond_.is_satisfied_by(vals);
  }
}
