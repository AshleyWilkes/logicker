#pragma once
#include "../../core/input_node.hpp"
#include "../../core/elements/value/boolean.hpp"
#include "../../core/constraint/extractor/value/count_if.hpp"

namespace logicker::puzzle::simples {
  //uvahy na tema, jestli SolutionValueType ma byt boolean nebo optional<object>, maji, zda se, malou
  //uzitnou hodnotu
  class Mines : public core::puzzle<core::topology::rectangle, std::string, core::value::integer, core::value::boolean> {
    public:
      static Mines create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      Mines( int size, const Givens& givens );
  };

  Mines
  Mines::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int size = cast_input.get_int( "Size" );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    auto givens = puzzle::parse_givens( givens_input );
    return Mines( size, givens );
  }

  Mines::Mines( int size, const Givens& givens ) : puzzle{ core::topology::rectangle{ size, size }, givens } {}

  Mines::PuzzleInstance
  Mines::get_instance() {
    Grid grid_;
    Constraint completeness_constraint_;
    Constraint givens_constraint_;
    Constraint puzzle_constraint_;

    core::constraint::value_set<core::value::boolean> value_set = core::value::boolean::range();
    auto value_slot = core::constraint::value_slot<core::value::boolean>{ value_set };
    //for ( auto slot : get_slot_ids_range_no_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers(), "Value" ) ) {
      grid_.add_value_slot( slot, value_slot );
    }

    //for ( auto slot : get_slot_ids_range_no_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter( no_given( "Value" )), "Value") ) {
      auto extractor = core::constraint::extractor::value::slot_value_determined
        <ValueSlotId>( slot );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor, true );
      completeness_constraint_.add_constraint( constraint );
    }

    //for ( auto slot : get_slot_ids_range_with_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    //for ( auto slot : SlotIdsGroup( cg_centers().apply_filter( with_given( "Value" )), "Value") ) {
      //auto neighs_no_given = get_slot_ids_range( core::topology::CG_Centers, { "Value" } );
      //auto neighs_no_given = SlotIdsGroup( cg_centers(), "Value" ).elems();
      //
    //!!!!!!neco je shnileho: pokud nasledujici radek vynecham a filter strcim primo do 
    //prave strany for cyklu, dela to podivne veci!!!!!!!
    //(jmenovite coordsy neprochazeji spravne inicializovanou cgroupu)
    auto centers_with_given = cg_centers().apply_filter( with_given( "Value" ));
    for ( auto coords : centers_with_given ) {
      auto cg_neighs_no_given = topology_.cg_neighs( coords ).apply_filter( no_given( "Value" ));
      SlotIdsGroup neighs_no_given{ cg_neighs_no_given, "Value" };
      auto is_true = []( const core::constraint::grid_slot& slot ) {
        auto cast_slot = dynamic_cast<const core::constraint::value_slot<core::value::boolean>&>( slot );
        return cast_slot.get_decided_value().get().get();
      };
      auto extractor = core::constraint::extractor::value::count_if
        <ValueSlotId, core::value::integer, decltype( is_true )>( neighs_no_given.elems(), is_true );
      auto constraint = core::constraint::value::values_equal
        <core::value::integer, Grid>( extractor, givens_.get<core::value::integer>({ coords, "Value" }) );
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
