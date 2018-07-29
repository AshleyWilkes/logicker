#include "puzzle/latin_square.hpp"
#include "core/puzzle_instance.hpp"
#include "core/checker.hpp"
#include <iostream>
#include <vector>

int main() {
  std::cout << "This is Logicker!\n";
  //tak to by neslo, potrebuju classu solver (a napred checker)
  /*try {
    using puzzle_type = logicker::puzzle::latin_square<2>;
    using field_type = puzzle_type::field_type;
    using topology = puzzle_type::topology;
    puzzle_type puzzle;

    puzzle.get_field({0, 0}).set_value(1);
    while ( auto deduction = puzzle.try_to_find_deduction()) {
      puzzle.apply_deduction(deduction);
    }
    std::cout << puzzle.get_grid();
  } catch (char const* e) {
    std::cout << e;
  }*/

  try {
  using puzzle_type = logicker::puzzle::latin_square<4>;
  using puzzle_instance = logicker::core::puzzle_instance<puzzle_type>;
  puzzle_instance puzzle({ 4, 4 });
  using checker = logicker::core::checker<puzzle_type>;
  checker chckr{ std::move(puzzle) };
  using field_type = puzzle_type::field_type;
  using topology = puzzle_type::topology;

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

  std::cout << std::boolalpha << chckr.is_solved_by(sol_grid) <<'\n';
  std::cout << sol_grid;

  } catch (char const* e) {
    std::cout << e;
  }
}
