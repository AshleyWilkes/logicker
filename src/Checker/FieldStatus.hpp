#include <map>
#include <vector>

//it is implicitly expected values form a linear structure, while that's
//not the case (the structure can be nonexistent (set of values when
//order doesn't matter) or nonlinear (e.g. square for e.g. loops))
namespace Logicker {
  template<class Value>
  class Condition {
    public:
      Condition(bool (*eval)(const std::vector<Value>&));
      bool is_satisfied_by(const std::vector<Value>& values) const;
    private:
      bool (*const eval_)(const std::vector<Value>&);
  };

  template<class Value>
  Condition<Value>::Condition(bool (*eval)(const std::vector<Value>&)) : eval_{eval} {};

  template<class Value>
  bool
  Condition<Value>::is_satisfied_by(const std::vector<Value>& values) const {
    return eval_(values);
  }

  auto neq_lambda = [](const std::vector<int>& values) {
    return values[0] != values[1];
  };

  Condition<int> neq {neq_lambda};
}

//this assumes each solution fiels contains exactly 1 value
//that's clearly not general enough, we can have empties
//
//the aforementioned assumption manifests in Field (as an abstraction
//between Grid and Value) missing
namespace Logicker::Checker {
  using FieldCoords = int;

  template<class Value>
  class SolutionGrid {
    public:
      SolutionGrid(const std::map<FieldCoords, Value>& grid_values);
      const Value& get_value(const FieldCoords& coords) const;
    private:
      const std::map<FieldCoords, Value>& vals_;
  };

  template<class Value>
  SolutionGrid<Value>::SolutionGrid(const std::map<FieldCoords, Value>& grid_values) : vals_{grid_values} {}

  template<class Value>
  const Value& SolutionGrid<Value>::get_value(const FieldCoords& coords) const {
    return vals_.find(coords)->second;
  }
}

//some preprocessing should be present in this (most importantly excluding empties,
//i.e. not putting empties into values array)
namespace Logicker::Checker {
  template<class Value>
  class GridCondition {
    public:
      GridCondition(const Condition<Value>& cond, const std::vector<FieldCoords>& fields);
      bool is_satisfied_by(const SolutionGrid<Value>& grid) const;
    private:
      const Condition<Value>& cond_;
      const std::vector<FieldCoords>& fields_;
  };

  template<class Value>
  GridCondition<Value>::GridCondition(const Condition<Value>& cond, const std::vector<FieldCoords>& fields) : cond_{cond}, fields_{fields} {}

  template<class Value>
  bool
  GridCondition<Value>::is_satisfied_by(const SolutionGrid<Value>& grid) const {
    std::vector<Value> vals;
    for (auto coord : fields_) {
      vals.push_back(grid.get_value(coord));
    }
    return cond_.is_satisfied_by(vals);
  }
}

