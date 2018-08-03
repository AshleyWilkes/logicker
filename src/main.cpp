#include "puzzle/latin_square.hpp"
#include "core/input_node.hpp"
#include "core/puzzle_instance.hpp"
#include "core/puzzle_instance_factory.hpp"
#include "core/checker.hpp"
#include "core/solver.hpp"
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <map>
#include <memory>
#include <streambuf>
#include <string>
#include <vector>

int main() {
  try {
  std::cout << "This is Logicker!\n";
  std::ifstream is{ "latin_square2x2.lgc" };
  std::string input_str{ std::istreambuf_iterator<char>( is ),
    std::istreambuf_iterator<char>() };

  using logicker::puzzle::latin_square;
  using logicker::core::puzzle_instance;
  using factory_t = logicker::core::puzzle_instance_factory<latin_square>;
  using puzzle_t = puzzle_instance<latin_square>;
  using checker_t = logicker::core::checker<puzzle_t>;

  //using base_input = logicker::core::input_node_base;
  //using int_input = logicker::core::int_input_node;
  using composite_input = logicker::core::composite_input_node;

  std::vector<int> vals {
    1,2,
    2,1};
  /*std::vector<int> vals {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};*/

  Json::Value input_json_value;
  Json::Reader reader;
  if (! reader.parse(input_str, input_json_value, false)) {
    throw "parsing json error: probably malformed syntax\n";
  }

  composite_input input{ "Input", input_json_value };

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
