#pragma once
#include "../../core/input_node.hpp"
#include "../../core/constraint/extractor/value/slot_values_equal.hpp"

namespace logicker::puzzle::simples {
  //cistsi by bylo nemit zde value::integer, ale value::elements; ??mozna by se to hodilo i
  //pro renderovani??; ale na litacku dobry i s value::integer
  class Elements : public core::simple_puzzle<core::topology::rectangle, std::string, core::value::integer> {
    public:
      static Elements create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      Elements( int width, int height, const RegionIds& region_ids, const Givens& givens );
      core::constraint::value_set<core::value::integer> value_set_;
      core::topology::coords_meta_group<core::topology::rectangle::coords> regions_;
  };

  Elements
  Elements::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;
    using vector_input_node = core::vector_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int width = cast_input.get_int( "Width" );
    int height = cast_input.get_int( "Height" );
    const vector_input_node& regions_input = cast_input.get<vector_input_node>( "Regions" );
    auto region_ids = simple_puzzle::parse_region_ids( regions_input );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    auto givens = simple_puzzle::parse_givens( givens_input );
    return Elements( width, height, region_ids, givens );
  }

  Elements::Elements( int width, int height, const RegionIds& region_ids, const Givens& givens ) : simple_puzzle{ core::topology::rectangle{ width, height }, givens}, value_set_{ core::value::integer::range( 0, 3 ) }, regions_{ topology_, region_ids } {}
  
  Elements::PuzzleInstance
  Elements::get_instance() {
    Grid grid_;
    Constraint completeness_constraint_;
    Constraint givens_constraint_;
    Constraint puzzle_constraint_;

    auto value_slot = core::constraint::value_slot<core::value::integer>{ value_set_ };
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
        <ValueSlotId, core::value::integer>( slot );
      auto constraint = core::constraint::value::values_equal
        <core::value::integer, Grid>( extractor, givens_.get<core::value::integer>( slot ) );
      givens_constraint_.add_constraint( constraint );
    }

    //v kazdem regionu maji vsechna policka stejnou hodnotu
    //sousedni policka z ruznych regionu maji ruznou hodnotu
    //
    //jinymi slovy sousedni policka maji stejnou hodnotu prave kdyz
    //jsou ve stejnem regionu
    //
    //takze chci jednak do kazdyho policka umistit slot s jedinou hodnotou
    //rovnou id regionu, do nehoz policko patri
    std::map<std::string, int> map;
    for ( auto region : regions_ ) {
      if ( map.find( region.id() ) == map.end() ) {
        map.insert({ region.id(), map.size() });
      }
      int index = map.find( region.id() )->second;
      auto region_set_ = core::value::integer::given( index );
      auto region_slot = core::constraint::value_slot<core::value::integer>{ region_set_ };
      for ( auto slot : SlotIdsGroup( region, "Region" ) ) {
        grid_.add_value_slot( slot, region_slot );
      }
    }

    //jednak udelat nad kazdou dvojici policek "kombinovane"
    //extractory, ktere extractnou, jestli se ta nebo ta dvojice
    //hodnot list nebo ne
    //a conditionu pak mit, ze tyhle dva kombinovane extractory
    //vratily stejnou hodnotu
    for ( auto neigh_pair_coords : topology_.cmg_neigh_pairs() ) {
      auto extractor1 = core::constraint::extractor::value::slot_values_equal
        <ValueSlotId, core::value::integer>( SlotIdsGroup( neigh_pair_coords, "Value").elems() );
      auto extractor2 = core::constraint::extractor::value::slot_values_equal
        <ValueSlotId, core::value::integer>( SlotIdsGroup( neigh_pair_coords, "Region").elems() );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor1, extractor2 );
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
