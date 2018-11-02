#pragma once
#include "../../core/input_node.hpp"
#include "../../core/constraint/extractor/value/slot_values_equal.hpp"

namespace logicker::puzzle::simples {
  class Staircases : public core::simple_puzzle<core::topology::rectangle, std::string, core::value::integer> {
      using ValueType = core::value::integer;
    public:
      static Staircases create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      Staircases( int size, const RegionIds& region_ids, const Givens& givens );
      core::constraint::value_set<core::value::integer> value_set_;
      core::topology::coords_meta_group<core::topology::rectangle::coords> regions_;
  };

  Staircases
  Staircases::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;
    using vector_input_node = core::vector_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int size = cast_input.get_int( "Size");
    const vector_input_node& regions_input = cast_input.get<vector_input_node>( "Regions" );
    auto region_ids = simple_puzzle::parse_region_ids( regions_input );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    auto givens = simple_puzzle::parse_givens( givens_input );
    return Staircases( size, region_ids, givens );
  }

  Staircases::Staircases( int size, const RegionIds& region_ids, const Givens& givens ) : simple_puzzle{ core::topology::rectangle{ size, size }, givens }, value_set_{ core::value::integer::range( 1, size ) }, regions_{ topology_, region_ids } {}

  Staircases::PuzzleInstance
  Staircases::get_instance() {
    Grid grid_;
    Constraint completeness_constraint_;
    Constraint givens_constraint_;
    Constraint puzzle_constraint_;

    auto value_slot = core::constraint::value_slot<ValueType>{ value_set_ };
    for ( auto slot : SlotIdsGroup( cg_centers(), "Value" ) ) {
      grid_.add_value_slot( slot, value_slot );
    }

    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter( no_given( "Value" )), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value_determined
        <ValueSlotId>( slot );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor, true );
      completeness_constraint_.add_constraint( constraint );
    }

    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter( with_given( "Value" )), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value
        <ValueSlotId, ValueType>( slot );
      auto constraint = core::constraint::value::values_equal
        <ValueType, Grid>( extractor, givens_.get<ValueType>( slot ) );
      givens_constraint_.add_constraint( constraint );
    }

    //radky a sloupce obsahuji kompletni sestavu
    for ( auto region_slot_ids : SlotIdsMetaGroup( topology_.cmg_rows_cols(), "Value" ) ) {
      auto extractor = core::constraint::extractor::values_set::values_set
        <ValueSlotId, ValueType>( region_slot_ids.elems() );
      auto constraint = core::constraint::values_sets::values_sets_equal
        <ValueType, Grid>( extractor, value_set_ );
      puzzle_constraint_.add_constraint( constraint );
    }

    //regiony obsahuji jedinou hodnotu
    for ( auto region_slot_ids : SlotIdsMetaGroup( regions_, "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_values_equal
        <ValueSlotId, ValueType>( region_slot_ids.elems() );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor, true );
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
