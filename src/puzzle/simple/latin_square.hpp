#pragma once
#include "../../core/input_node.hpp"

namespace logicker::puzzle::simples {
  class LatinSquare : public core::simple_puzzle<core::topology::rectangle, std::string, core::value::integer> {
      using Topology = core::topology::rectangle;//delete
      using ValueSlotName = std::string;//delete
      using ValueType = core::value::integer;//delete
    public:
      static LatinSquare create_instance( const core::input_node_base& input );
      PuzzleInstance get_instance();//temp; delete
    private:
      LatinSquare( int size, const Givens& givens );

      core::constraint::value_set<ValueType> value_set_;
  };

  //co se tyce zpracovavani vstupu, jak pisu v commentu nekde kdovikde, probiha to
  //ve 3 fazich (popisuju Givens jako nejslozitejsi cast, ale ostatnich vstupnich
  //hodnot se to tyka taky):
  //a) syntakticka analyza, tu provadi metoda parse_givens a vystupem je objekt Givens
  //b) semanticka analyza, tu provadi konstruktor simple_puzzlu, jejim ukolem je
  //  provest sanity check, tj. vyhodit vyjimku, pokud data z bodu a) nejsou vnitrne
  //  konzistentni
  //c) zpracovani dat, to provadi metoda get_instance, jde o pouziti (nyni jiz overenych)
  //  vstupnich dat k vytvoreni objektu puzzle_instance, obsahujiciho vnitrni reprezentaci
  //  dane uloh; spravne by implementace mela byt rozdelena do dvou kroku, jeden
  //  by zavolal virtualni v LatinSquaru implementovanou metodu ??co-ja-vim??,
  //  a druhy by overil, ze v givens_ neni zadna zadana hodnota, ktera se neda zapsat
  //  do instance.empty_gridu_. Toto by tvorilo druhou cast sanity checku
  LatinSquare
  LatinSquare::create_instance( const core::input_node_base& input ) {
    using composite_input_node = core::composite_input_node;

    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    int size = cast_input.get_int( "Size" );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    auto givens = simple_puzzle::parse_givens( givens_input );
    return LatinSquare( size, givens );
  }

  LatinSquare::LatinSquare( int size, const Givens& givens ) : simple_puzzle{ core::topology::rectangle{ size, size }, givens }, value_set_{ ValueType::range( 1, size ) } {}

  LatinSquare::PuzzleInstance
  LatinSquare::get_instance() {
    Grid grid_;
    Constraint completeness_constraint_;
    Constraint givens_constraint_;
    Constraint puzzle_constraint_;
    //vnitrek teto metody by imho mel vypadat jinak:
    //completeness a givens, takto implementovane,
    //jsou zcela standardni a mel by existovat zpusob, jak tyto
    //implementace snadno znovupouzit
    //
    //mno tedy init_grid potrebuje od LatinSquaru parametry:
    //  core::CG_Centers -- NE-E, tohle ma byt iterator nad valueSlotIds
    //      takze se to da pretizit, aby to fungovalo pro CMGroup i CGroup
    //      dale budu potrebovat overload, ktery rekne givens, a overload
    //      pro non-givens
    //  "Value" 
    //  value_set_
    //
    //OK, zacneme Topologii rectangle (nikoli compositni) a ulohou Tapa:
    //  coordsy poskytuje topologie
    //  valueSlotNames poskytuje SimplePuzzle
    //    Tapa obnasi, ze policko (Coords) bud je Given,
    //      pak obsahuje nekolik zadanych intu
    //        a takove policko nema v gridu zadny slot
    //        jeho obsah je soucasti nejake ValueSetsEqual constrainty
    //    nebo neni Given
    //      pak obsahuje jeden slot s (ValueType = bool)
    /*for ( auto coords : topology_.get_coords_in_group( core::CG_Centers ) ) {
      grid_.add_value_slot( std::make_pair( coords, "Value" ), core::constraint::value_slot<ValueType>{ value_set_ });
    }*/

    auto value_slot = core::constraint::value_slot<ValueType>{ value_set_ };
    for ( auto slot : SlotIdsGroup( cg_centers(), "Value" ) ) {
    //for ( auto slot : get_slot_ids_range( core::topology::CG_Centers, { "Value" } ) ) {
      grid_.add_value_slot( slot, value_slot );
    }

    //completeness_constraint potrebuje parametry:
    //  core::CG_Centers -- NE-E, viz vyse
    //  "Value"
    /*for ( auto coords : topology_.get_coords_in_group( core::CG_Centers ) ) {
      auto extractor = core::constraint::extractor::value::slot_value_determined
        <ValueSlotId>( std::make_pair( coords, "Value" ));
      auto constraint = core::constraint::value::values_equal
        <bool, Grid>( extractor, true );
      completeness_constraint_.add_constraint( constraint );
    }*/

    //for ( auto slot : get_slot_ids_range_no_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter(no_given( "Value" )), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value_determined
        <ValueSlotId>( slot );
      auto constraint = core::constraint::value::values_equal
        <core::value::boolean, Grid>( extractor, true );
      completeness_constraint_.add_constraint( constraint );
    }

