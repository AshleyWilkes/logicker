#include "Checker/FieldStatus.hpp"
#include <algorithm>
#include <iostream>

int main() {
  std::cout << "This is Logicker!\n";

  using FieldType = Logicker::IntegerField<1, 2>;
  using Top = Logicker::Topology::Rectangle;
  Top::Size grid_size{ 2, 2 };

  std::vector<Logicker::Checker::GridCondition<FieldType, Top>> grid_conds;
  for (auto coords_group : Top::get_all_coords_groups(grid_size)) {
    grid_conds.push_back(
        Logicker::Checker::GridCondition<FieldType, Top>(Logicker::neq, 
          Top::get_coords_in_group(grid_size, coords_group)));
  }

  Logicker::Grid<FieldType, Top> grid{ grid_size };
  std::vector<int> vals {1, 2, 2, 1};
  grid.set_values(vals);

  bool result = std::all_of(grid_conds.begin(), grid_conds.end(),
      [grid](auto& cond){ return cond.is_satisfied_by(grid); } );

  std::cout << std::boolalpha << result << '\n';
}
