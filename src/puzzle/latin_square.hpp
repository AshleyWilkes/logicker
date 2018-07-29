#pragma once
#include "../core/condition_instance.hpp"
#include "../core/integer_field.hpp"
#include "../core/grid.hpp"
#include "../core/rectangle.hpp"

#include <algorithm>
#include <vector>

namespace logicker::puzzle {
  using condition_description = std::pair<std::vector<core::CoordsMetaGroup>, core::ConditionDescription>;
  template<int size>
  class latin_square {
    public:
      typedef core::integer_field field_type;
      typedef core::rectangle topology;

      static const std::vector<condition_description>& get_condition_descriptions();
  };

  template<int size>
  const std::vector<condition_description>&
  latin_square<size>::get_condition_descriptions() {
    static std::vector<condition_description> cond_descs_{
      { { "Rows", "Cols" }, "EachValueOnce" }
    };
    return cond_descs_;
  }
}
