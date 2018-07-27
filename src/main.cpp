#include "Checker/FieldStatus.hpp"
#include <iostream>

int main() {
  std::cout << "This is Logicker!\n";

  using FieldType = Logicker::IntegerField<1, 2>;
  using Top = Logicker::Topology::Rectangle;
  using Coords = Top::Coords;
  Top::Size grid_size{ 2, 2 };
  Logicker::Grid<FieldType, Top> grid{ grid_size };

  std::vector<Logicker::Checker::GridCondition<FieldType, Top>> grid_conds;
  for (auto coords_group : grid.get_all_coords_groups()) {
    grid_conds.push_back(
        Logicker::Checker::GridCondition<FieldType, Top>(Logicker::neq, 
          grid.get_coords_in_group(coords_group)));
  }

  Coords c0{0, 0};
  grid.get_field(c0).set(1);
  Coords c1{0, 1};
  grid.get_field(c1).set(2);
  Coords c2{1, 0};
  grid.get_field(c2).set(2);
  Coords c3{1, 1};
  grid.get_field(c3).set(1);

  bool result = true;
  for (auto cond : grid_conds) {
    result &= cond.is_satisfied_by(grid);
  }

  std::cout << std::boolalpha << result << '\n';
}
