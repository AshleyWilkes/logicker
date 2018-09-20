#include "puzzle/latin_square.hpp"
#include "core/input_node.hpp"
#include "core/puzzle_instance.hpp"
#include "core/puzzle_instance_factory.hpp"
#include "checker/checker_old.hpp"
#include "checker/checker.hpp"
#include "solver/solver_old.hpp"
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
  std::ifstream is{ "latin_square3x3.lgc" };
  std::string input_str{ std::istreambuf_iterator<char>( is ),
    std::istreambuf_iterator<char>() };

  using logicker::puzzle::latin_square;
  using logicker::core::puzzle_instance_v1;
  using factory_t = logicker::core::puzzle_instance_factory<latin_square>;
  using puzzle_t = puzzle_instance_v1<latin_square>;
  using checker_t = logicker::core::old_checker::checker<puzzle_t>;

  //using base_input = logicker::core::input_node_base;
  //using int_input = logicker::core::int_input_node;
  using composite_input = logicker::core::composite_input_node;

  /*std::vector<int> vals {
    1,2,
    2,1};*/
  std::vector<int> vals {
    1,3,2,
    3,2,1,
    2,1,3};
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

  using solver_t = logicker::core::old_solver::solver<puzzle_t>;
  using solver_factory_t = logicker::core::old_solver::solver_factory<puzzle_t>;

  puzzle_t puzzle_inst2 = factory_t::create( input );
  solver_t solver = solver_factory_t().create_solver_for_assignment( std::move(puzzle_inst2) );
  std::cout << "Solver's solution:\n" << solver.get_solution();

  puzzle_t puzzle_inst = factory_t::create( input );
  checker_t checker{ std::move(puzzle_inst) };

  std::cout << "Solved by vals: " << std::boolalpha << checker.is_solved_by(vals) << '\n';

  puzzle_t::grid_type sol_grid = checker.get_assignment_grid();
  sol_grid.clear();
  sol_grid.set_values(vals);

  std::cout << "Solved by grid: " << std::boolalpha << checker.is_solved_by(sol_grid) <<'\n';

  using new_checker_t = logicker::core::checker::checker<latin_square>;
  new_checker_t checker3 = new_checker_t::create( input );
  typename new_checker_t::grid_t sol_grid3 = checker3.assignment_grid();
  sol_grid3.clear();
  sol_grid3.set_values(vals);

  std::cout << "New checker: solved by grid: " << std::boolalpha << checker3.is_solved_by( sol_grid3 ) << '\n';


  } catch (char const* e) {
    std::cout << e << '\n';
  } catch (std::string e) {
    std::cout << e << '\n';
  }
}
