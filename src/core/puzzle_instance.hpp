#pragma once
#include "elements/topology/rectangle.hpp"
#include "constraint/constraint/composite/conjunction.hpp"
#include "constraint/grid.hpp"
#include "constraint/value_slot.hpp"
#include "../checker/even_newer.hpp"

namespace logicker::core {
  template<typename Grid, typename Constraint, typename Topology, typename ValueType>
  class puzzle_instance {
    public:
      using grid = Grid;//potrebuju pro checker; hnus
      puzzle_instance(
          const Grid& empty_grid, 
          const Topology& topology,
          const Constraint& completeness_constraint,
          const Constraint& givens_constraint,
          const Constraint& puzzle_constraint);
      Grid create_solution( const std::vector<ValueType>& solution_values ) const;

      friend checker::checker<puzzle_instance>;
    private:
      const Grid empty_grid_;
      const Topology topology_;
      //conditiona: popis ma byt tvrzeni typu
      //a) 2 hodnoty se rovnaji
      //b) 2 hodnoty se nerovnaji
      //c) 2 hodnoty splnuji nejaky jiny predikat
      //
      //A) 2 mnoziny hodnot se rovnaji
      //B) 1 mnozina hodnot je podmnozinou jine
      //
      //krom typu tvrzeni je treba jeste specifikovat
      //objekty (hodnoty, mnoziny hodnot), kterych se
      //conditiona tyka
      //
      //mno, a hodnoty jsou uz specifikovane temi coordsy
      //nebo muze byt hodnota specifikovana jako count_if,
      //tj. mnozinou coordsu a predikatem nad hodnotou
      //mnozina hodnot muze byt specifikovana jako
      //collector nad coordsy
      //
      //nebo muze byt hodnota, resp mnozina zadana vyctem
      const Constraint completeness_constraint_;
      const Constraint givens_constraint_;
      const Constraint puzzle_constraint_;
  };

  template<typename Grid, typename Constraint, typename Topology, typename ValueType>
  puzzle_instance<Grid, Constraint, Topology, ValueType>::puzzle_instance(
      const Grid& empty_grid,
      const Topology& topology,
      const Constraint& completeness_constraint,
      const Constraint& givens_constraint,
      const Constraint& puzzle_constraint) : 
    empty_grid_{ empty_grid },
    topology_{ topology },
    completeness_constraint_{ completeness_constraint },
    givens_constraint_{ givens_constraint },
    puzzle_constraint_{ puzzle_constraint } {}

  template<typename Grid, typename Constraint, typename Topology, typename ValueType>
  Grid
  puzzle_instance<Grid, Constraint, Topology, ValueType>::create_solution( const std::vector<ValueType>& solution_values ) const {
    //zkopirovat empty_grid do result
    Grid result{ empty_grid_ };
    auto vals_it = solution_values.cbegin();
    //prochazet coordsy, viz create_instance, do kazdeho policka umistit prislusnou hodnotu
    for ( auto coords : topology_.get_coords_in_group( topology::CG_Centers ) ) {
      typename Grid::ValSlotId slot_id{ coords, "Value" };
      if ( result.has( slot_id ) ) {
        auto& slot = result.template get_slot<ValueType>( slot_id );
        slot.decide_on( *vals_it );
      }
      ++vals_it;
    }
    return result;
  }
}
