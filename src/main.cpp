#include "puzzle/simple_puzzles.hpp"
#include "puzzle/simple/latin_square.hpp"
#include "puzzle/simple/mines.hpp"
#include "puzzle/simple/sudoku.hpp"
#include "checker/even_newer.hpp"
//#include "puzzle/latin_square.hpp"
#include "core/input_node.hpp"
/*#include "core/puzzle_instance_v1.hpp"
#include "core/puzzle_instance_factory.hpp"
#include "checker/checker_old.hpp"
#include "checker/checker.hpp"
#include "solver/solver_old.hpp"*/
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <map>
#include <memory>
#include <streambuf>
#include <string>
#include <vector>

namespace logicker::run {
  const std::string path_to_puzzles_dir = "../puzzles/";

  using input_t = logicker::core::input_node_base;
  using composite_input_t = logicker::core::composite_input_node;

  template<typename PuzzleType>
  using even_newer_checker_t = logicker::checker::checker<PuzzleType>;

  composite_input_t read_input_file( const std::string& path_to_input ) {
    std::ifstream is{ path_to_input };
    std::string input_str{ std::istreambuf_iterator<char>( is ),
      std::istreambuf_iterator<char>() };

    Json::Value input_json_value;
    Json::Reader reader;
    if (! reader.parse(input_str, input_json_value, false)) {
      throw "parsing json error: probably malformed syntax (" + path_to_input +")\n";
    }

    return { "Input", input_json_value };
  }

  //??vazne potrebuju ten druhej argument, nestacilo by proste
  //mit na disku vedle sebe soubor s assign a soubor se solution
  //a sem predavat pouze cestu k adresari, v nemz zminene soubory jsou??
  template<typename PuzzleType, typename SolutionInputType>
  void check_puzzle( const std::string& path_to_assignment, const std::vector<SolutionInputType>& solution_vals ) {
    std::cout << "Check '" << path_to_assignment << "' via check_puzzle()\n";
    using PuzzleInstance = typename PuzzleType::PuzzleInstance;

    std::string assignment_path = path_to_puzzles_dir + path_to_assignment;

    auto input = read_input_file( assignment_path );
    PuzzleType puzzle = PuzzleType::create_instance( input );
    PuzzleInstance puzzle_instance = puzzle.get_instance();
    even_newer_checker_t<PuzzleInstance> checker{ puzzle_instance };
    std::vector<typename PuzzleType::SolutionValType> cast_vals;
    for ( SolutionInputType val : solution_vals ) {
      cast_vals.push_back( val );
    }
    //auto solution_grid{ puzzle_instance.create_solution( cast_vals )};
    auto solution_grid{ puzzle_instance.create_solution( cast_vals )};
    std::cout << "Checker via check_puzzle: " << std::boolalpha << checker.check( solution_grid ) << '\n';
  }
}

int main() {
  try {
  std::cout << "This is Logicker!\n";
  /*std::ifstream is{ "latin_square3x3.lgc" };
  std::string input_str{ std::istreambuf_iterator<char>( is ),
    std::istreambuf_iterator<char>() };

  Json::Value input_json_value;
  Json::Reader reader;
  if (! reader.parse(input_str, input_json_value, false)) {
    throw "parsing json error: probably malformed syntax\n";
  }

  using composite_input = logicker::core::composite_input_node;
  composite_input input{ "Input", input_json_value };*/

  /*std::vector<int> vals {
    1,2,
    2,1};*/
  std::vector<int> latin_square3x3_vals {
    1,3,2,
    3,2,1,
    2,1,3
  };
  /*std::vector<int> vals {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};*/

  std::vector<int> sudoku_class_vals {
    7,5,6,3,4,2,1,9,8,
    1,4,8,9,2,5,7,3,6,
    6,7,9,8,5,3,2,1,4,
    5,8,3,1,9,7,6,4,2,
    3,6,2,5,1,8,4,7,9,
    8,9,4,2,6,1,3,5,7,
    2,1,7,6,3,4,9,8,5,
    4,2,1,7,8,9,5,6,3,
    9,3,5,4,7,6,8,2,1
  };

  std::vector<bool> mines_vals {
    0,1,0,0,
    1,0,1,0,
    0,0,1,1,
    0,0,0,0
  };

  /*std::cout << "Simple Puzzles: \n";
  LatinSquare latin_square_instance = LatinSquare::create_instance( input );
  LatinSquare::PuzzleInstance puzzle_instance = latin_square_instance.get_instance();
  using even_newer_checker_t = logicker::checker::checker<LatinSquare::PuzzleInstance>;
  even_newer_checker_t checker4{ puzzle_instance };
  std::vector<logicker::core::value::integer> cast_vals;
  for ( int val : latin_square3x3_vals ){
    cast_vals.push_back( val );
  }
  auto sol_grid4{ puzzle_instance.create_solution( cast_vals )};
  std::cout << "Checker: " << std::boolalpha << checker4.check( sol_grid4 ) << '\n';*/

  using LatinSquare = logicker::puzzle::simples::LatinSquare;
  logicker::run::check_puzzle<LatinSquare>( "latin_square3x3.lgc", latin_square3x3_vals );

  using Sudoku = logicker::puzzle::simples::Sudoku;
  logicker::run::check_puzzle<Sudoku>( "sudoku_classic.lgc", sudoku_class_vals );
  
  using Mines = logicker::puzzle::simples::Mines;
  logicker::run::check_puzzle<Mines>( "mines.lgc", mines_vals );
  /*using logicker::puzzle::latin_square;
  using logicker::core::puzzle_instance_v1;
  using factory_t = logicker::core::puzzle_instance_factory<latin_square>;
  using puzzle_t = puzzle_instance_v1<latin_square>;
  using checker_t = logicker::core::old_checker::checker<puzzle_t>;

  //using base_input = logicker::core::input_node_base;
  //using int_input = logicker::core::int_input_node;*/
  /*using solver_t = logicker::core::old_solver::solver<puzzle_t>;
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

  std::cout << "Solved by grid: " << std::boolalpha << checker.is_solved_by(sol_grid) << '\n';

  using new_checker_t = logicker::core::checker::checker<latin_square>;
  new_checker_t checker3 = new_checker_t::create( input );
  typename new_checker_t::grid_t sol_grid3 = checker3.assignment_grid();
  sol_grid3.clear();
  sol_grid3.set_values(vals);

  std::cout << "New checker: solved by grid: " << std::boolalpha << checker3.is_solved_by( sol_grid3 ) << '\n';*/
  } catch (char const* e) {
    std::cout << e << '\n';
  } catch (std::string e) {
    std::cout << e << '\n';
  }
}
