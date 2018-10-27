#pragma once
#include "../../core/input_node.hpp"
#include "../../core/elements/value/boolean.hpp"

namespace logicker::puzzle::simples {
  //toto je libovolne (napr geometricke) Sudoku, ktere krome rows a cols
  //pouziva jenom regiony zadane silnymi carami v obrazci
  //je to mineno jako predek jednak pro SudokuClassic
  //a dvak pro ruzne jine varianty Sudoku, treba s extraregiony
  class Sudoku : public core::simple_puzzle<core::topology::rectangle, std::string, core::value::integer> {
      using Topology = core::topology::rectangle;//delete
      using ValueSlotName = std::string;//delete
      using ValueType = core::value::integer;//delete
    public:
      static Sudoku create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      Sudoku( int size, const RegionIds& region_ids, const Givens& givens );

      core::constraint::value_set<ValueType> value_set_;
      //Regions regions_;
      core::topology::coords_meta_group<Topology::coords> regions_;
  };

  Sudoku
  Sudoku::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;
    using vector_input_node = core::vector_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int size = cast_input.get_int( "Size" );
    const vector_input_node& regions_input = cast_input.get<vector_input_node>( "Regions" );
    auto region_ids = simple_puzzle::parse_region_ids( regions_input );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    auto givens = simple_puzzle::parse_givens( givens_input );
    return Sudoku( size, region_ids, givens );
  }

  Sudoku::Sudoku( int size, const RegionIds& region_ids, const Givens& givens ) : simple_puzzle{ core::topology::rectangle{ size, size }, givens }, value_set_{ ValueType::range( 1, size ) }, regions_{ topology_, region_ids } {}

  Sudoku::PuzzleInstance
  Sudoku::get_instance() {
    Grid grid_;
    Constraint completeness_constraint_;
    Constraint givens_constraint_;
    Constraint puzzle_constraint_;

    auto value_slot = core::constraint::value_slot<ValueType>{ value_set_ };
    //for ( auto slot : get_slot_ids_range( core::topology::CG_Centers, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers(), "Value" ) ) {
      grid_.add_value_slot( slot, value_slot );
    }

    //for ( auto slot : get_slot_ids_range_no_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter( no_given( "Value" )), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value_determined
        <ValueSlotId>( slot );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor, true );
      completeness_constraint_.add_constraint( constraint );
    }

    //for ( auto slot : get_slot_ids_range_with_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter( with_given( "Value" )), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value
        <ValueSlotId, ValueType>( slot );
      auto constraint = core::constraint::value::values_equal
        <ValueType, Grid>( extractor, givens_.get<ValueType>( slot ) );
      givens_constraint_.add_constraint( constraint );
    }

    /*for ( auto region : topology_.get_all_coords_groups( core::CMG_rows_cols ) ) {
      auto region_coords = topology_.get_coords_in_group( region );
      auto region_slot_ids = make_slot_ids( region_coords, "Value" );*/
    //for ( auto region_slot_ids : get_slot_ids_ranges( core::topology::CMG_rows_cols, { "Value" } ) ) {
    for ( auto region_slot_ids : SlotIdsMetaGroup( topology_.cmg_rows_cols(), "Value" ) ) {
      auto extractor = core::constraint::extractor::values_set::values_set
        <ValueSlotId, ValueType>( region_slot_ids.elems() );
      auto constraint = core::constraint::values_sets::values_sets_equal
        <ValueType, Grid>( extractor, value_set_ );
      puzzle_constraint_.add_constraint( constraint );
    }

    /*for ( auto region : regions_ ) {
      auto region_coords = *( region.second );
      auto region_slot_ids = make_slot_ids( region_coords, "Value" );*/
    //for ( auto region_slot_ids : get_slot_ids_ranges( regions_, { "Value" } ) ) {
    for ( auto region_slot_ids : SlotIdsMetaGroup( regions_, "Value" ) ) {
      auto extractor = core::constraint::extractor::values_set::values_set
        <ValueSlotId, ValueType>( region_slot_ids.elems() );
      auto constraint = core::constraint::values_sets::values_sets_equal
        <ValueType, Grid>( extractor, value_set_ );
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
