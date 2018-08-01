#include "puzzle/latin_square.hpp"
#include "core/input_node.hpp"
#include "core/puzzle_instance.hpp"
#include "core/puzzle_instance_factory.hpp"
#include "core/checker.hpp"
#include "core/solver.hpp"
#include <iostream>
#include <map>
#include <memory>
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
  using logicker::puzzle::latin_square;
  using logicker::core::puzzle_instance;
  using factory_t = logicker::core::puzzle_instance_factory<latin_square>;
  using puzzle_t = puzzle_instance<latin_square>;
  using checker_t = logicker::core::checker<puzzle_t>;

  using base_input = logicker::core::input_node_base;
  using int_input = logicker::core::int_input_node;
  using composite_input = logicker::core::composite_input_node;

  std::map<std::string, std::shared_ptr<base_input>> map;
  std::shared_ptr<base_input> size_input = std::make_shared<int_input>(int_input{ "Size", 2 });
  std::vector<int> vals {
    1,2,
    2,1};
  /*std::shared_ptr<base_input> size_input = std::make_shared<int_input>(int_input{ "Size", 4 });
  std::vector<int> vals {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};*/

  map.insert({ "Size", std::move(size_input) });

  std::map<std::string, std::shared_ptr<base_input>> givens_map;
  std::map<std::string, std::shared_ptr<base_input>> given1_map;
  std::shared_ptr<int_input> row_input = std::make_shared<int_input>(int_input{ "Row", 0 });
  given1_map.insert({ "Row", row_input });
  std::shared_ptr<int_input> col_input = std::make_shared<int_input>(int_input{ "Col", 0 });
  given1_map.insert({ "Col", col_input });
  std::shared_ptr<int_input> val_input = std::make_shared<int_input>(int_input{ "Value", 1 });
  given1_map.insert({ "Value", val_input });
  std::shared_ptr<base_input> given1_input = std::make_shared<composite_input>(composite_input{ "[0, 0]", given1_map });

  givens_map.insert({ "[0, 0]", given1_input });
  std::shared_ptr<base_input> givens_input = std::make_shared<composite_input>(composite_input{ "Givens", givens_map });
  map.insert({ "Givens", std::move(givens_input) });

  composite_input input{ "Input", map };
  puzzle_t puzzle_inst = factory_t::create( input );
  checker_t checker{ std::move(puzzle_inst) };

  std::cout << "Solved by vals: " << std::boolalpha << checker.is_solved_by(vals) << '\n';

  puzzle_t::grid_type sol_grid = checker.get_assignment_grid();
  sol_grid.set_values(vals);

  std::cout << "Solved by grid: " << std::boolalpha << checker.is_solved_by(sol_grid) <<'\n';

  using solver_t = logicker::core::solver<puzzle_t>;

  puzzle_t puzzle_inst2 = factory_t::create( input );
  solver_t solver{ std::move(puzzle_inst2) };
  std::cout << "Solver's solution:\n" << solver.get_solution();

  } catch (char const* e) {
    std::cout << e << '\n';
  } catch (std::string e) {
    std::cout << e << '\n';
  }
}
