#include "puzzle/latin_square.hpp"
#include <iostream>
#include <vector>

int main() {
  std::cout << "This is Logicker!\n";

  std::vector<int> vals {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};
  logicker::puzzle::latin_square<4> puzzle;

  logicker::core::grid<logicker::puzzle::latin_square<4>::field_type, logicker::puzzle::latin_square<4>::topology> sol_grid { puzzle.grid_size() };
  sol_grid.set_values(vals);

  std::cout << std::boolalpha << puzzle.is_solved_by(sol_grid) <<'\n';
}
