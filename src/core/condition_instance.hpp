#pragma once
#include "condition.hpp"
#include "grid.hpp"
#include <algorithm>
#include <boost/optional.hpp>
#include <memory>
#include <vector>

//some preprocessing should be present in this (most importantly excluding empties,
//i.e. not putting empties into values array)
namespace logicker::core {
  template<class FieldType, class Topology>
  class condition_instance {
    public:
      virtual bool is_satisfied_by(const grid<FieldType, Topology>& grid) const = 0;
      virtual boost::optional<typename grid<FieldType, Topology>::deduction_type>
        try_to_find_deduction(const grid<FieldType, Topology>& grid) const = 0;
      
      static std::unique_ptr<condition_instance>
      create_instance(std::string, const std::vector<typename Topology::coords>& fields);
  };

  template<class FieldType, class Topology>
  class simple_condition_instance : public condition_instance<FieldType, Topology> {
    public:
      simple_condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<typename Topology::coords>& fields);

      bool is_satisfied_by(const grid<FieldType, Topology>& grid) const override;
      boost::optional<typename grid<FieldType, Topology>::deduction_type>
        try_to_find_deduction(const grid<FieldType, Topology>& grid) const override;
    private:
      const condition<typename FieldType::value_type>& cond_;
      const std::vector<typename Topology::coords> fields_;
  };

  template<class FieldType, class Topology>
  class composite_condition_instance : public condition_instance<FieldType, Topology> {
    public:
      void add(std::unique_ptr<condition_instance<FieldType, Topology>> part);

      bool is_satisfied_by(const grid<FieldType, Topology>& grid) const override;
      boost::optional<typename grid<FieldType, Topology>::deduction_type>
        try_to_find_deduction(const grid<FieldType, Topology>& grid) const override;
    private:
      std::vector<std::unique_ptr<condition_instance<FieldType, Topology>>> parts_;
  };

  template<class FieldType, class Topology>
  simple_condition_instance<FieldType, Topology>::simple_condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<typename Topology::coords>& fields) : cond_{cond}, fields_{fields} {}

  //this assumes each solution field contains exactly 1 value
  //that's clearly not general enough, we can have empties
  template<class FieldType, class Topology>
  bool
  simple_condition_instance<FieldType, Topology>::is_satisfied_by(const grid<FieldType, Topology>& grid) const {
    std::vector<typename FieldType::value_type> vals;
    //std::transform(fields_.begin(), fields_.end(), std::back_inserter(vals),
    //    [grid](const Topology::coords& coord) { return grid.get_field(coord).get(); });
    for (auto coord : fields_) {
      vals.push_back(grid.get_field(coord).get());
    }
    return cond_.is_satisfied_by(vals);
  }

  template<class FieldType, class Topology>
  boost::optional<typename grid<FieldType, Topology>::deduction_type>
  simple_condition_instance<FieldType, Topology>::try_to_find_deduction(const grid<FieldType, Topology>& grid) const {
    if (fields_.size() != 2) {
      throw "simple_condition_instance::try_to_find_deduction -- only 2 fields deductioning impled";
    }
    //tady predpokladam, ze kdyz je field is_set, tak obsahuje hodnotu, ono vubec integer field je na pikacu
    FieldType f1 = grid.get_field(fields_[0]);
    FieldType f2 = grid.get_field(fields_[1]);
    if (f1.is_set() && f2.is_value_option(f1.get())) {
      return typename grid<FieldType, Topology>::deduction_type { fields_[1], f1.get() };
    } else if (f2.is_set() && f1.is_value_option(f2.get())) {
      return typename grid<FieldType, Topology>::deduction_type { fields_[0], f2.get() };
    }
    return boost::none;
  }

  template<class FieldType, class Topology>
  void
  composite_condition_instance<FieldType, Topology>::add(std::unique_ptr<condition_instance<FieldType, Topology>> part) {
    parts_.push_back(std::move(part));
  }

  template<class FieldType, class Topology>
  bool
  composite_condition_instance<FieldType, Topology>::is_satisfied_by(const grid<FieldType, Topology>& grid) const {
    for (auto it = parts_.begin(); it != parts_.end(); ++it) {
      if (! (*it)->is_satisfied_by(grid)) {
        return false;
      }
    }
    return true;
    //return std::all_of(parts_.begin(), parts_.end(), 
        //[grid](auto part){ return part->is_satisfied_by(grid); });
  }

  template<class FieldType, class Topology>
  boost::optional<typename grid<FieldType, Topology>::deduction_type>
  composite_condition_instance<FieldType, Topology>::try_to_find_deduction(const grid<FieldType, Topology>& grid) const {
    for (auto it = parts_.begin(); it != parts_.end(); ++it) {
      auto potential_deduction = (*it)->try_to_find_deduction(grid);
      if (potential_deduction) {
        return potential_deduction;
      }
      return boost::none;
    }
  }

  template<class FieldType, class Topology>
  std::unique_ptr<condition_instance<FieldType, Topology>>
  condition_instance<FieldType, Topology>::create_instance(core::ConditionDescription desc, const std::vector<typename Topology::coords>& fields) {
    std::unique_ptr<composite_condition_instance<FieldType, Topology>> result = std::make_unique<composite_condition_instance<FieldType, Topology>>();
    if (desc == "EachValueOnce") {
      int fields_size = fields.size();
      for (int i = 0; i < fields_size; ++i) {
        for (int j = i + 1; j < fields_size; ++j) {
          std::vector<typename Topology::coords> temp_fields{ fields.at(i), fields.at(j) };
          auto temp_instance = std::make_unique<simple_condition_instance<FieldType, Topology>>( neq, temp_fields );
          //result.add(std::make_unique<simple_condition_instance<FieldType, Topology>>(temp_instance));
          result->add(std::move(temp_instance));
        }
      }
    } else {
      throw "condition::create_instance: unknown description";
    }
    return result;
  }
}
