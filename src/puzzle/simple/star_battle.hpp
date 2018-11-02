#pragma once
#include "../../core/input_node.hpp"
#include "../../core/constraint/constraint/value/not_both_true.hpp"

namespace logicker::puzzle::simples {
  class StarBattle : public core::simple_puzzle<core::topology::square, std::string, core::value::boolean> {
    public:
      static StarBattle create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      StarBattle( int size, int stars, const RegionIds& region_ids );
      core::topology::coords_meta_group<core::topology::rectangle::coords> regions_;
      int stars_;
  };

  StarBattle
  StarBattle::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;
    using vector_input_node = core::vector_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int size = cast_input.get_int( "Size" );
    int stars = cast_input.get_int( "StarsCount" );
    const vector_input_node& regions_input = cast_input.get<vector_input_node>( "Regions" );
    auto region_ids = simple_puzzle::parse_region_ids( regions_input );
    return StarBattle( size, stars, region_ids );
  }

  StarBattle::StarBattle( int size, int stars, const RegionIds& region_ids ) : simple_puzzle{ core::topology::square{ size }}, regions_{ topology_, region_ids }, stars_{ stars } {}

  StarBattle::PuzzleInstance
  StarBattle::get_instance() {
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

    //!!!!!!rows_cols a regions maji stejna tela; pripsat mergovani vice
    //CoordsGroups do jedne a pomoci teto schoponosti jeden z nasledujicich
    //for-cyklu zrusit!!!!!!!
    for ( auto region_slot_ids : SlotIdsMetaGroup( topology_.cmg_rows_cols(), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::count_if
        <ValueSlotId, core::value::boolean, core::value::integer>( region_slot_ids.elems(), is_true );
      auto constraint = core::constraint::value::values_equal
        <core::value::integer, Grid>( extractor, core::value::integer{ stars_ } );
      puzzle_constraint_.add_constraint( constraint );
    }

    for ( auto region_slot_ids : SlotIdsMetaGroup( regions_, "Value" ) ) {
      auto extractor = core::constraint::extractor::value::count_if
        <ValueSlotId, core::value::boolean, core::value::integer>( region_slot_ids.elems(), is_true );
      auto constraint = core::constraint::value::values_equal
        <core::value::integer, Grid>( extractor, core::value::integer{ stars_ } );
      puzzle_constraint_.add_constraint( constraint );
    }

    //zadne dve hvezdy se nedotykaji ani rohem
    for ( auto neigh_pair : SlotIdsMetaGroup( topology_.cmg_neigh_pairs(), "Value" ) ) {
      auto slots = neigh_pair.elems();
      auto slots_it = slots.begin();
      auto extractor1 = core::constraint::extractor::value::slot_value
        <ValueSlotId, core::value::boolean>( *slots_it );
      ++slots_it;
      auto extractor2 = core::constraint::extractor::value::slot_value
        <ValueSlotId, core::value::boolean>( *slots_it );
      auto constraint = core::constraint::value::not_both_true
        <Grid>( extractor1, extractor2 );
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
