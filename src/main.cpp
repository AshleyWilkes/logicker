#include "core/condition_instance.hpp"
#include "core/grid.hpp"
#include "core/integer_field.hpp"
#include "core/rectangle.hpp"
#include <algorithm>
#include <iostream>

int main() {
  std::cout << "This is Logicker!\n";

  using namespace logicker::core;
  using FieldType = integer_field<1, 2>;
  using Topology = rectangle;
  Topology::size grid_size{ 2, 2 };

  std::vector<condition_instance<FieldType, Topology>> grid_conds;
  for (auto coords_group : Topology::get_all_coords_groups(grid_size)) {
    grid_conds.push_back(
        condition_instance<FieldType, Topology>(neq, 
          Topology::get_coords_in_group(grid_size, coords_group)));
  }

  grid<FieldType, Topology> grid{ grid_size };
  std::vector<FieldType::value_type> vals {1, 2, 2, 1};
  grid.set_values(vals);

  bool result = std::all_of(grid_conds.begin(), grid_conds.end(),
      [grid](auto& cond){ return cond.is_satisfied_by(grid); } );

  std::cout << std::boolalpha << result << '\n';
}
