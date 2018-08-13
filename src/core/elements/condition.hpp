#pragma once
#include <string>
#include <vector>

//it is implicitly expected values form a linear structure, while that's
//not the case (the structure can be nonexistent (set of values when
//order doesn't matter) or nonlinear (e.g. square for e.g. loops))
//
//takze tady by mel byt mozna jeste dalsi type paramater, namely Topology
namespace logicker::core {
  using ConditionDescription = std::string;

  template<class Value>
  class condition {
    public:
      condition(bool (*eval)(const std::vector<Value>&));
      bool is_satisfied_by(const std::vector<Value>& values) const;
    private:
      bool (*eval_)(const std::vector<Value>&);
  };

  template<class Value>
  condition<Value>::condition(bool (*eval)(const std::vector<Value>&)) : eval_{eval} {};

  template<class Value>
  bool
  condition<Value>::is_satisfied_by(const std::vector<Value>& values) const {
    return eval_(values);
  }

  auto neq_lambda = [](const std::vector<int>& values) {
    return values[0] != values[1];
  };

  condition<int> neq {neq_lambda};
}
