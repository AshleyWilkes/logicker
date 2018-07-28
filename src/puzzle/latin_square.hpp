#pragma once
#include "../core/condition_instance.hpp"
#include "../core/integer_field.hpp"
#include "../core/grid.hpp"
#include "../core/rectangle.hpp"

#include <algorithm>
#include <vector>

namespace logicker::puzzle {
  template<int size>
  class latin_square {
    public:
      typedef core::integer_field<1, size> field_type;
      typedef core::rectangle topology;

      latin_square();
      bool is_solved_by(const core::grid<field_type, topology>& grid) const;
      topology::size grid_size() const;
    private:
      topology::size grid_size_{ size, size };
      std::vector<core::condition_instance<field_type, topology>> grid_conds_;
  };

  template<int size>
  latin_square<size>::latin_square() {
    for (auto coords_group : topology::get_all_coords_groups(grid_size_)) {
      grid_conds_.push_back(
          core::condition_instance<field_type, topology>(logicker::core::neq,
            topology::get_coords_in_group(grid_size_, coords_group)));
    }
  }

  template<int size>
  bool
  latin_square<size>::is_solved_by(const core::grid<field_type, topology>& grid) const {
    return std::all_of(grid_conds_.begin(), grid_conds_.end(),
        [grid](auto& cond){ return cond.is_satisfied_by( grid ); });
  }

  template<int size>
  typename latin_square<size>::topology::size
  latin_square<size>::grid_size() const {
    return grid_size_;
  }
}
