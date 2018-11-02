#pragma once
#include "sudoku.hpp"

namespace logicker::puzzle::simples {
  class SudokuClassic : public Sudoku {
    public:
      static RegionIds classic_regions;
      static SudokuClassic create_instance( const core::input_node_base& input );
    private:
      SudokuClassic( const Givens& region_ids );
  };

  Sudoku::RegionIds SudokuClassic::classic_regions {
    "0", "0", "0", "1", "1", "1", "2", "2", "2",
    "0", "0", "0", "1", "1", "1", "2", "2", "2",
    "0", "0", "0", "1", "1", "1", "2", "2", "2",
    "3", "3", "3", "4", "4", "4", "5", "5", "5",
    "3", "3", "3", "4", "4", "4", "5", "5", "5",
    "3", "3", "3", "4", "4", "4", "5", "5", "5",
    "6", "6", "6", "7", "7", "7", "8", "8", "8",
    "6", "6", "6", "7", "7", "7", "8", "8", "8",
    "6", "6", "6", "7", "7", "7", "8", "8", "8"
  };

  SudokuClassic
  SudokuClassic::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    auto givens = simple_puzzle::parse_givens( givens_input );
    return SudokuClassic( givens );
  }

  SudokuClassic::SudokuClassic( const Givens& givens ) : Sudoku{ 9, classic_regions, givens } {}
}
