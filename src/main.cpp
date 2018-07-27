#include "Checker/FieldStatus.hpp"
#include <iostream>

int main() {
  std::cout << "This is Logicker!\n";

  using FieldType = int;
  using Top = Logicker::Topology::Rectangle;
  using Coords = Top::Coords;
  Top::Size grid_size{ 2, 2 };
  Logicker::Grid<FieldType, Top> grid{ grid_size };

  std::vector<Logicker::Checker::GridCondition<int, Top>> grid_conds;
  for (auto coords_group : grid.get_all_coords_groups()) {
    grid_conds.push_back(
        Logicker::Checker::GridCondition<int, Top>(Logicker::neq, 
          grid.get_coords_in_group(coords_group)));
  }

  std::map<Coords, int> grid_vals;
  Coords c0{0, 0};
  grid_vals[c0] = 1;
  Coords c1{0, 1};
  grid_vals[c1] = 2;
  Coords c2{1, 0};
  grid_vals[c2] = 2;
  Coords c3{1, 1};
  grid_vals[c3] = 1;

  Logicker::Checker::SolutionGrid<int, Coords> solution_grid {grid_vals};

  bool result = true;
  for (auto cond : grid_conds) {
    result &= cond.is_satisfied_by(solution_grid);
  }

  std::cout << std::boolalpha << result << '\n';
}
