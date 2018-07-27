#include "Checker/FieldStatus.hpp"
#include <iostream>

int main() {
  std::cout << "This is Logicker!\n";

  using FieldType = int;
  using Top = Logicker::Topology::Rectangle;
  using Coords = Top::Coords;
  //Logicker::Grid<FieldType, Top> grid{ 2, 2 };

  std::vector<Logicker::Checker::GridCondition<int, Coords>> grid_conds;

  std::vector<Coords> coords1 {{0, 0}, {0, 1}};//row1
  grid_conds.push_back(
      Logicker::Checker::GridCondition<int, Coords>(Logicker::neq, coords1));
  std::vector<Coords> coords2 {{1, 0}, {1, 1}};//row2
  grid_conds.push_back(
      Logicker::Checker::GridCondition<int, Coords>(Logicker::neq, coords2));
  std::vector<Coords> coords3 {{0, 0}, {1, 0}};//col1
  grid_conds.push_back(
      Logicker::Checker::GridCondition<int, Coords>(Logicker::neq, coords3));
  std::vector<Coords> coords4 {{0, 1}, {1, 1}};//col2
  grid_conds.push_back(
      Logicker::Checker::GridCondition<int, Coords>(Logicker::neq, coords4));

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
