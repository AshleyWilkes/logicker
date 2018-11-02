#include "puzzle/simple_puzzles.hpp"
#include "puzzle/simple/magnets.hpp"
#include "puzzle/simple/norinori.hpp"
#include "puzzle/simple/staircases.hpp"
#include "puzzle/simple/elements.hpp"
#include "puzzle/simple/latin_square.hpp"
#include "puzzle/simple/mines.hpp"
#include "puzzle/simple/sudoku.hpp"
#include "puzzle/simple/sudoku_classic.hpp"
#include "puzzle/simple/star_battle.hpp"
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

  std::vector<int> sudoku_vals {
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

  std::vector<int> sudoku_classic_vals {
    6,8,7,9,5,3,4,1,2,
    4,1,5,2,7,8,3,6,9,
    9,3,2,6,4,1,5,7,8,
    3,7,8,1,9,2,6,5,4,
    2,4,6,3,8,5,7,9,1,
    1,5,9,7,6,4,2,8,3,
    5,2,4,8,1,7,9,3,6,
    7,6,1,4,3,9,8,2,5,
    8,9,3,5,2,6,1,4,7
  };

  std::vector<bool> mines_vals {
    0,1,0,0,
    1,0,1,0,
    0,0,1,1,
    0,0,0,0
  };

  std::vector<bool> star_battle_vals {
    0,0,1,0,0,0,0,
    1,0,0,0,0,0,0,
    0,0,0,1,0,0,0,
    0,0,0,0,0,0,1,
    0,0,0,0,1,0,0,
    0,1,0,0,0,0,0,
    0,0,0,0,0,1,0
  };

  std::vector<int> elements_vals {
    3,3,0,2,
    1,2,1,3,
    0,3,0,2,
    1,2,0,3
  };

  std::vector<int> staircases_vals {
    3,4,1,2,5,6,
    1,3,2,6,4,5,
    4,2,6,5,3,1,
    6,5,3,4,1,2,
    2,1,5,3,6,4,
    5,6,4,1,2,3
  };

  std::vector<bool> norinori_vals {
    0,1,1,0,
    1,0,0,1,
    1,0,0,1,
    0,1,1,0
  };

  std::vector<int> magnets_vals {
     1,-1, 1,-1, 1, 0,
     0, 0, 0, 0,-1, 0,
     0, 0, 0, 0, 1,-1,
     1,-1, 1, 0 ,0 ,0,
    -1, 0,-1, 0, 0, 0,
     1, 0, 0, 0, 1,-1
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
  logicker::run::check_puzzle<Sudoku>( "sudoku.lgc", sudoku_vals );

  using SudokuClassic = logicker::puzzle::simples::SudokuClassic;
  logicker::run::check_puzzle<SudokuClassic>( "sudoku_classic.lgc", sudoku_classic_vals );
  
  using Mines = logicker::puzzle::simples::Mines;
  logicker::run::check_puzzle<Mines>( "mines.lgc", mines_vals );

  using StarBattle = logicker::puzzle::simples::StarBattle;
  logicker::run::check_puzzle<StarBattle>( "star_battle.lgc", star_battle_vals );

  using Elements = logicker::puzzle::simples::Elements;
  logicker::run::check_puzzle<Elements>( "elements.lgc", elements_vals );

  using Staircases = logicker::puzzle::simples::Staircases;
  logicker::run::check_puzzle<Staircases>( "staircases.lgc", staircases_vals );

  using Norinori = logicker::puzzle::simples::Norinori;
  logicker::run::check_puzzle<Norinori>( "norinori.lgc", norinori_vals );

  //using Magnets = logicker::puzzle::simples::Magnets;
  //logicker::run::better_check_puzzle_file<Magnets>( "magnets.lgc" );

  } catch (char const* e) {
    std::cout << e << '\n';
  } catch (std::string e) {
    std::cout << e << '\n';
  }
}
