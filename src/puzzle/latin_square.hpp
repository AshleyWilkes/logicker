#pragma once
#include "../core/condition_instance.hpp"
#include "../core/elements/field_type/integer_field_type.hpp"
#include "../core/field.hpp"
#include "../core/grid.hpp"
#include "../core/elements/topology/square.hpp"

#include <algorithm>
#include <vector>

namespace logicker::puzzle {
  using condition_description = std::pair<std::vector<core::CoordsMetaGroup>, core::condition::ConditionDescription>;
  class latin_square {
    public:
      using value_type = int;
      using field_type = core::field_type<value_type>;
      using topology = core::square;
      using condition = core::condition::each_value_once_in_rows_and_cols;

      static const std::vector<condition_description>& get_condition_descriptions();
  };

  const std::vector<condition_description>&
  latin_square::get_condition_descriptions() {
    static std::vector<condition_description> cond_descs_{
      { { "Rows", "Cols" }, "EachValueOnce" }
    };
    return cond_descs_;
  }
}
