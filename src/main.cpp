#include "puzzle/latin_square.hpp"
#include <iostream>
#include <vector>

int main() {
  std::cout << "This is Logicker!\n";
  try {
  using puzzle_type = logicker::puzzle::latin_square<4>;
  using field_type = puzzle_type::field_type;
  using topology = puzzle_type::topology;
  puzzle_type puzzle;

  std::vector<int> vals {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};

  logicker::core::grid<field_type, topology> sol_grid { puzzle.grid_size() };
  for (auto coords : topology::get_all_coords( puzzle.grid_size() )) {
    sol_grid.get_field(coords).set_limits(1, 4);
  }
  sol_grid.set_values(vals);

  std::cout << std::boolalpha << puzzle.is_solved_by(sol_grid) <<'\n';

  std::vector<int> vals2 {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};

  logicker::core::grid<field_type, topology> sol_grid2 { puzzle.grid_size() };
  for (auto coords : topology::get_all_coords( puzzle.grid_size() )) {
    sol_grid2.get_field(coords).set_limits(1, 4);
  }
  sol_grid2.set_values(vals2);

  std::cout << std::boolalpha << puzzle.is_solved_by(sol_grid2) <<'\n';
  } catch (char const* e) {
    std::cout << e;
  }
}
