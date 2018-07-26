#include "Checker/FieldStatus.hpp"
#include <iostream>

int main() {
  std::cout << "This is Logicker!\n";
  std::vector<int> coords1 {0, 1};
  std::vector<int> coords2 {2, 3};
  std::vector<int> coords3 {0, 2};
  std::vector<int> coords4 {1, 3};

  std::vector<Logicker::Checker::GridCondition<int>> grid_conds;
  grid_conds.push_back(Logicker::Checker::GridCondition<int>(Logicker::neq, coords1));
  grid_conds.push_back(Logicker::Checker::GridCondition<int>(Logicker::neq, coords2));
  grid_conds.push_back(Logicker::Checker::GridCondition<int>(Logicker::neq, coords3));
  grid_conds.push_back(Logicker::Checker::GridCondition<int>(Logicker::neq, coords4));

  std::map<int, int> grid_vals;
  grid_vals[0] = 1;
  grid_vals[1] = 2;
  grid_vals[2] = 2;
  grid_vals[3] = 1;

  Logicker::Checker::SolutionGrid<int> solution_grid {grid_vals};

  bool result = true;
  for (auto cond : grid_conds) {
    result &= cond.is_satisfied_by(solution_grid);
  }

  std::cout << std::boolalpha << result << '\n';
}
