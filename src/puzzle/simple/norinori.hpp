#pragma once
#include "../../core/input_node.hpp"
#include "../../core/constraint/extractor/value/count_if.hpp"
#include "../../core/constraint/constraint/composite/disjunction.hpp"

namespace logicker::puzzle::simples {
  class Norinori : public core::simple_puzzle<core::topology::rectangle, std::string, core::value::boolean> {
    public:
      static Norinori create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      Norinori( int width, int height, const RegionIds& region_ids );
      core::topology::coords_meta_group<core::topology::rectangle::coords> regions_;
  };

  Norinori
  Norinori::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;
    using vector_input_node = core::vector_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int width = cast_input.get_int( "Width" );
    int height = cast_input.get_int( "Height" );
    const vector_input_node& regions_input = cast_input.get<vector_input_node>( "Regions" );
    auto region_ids = simple_puzzle::parse_region_ids( regions_input );
    return Norinori( width, height, region_ids );
  }

  Norinori::Norinori( int width, int height, const RegionIds& region_ids ) : simple_puzzle{ core::topology::rectangle{ width, height }}, regions_{ topology_, region_ids } {}

  Norinori::PuzzleInstance
  Norinori::get_instance() {
    Grid grid_;
    Constraint completeness_constraint_;
    Constraint givens_constraint_;
    Constraint puzzle_constraint_;

    core::constraint::value_set<core::value::boolean> value_set = core::value::boolean::range();
    auto value_slot = core::constraint::value_slot<core::value::boolean>{ value_set };

    for ( auto slot : SlotIdsGroup( cg_centers(), "Value" ) ) {
      grid_.add_value_slot( slot, value_slot );
    }

    for ( auto slot : SlotIdsGroup( cg_centers(), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value_determined
        <ValueSlotId>( slot );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor, true );
      completeness_constraint_.add_constraint( constraint );
    }

    auto is_true = []( const core::constraint::value_slot<core::value::boolean>& slot ) {
      return slot.get_decided_value().get().get();
    };

    //v kazdym regionu jsou prave 2 policka true
    for ( auto region_slot_ids : SlotIdsMetaGroup( topology_.cmg_rows_cols(), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::count_if
        <ValueSlotId, core::value::boolean, core::value::integer>( region_slot_ids.elems(), is_true );
      auto constraint = core::constraint::value::values_equal
        <core::value::integer, Grid>( extractor, 2 );
      puzzle_constraint_.add_constraint( constraint );
    }

    //kazde true policko ma prave jednoho border souseda, kterej je taky true
    //jinymi slovy pro kazde policko plati disjunkce !is_true || count == 1
    for ( auto center : SlotIdsGroup( cg_centers(), "Value" ) ) {
      core::constraint::composite::disjunction<Grid> constraint;
      //vytvorit !is_true
      auto true_extractor = core::constraint::extractor::value::slot_value
        <ValueSlotId, core::value::boolean>( center );
      auto true_constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( true_extractor, false );
      constraint.add_constraint( true_constraint );
      //vytvorit count == 1
      auto neighs = SlotIdsGroup( topology_.cg_neighs( center.first, false ), "Value" );
      auto true_neighs_extractor = core::constraint::extractor::value::count_if
        <ValueSlotId, core::value::boolean, core::value::integer>( neighs.elems(), is_true );
      auto true_neighs_constraint = core::constraint::value::values_equal
        <core::value::integer, Grid>( true_neighs_extractor, 1 );
      constraint.add_constraint( true_neighs_constraint );
      //udelat disjunkci
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