    //givens_constraint potrebuje parametry:
    //  iterator pre coordsy, pro ktere exje given
    //  "Value" -- ale tohle je taky zjednodusujici
    //  jak ziskat hodnotu, ktera odpovida givenu pro dane coordsy
    //soucasna implementace je zalozena na predpokladu, ze
    //  givens_map je std::map<Coords, Value>, zrovna jako implementace
    //  metody parse_givens. Je vylozene otazka, jestli to ma cenu
    //  nejak predelavat; jednoznacne ma takova predelavka velmi
    //  nizkou prioritu, dokud se nezjisti, ze je k necemu potreba.
    //
    //nicmene je zde problemek: soucasti rozparsovanych givensu neni
    //  a mel by byt ValueSlotName, protoze givensy mohou se snadno
    //  tykat vice nez jednoho ValueSlotNamu, cili?
    //
    //Cili givens_ musi obsahovat i informaci o SlotName, to se zda jasny.
    //  takze to ma byt std::map<SlotValueId, Value>, takze parse_givens
    //  ma vypadat ponekud jinak nez vypada.
    //
    //Jinak stale plati, ze to, ze je to natvrdo mapa a ze tahle
    //  implementace s tim pocita, je v poradku a predelavani ma
    //  nizkou prioritu.
    /*for ( auto given : givens_ ) {
      auto extractor = core::constraint::extractor::value::slot_value
        <ValueSlotId, ValueType>( std::make_pair( given.first, "Value" ));
      auto constraint = core::constraint::value::values_equal
        <ValueType, Grid>( extractor, given.second );
      givens_constraint_.add_constraint( constraint );
    }*/

    //for ( auto slot : get_slot_ids_range_with_given( core::topology::CG_Centers, { "Value" }, { "Value" } ) ) {
    for ( auto slot : SlotIdsGroup( cg_centers().apply_filter(with_given( "Value" )), "Value" ) ) {
      auto extractor = core::constraint::extractor::value::slot_value
        <ValueSlotId, ValueType>( slot );
      auto constraint = core::constraint::value::values_equal
        <ValueType, Grid>( extractor, givens_.get<ValueType>( slot ) );
      givens_constraint_.add_constraint( constraint );
    }

    //mno, tohle bych chtel asi vyhledove udelat nejak tak,
    //  aby se to dalo konfigurovat lidsky citelnym
    //  textovym souborem. To je hudba budoucnosti,
    //  proted je treba nechat to jako virtualni metodu
    //  implementovanou v potomku (naor. v LatinSquaru)
    //
    //Nicmene zda se, ze by tato metoda mohla vracet iterator
    //  pres jednotlive subpodminky a ne rovnou jejich conjunction
    /*for ( auto region : topology_.get_all_coords_groups( core::CMG_rows_cols ) ) {
      auto region_coords = topology_.get_coords_in_group( region );
      auto region_slot_ids = make_slot_ids( region_coords, "Value" );*/
    //for ( auto region_slot_ids : get_slot_ids_ranges( core::topology::CMG_rows_cols, { "Value" } ) ) {
    for (auto region_slot_ids : SlotIdsMetaGroup( topology_.cmg_rows_cols(), "Value" ) ) {
      auto extractor = core::constraint::extractor::values_set::values_set
        <ValueSlotId, ValueType>( region_slot_ids.elems() );
      auto constraint = core::constraint::values_sets::values_sets_equal
        <ValueType, Grid>( extractor, value_set_ );
      puzzle_constraint_.add_constraint( constraint );
    }

    return { grid_, topology_, completeness_constraint_, givens_constraint_, puzzle_constraint_ };
  }
}
