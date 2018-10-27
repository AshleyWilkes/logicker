#pragma once
#include "constraint/givens.hpp"
#include "constraint/regions.hpp"
#include "elements/topology/rectangle.hpp"

namespace logicker::core {
  template<typename Topology, typename ValueSlotName, template <typename...> typename Container = std::vector>
  class slot_ids_group {
    public:
      using Coords = typename Topology::coords;
      using ValueSlotId = typename std::pair<Coords, ValueSlotName>;
      using const_iterator = typename Container<ValueSlotId>::const_iterator;

      slot_ids_group( const topology::coords_group<Coords>& cgroup, const ValueSlotName& slot_name );
      slot_ids_group( const topology::coords_group<Coords>& cgroup, const std::vector<ValueSlotName>& slot_names );
      const_iterator begin() const;
      const_iterator end() const;

      Container<ValueSlotId> elems() const;
    private:
      Container<ValueSlotId> elems_;
  };

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  slot_ids_group<Topology, ValueSlotName, Container>::slot_ids_group( const topology::coords_group<Coords>& cgroup, const ValueSlotName& slot_name ) : slot_ids_group{ cgroup, std::vector<ValueSlotName>{ slot_name }} {}

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  slot_ids_group<Topology, ValueSlotName, Container>::slot_ids_group( const topology::coords_group<Coords>& cgroup, const std::vector<ValueSlotName>& slot_names ) {
    for ( auto coords : cgroup ) {
      for ( auto slot_name : slot_names ) {
        elems_.push_back({ coords, slot_name });
      }
    }
  }

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  typename slot_ids_group<Topology, ValueSlotName, Container>::const_iterator
  slot_ids_group<Topology, ValueSlotName, Container>::begin() const {
    return elems_.begin();
  }

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  typename slot_ids_group<Topology, ValueSlotName, Container>::const_iterator
  slot_ids_group<Topology, ValueSlotName, Container>::end() const {
    return elems_.end();
  }

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  Container<typename slot_ids_group<Topology, ValueSlotName, Container>::ValueSlotId>
  slot_ids_group<Topology, ValueSlotName, Container>::elems() const {
    return elems_;
  }

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container = std::vector>
  class slot_ids_meta_group {
    public:
      using Coords = typename Topology::coords;
      using SlotIdsGroup = slot_ids_group<Topology, ValueSlotName, Container>;
      using const_iterator = typename Container<SlotIdsGroup>::const_iterator;

      slot_ids_meta_group( const topology::coords_meta_group<Coords>& cmgroup, const ValueSlotName& slot_name );
      slot_ids_meta_group( const topology::coords_meta_group<Coords>& cmgroup, const std::vector<ValueSlotName>& slot_name );
      const_iterator begin() const;
      const_iterator end() const;
    private:
      Container<SlotIdsGroup> elems_;
  };

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  slot_ids_meta_group<Topology, ValueSlotName, Container>::slot_ids_meta_group( const topology::coords_meta_group<typename Topology::coords>& cmgroup, const ValueSlotName& slot_name ) : slot_ids_meta_group{ cmgroup, std::vector<ValueSlotName>{ slot_name }} {}

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  slot_ids_meta_group<Topology, ValueSlotName, Container>::slot_ids_meta_group( const topology::coords_meta_group<typename Topology::coords>& cmgroup, const std::vector<ValueSlotName>& slot_names ) {
    for ( auto cgroup : cmgroup ) {
      for ( auto slot_name : slot_names ) {
        elems_.push_back( SlotIdsGroup{ cgroup, slot_name } );
      }
    }
  }

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  typename slot_ids_meta_group<Topology, ValueSlotName, Container>::const_iterator
  slot_ids_meta_group<Topology, ValueSlotName, Container>::begin() const {
    return elems_.begin();
  }

  template<typename Topology, typename ValueSlotName, template<typename...> typename Container>
  typename slot_ids_meta_group<Topology, ValueSlotName, Container>::const_iterator
  slot_ids_meta_group<Topology, ValueSlotName, Container>::end() const {
    return elems_.end();
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  class puzzle {
    protected:
      using Coords = typename Topology::coords;
      using ValueSlotId = typename std::pair<Coords, ValueSlotName>;
      using Grid = constraint::grid<ValueSlotId>;

      using Constraint = constraint::composite::conjunction<Grid>;
      //constraint::, to je takovy docasny namespace,
      //dokud nezrusim skoro vsechno, co je primo v adresari core
      using Givens = constraint::givens<ValueSlotId, GivensValueType>;

      using RegionId = std::string;
      using RegionIds = std::vector<RegionId>;
//      using Regions = constraint::regions<RegionId, Coords>;

      using CoordsGroup = typename topology::coords_group<Coords>;
      using SlotIdsGroup = typename core::slot_ids_group<Topology, ValueSlotName>;
      using SlotIdsMetaGroup = typename core::slot_ids_meta_group<Topology, ValueSlotName>;

      Topology topology_;
      Givens givens_;
      //parse_givens ma byt rozdelena na dva kusy jako v puvodni verzi z Frankfurtu:
      //jedna cast ma parsovat coordsy a patri do rectangle.hpp,
      //druha cast ma parsovat hodnotu a patri do ??integer.hpp??
      //
      //krom toho s novou implementaci Givens se jako klic nepouzivaji
      //primo coordsy, ale ValueSlotIds, takze parse_givens musi toto brat v potaz
      //
      //a mozna, ze parse_givens vubec nepatri sem. Hlavicka jeji implementace je rozhodne
      //hnusna a je divny, ze je to static. Je tady po presunu z LatinSquaru.
      static Givens parse_givens( const core::composite_input_node& givens_input );

      static RegionIds parse_region_ids( const core::vector_input_node& regions_input );

      std::vector<ValueSlotId> get_slot_ids_range( std::vector<Coords> region, std::vector<std::string> slot_names );//??

      CoordsGroup cg_centers();
      /*CoordsGroup cg_centers_no_given( const ValueSlotName& slot_name );
      CoordsGroup cg_centers_with_given( const ValueSlotName& slot_name );*/

      //navratovou hodnotou no_given() je lambda, ktera bere jako parameter
      //Coords objekt a vraci bool
      //hodnota tohoto boolu urcuje, zda se prislusne coords maji timto filterem
      //eliminovat (true); takze to vlastne vraci opacne hodnoty, nez by clovek cekal
      //(no_given vrati true, pokud given existuje...)
      std::function<bool (const Coords&)> no_given( const ValueSlotName& slot_name ) {
        return [this, &slot_name]( const Coords& coords) { 
          return this->givens_.has({ coords, slot_name });
        };
      }

      std::function<bool (const Coords&)> with_given( const ValueSlotName& slot_name ) {
        return [this, &slot_name]( const Coords& coords) {
          return ! this->givens_.has({ coords, slot_name });
        };
      }

      //a zejmena, a to je NEXT STEP, v tomto souboru potrebuju prinejmensim
      //definovat iteratory pres ValueSlotIds pro pouziti v definici ulohy
      //nekolik veci neni jasnych:
      //a) chci iteratory nebo range?
      //  to vyjde celkem nastejno
      //  nicmene v kombinaci s bodem b) se zda byt vyhodne pouzit range,
      //    ??u ktereho nebudu muset tolik resit lifetime??
      //b) chci pouzit custom iteratory nebo nejake ze std?
      //  custom je hroznej vopruz psat
      //  std vyzaduje container se vsemi hodnotami, coz muze byt i docela
      //    dost hodnot
      //  takze protoze Premature Optimization volim iterator ze std
      //c) jake iterace je zadouci podporovat:
      //  1) pres vsechny [typ_policka (Center) X ValueSlotNames]
      //    topology.iterate x ValueSlotName, cili basically make_slot_ids
      //  2) pres 1), ale vynechat ty, kde existuje Given
      //    .no_givens()
      //  3) pres vsechny ValueSlotIds, kde existuje Given
      //    .givens_only()
      //
      //takze by se mohlo mit tridu SlotIdsRange
      //  ta by mela zminene no_givens() a givens_only() metody
      //    a ty by klidne mohly brat Givens jako parametry, hmm
      //  a konstruktor by tedy bral iterator pres Coordsy a 
      //    iterator pres ValueSlotNames
      //
      //anebo asi lepe tridu SlotIdsRange nemit a misto ni mit
      //  metodu, ktera vrati ad hoc vytvoreny treba vector
      //  A to proto, ze tyto Range beztak potrebuje pouzit
      //  pouze inicializacni metoda (get_instance()), ktera
      //  ale ma tu vlastnost, ze muze menit obsah gridu
      //  a tedy i obsah Rangu; nezda se tedy, ze existuje
      //  vhodne misto, kde Range inicializovat a pak jen
      //  pouzivat. Premature Optimization => vytvaret
      //  Range vzdy ad hoc, dokud se neukaze, ze to je
      //  vykonnostni problem, a to se neukaze, protoze ta
      //  inicializace beztak nebude uzke hrdlo.
      //
      //nastrel:
      //  std::vector<ValueSlotId> get_slot_ids_range(
      //    CGroup, std::vector<ValueSlotName>);
      //  dale ale potrebuju i verze 2) a 3) z vyse,
      //    ktere omezuji range na coordsy, v nichz 
      //    je/neni zadan given pro urcity ValueSlotName
      //Jevi se jako rozumne zadat tedy dodatecny parametr,
      //  coz by byla lambda, urcujici, zda urcity Coords
      //  zahrnout, cili nic.
      //
      //??ValueSlotNames musi byt zodpovednost potomka simple_puzzle
      //  na druhou stranu givens je treba rozparsovat uz pred vytvorenim
      //  instance tohoto potomka, takze tomuto potomku do techto
      //  ValueSlotNames ponekud keca obsah inputu??
      //
      //well, metody jsou uzitecny, avsak nejsou uplne dobre pojmenovany,
      //antoz z jejich nazvu neni poznat, ze prvni parametr je CGroup
      //
      //!!a zjevne by se sikly i verze, berouci CMGroup a vracejici
      //std::vector<std::vector<ValueSlotId>> !!
      /*std::vector<ValueSlotId> get_slot_ids_range(
          const typename Topology::coords_group_old& coords_group,
          const std::vector<ValueSlotName>& slot_names);
      std::vector<ValueSlotId> get_slot_ids_range_no_given(
          const typename Topology::coords_group_old& coords_group,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& no_given_slot_names);
      std::vector<ValueSlotId> get_slot_ids_range_with_given(
          const typename Topology::coords_group_old& coords_group,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& with_given_slot_names);

      template<typename CoordsFilterCondition>
      std::vector<ValueSlotId> get_slot_ids_range(
          const typename Topology::coords_group_old& coords_group,
          const std::vector<ValueSlotName>& slot_names,
          CoordsFilterCondition condition);

      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges(
          const typename Topology::coords_group_range_old& coords_group_range,
          const std::vector<ValueSlotName>& slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_no_given(
          const typename Topology::coords_group_range_old& coords_group_range,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& no_given_slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_with_given(
          const typename Topology::coords_group_range_old& coords_group_range,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& with_given_slot_names);

      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges(
          const typename Topology::coords_meta_group_old& coords_meta_group,
          const std::vector<ValueSlotName>& slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_no_given(
          const typename Topology::coords_meta_group_old& coords_meta_group,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& no_given_slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_with_given(
          const typename Topology::coords_meta_group_old& coords_meta_group,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& with_given_slot_names);

      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges(
          const std::vector<typename Topology::coords_meta_group_old>& coords_meta_groups,
          const std::vector<ValueSlotName>& slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_no_given(
          const std::vector<typename Topology::coords_meta_group_old>& coords_meta_groups,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& no_given_slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_with_given(
          const std::vector<typename Topology::coords_meta_group_old>& coords_meta_groups,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& with_given_slot_names);

      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges(
          const Regions regions,
          const std::vector<ValueSlotName>& slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_no_given(
          const Regions regions,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& no_given_slot_names);
      std::vector<std::vector<ValueSlotId>> get_slot_ids_ranges_with_given(
          const Regions regions,
          const std::vector<ValueSlotName>& slot_names,
          const std::vector<ValueSlotName>& with_given_slot_names);
*/
    public:
      using PuzzleInstance = core::puzzle_instance<Grid, Constraint, Topology, SolutionValueType>;
      using SolutionValType = SolutionValueType;

      puzzle( const Topology& topology, const Givens& givens );
  };

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::puzzle( const Topology& topology, const Givens& givens ) : topology_{ topology }, givens_{ givens } {}

  //tento SimplePuzzle je Simple v tom smyslu, ze vsechna pouzita
  //policka obsahuji stejnou mnozinu ValueSlotu a ze stejne ValueSloty
  //obsahuji v kazdem pouzitem policku stejnou mnozinu prvku stejneho
  //ValueTypu
  //
  //je to podivne, nektere Puzzly se mi do tohohle vejdou, ale uz
  // - Mrakodrapy ne, protoze pouzivaji CompositeTopology,
  // - Tapa ne, leda bych vycucal spolecny ValueType pro policko s givensy
  //a pro boolovske policko
  //
  //!!Mrakodrapy se mi vlezou, kdyz vymyslim novej input_node a novou instanci
  //Givens, ktery budou reprezentovat givensy zadany nikoli pro policko
  //uvnitr gridu, ale pro "smer" v Gridu, cili basically pro CoordsGroupu!!
  //Takto mmchdm ma fungovat i vnitrni reprezentace puzzlu s compositnim
  //gridem: Jejich instance simple_puzzle nema pouzivat nic jako composite,
  //nybrz ma pracovat s tim, ze jsou veci, ktere jsou zadane pro smery.
  //
  //(Neco jako composite se mozna vyuzije pro slozene ulohy jako 
  //jak-se-to-zatr-sakr-jmenuje takovejch tech vzjemne propojenejch
  //Sudok, nebo Sudoku na krychli a podobne; pro jednoduche ulohy
  //zakladnich typu zadnej composite netreba, alespon dokud se bavime
  //o jejich vnitrni reprezentaci, checkovani a solveni; pro GUI zatim
  //myslenku CompositeGridu neopoustim, resp. neresim)
  //
  //celou zalezitost predka (predku) konkretni tridy (napr. LatinSquare),
  //ktery umi zpracovavat inputy a delat z nich puzzle_instance, je treba
  //dopracovat. V tuto chvili v zajmu sprintu volim nejjednodussi cestu,
  //vyjimam z LatinSquaru to, co ma LatinSquare ocividne dedit, davam to sem
  //a implementuju Puzzly, pro ktere to staci.
  template<typename Topology, typename ValueSlotName, typename ValueType>
  class simple_puzzle : public puzzle<Topology, ValueSlotName, ValueType, ValueType> {
    protected:
      using Puzzle = puzzle<Topology, ValueSlotName, ValueType, ValueType>;
      using Givens = typename Puzzle::Givens;
    public:
      simple_puzzle( const Topology& topology, const Givens& givens );
  };

  template<typename Topology, typename ValueSlotName, typename ValueType>
  simple_puzzle<Topology, ValueSlotName, ValueType>::simple_puzzle( const Topology& topology, const Givens& givens) : puzzle<Topology, ValueSlotName, ValueType, ValueType>{ topology, givens } {}



  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::Givens
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::parse_givens( const composite_input_node& givens_input ) {
    Givens result;
    for ( auto single_given_input : givens_input ) {
      auto single_given_cast = dynamic_cast<const composite_input_node&>( * single_given_input.second );
      int row = single_given_cast.get_int( "Row" );
      int col = single_given_cast.get_int( "Col" );
      int value = single_given_cast.get_int( "Value" );
      Coords coords{ row, col };
      ValueSlotId slot_id{ coords, "Value" }; 
      result.add( slot_id, GivensValueType{ value } );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::RegionIds
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::parse_region_ids( const core::vector_input_node& regions_input ) {
    return regions_input.get();
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::CoordsGroup
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::cg_centers() {
    return topology_.cg_centers();
  }
  


  /*template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_range( std::vector<Coords> region, std::vector<std::string> slot_names ) {
    std::vector<ValueSlotId> result;
    for ( auto coords : region ) {
      for ( auto slot_name : slot_names ) {
        result.push_back( std::make_pair( coords, slot_name ));
      }
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_range(
      const typename Topology::coords_group_old& coords_group,
      const std::vector<ValueSlotName>& slot_names) {
    return get_slot_ids_range( topology_.get_coords_in_group( coords_group ), slot_names );
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_range_no_given(
      const typename Topology::coords_group_old& coords_group,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& no_given_slot_names) {
    auto no_given_slot_filter_condition =
      [no_given_slot_names, this]( const typename Topology::coords& coords ) {
        return std::none_of( no_given_slot_names.begin(), no_given_slot_names.end(),
            [this, coords]( const ValueSlotName& ng_slot_name ) {
                return givens_.has({ coords, ng_slot_name });
            });
      };
      
    return get_slot_ids_range( coords_group, slot_names, no_given_slot_filter_condition );
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_range_with_given(
      const typename Topology::coords_group_old& coords_group,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& with_given_slot_names) {
    auto with_given_slot_filter_condition =
      [with_given_slot_names, this]( const typename Topology::coords& coords ) {
        return std::any_of( with_given_slot_names.begin(), with_given_slot_names.end(),
            [this, coords]( const ValueSlotName& wg_slot_name ) {
                return givens_.has({ coords, wg_slot_name });
            });
      };

    return get_slot_ids_range( coords_group, slot_names, with_given_slot_filter_condition );
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  template<typename CoordsFilterCondition>
  std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_range(
      const typename Topology::coords_group_old& coords_group,
      const std::vector<ValueSlotName>& slot_names,
      CoordsFilterCondition condition) {
    std::vector<ValueSlotId> result;
    for ( auto coords : topology_.get_coords_in_group( coords_group ) ) {
      if ( condition( coords ) ) {
        for ( auto slot_name : slot_names ) {
          result.push_back({ coords, slot_name });
        }
      }
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges(
      const typename Topology::coords_group_range_old& coords_group_range,
      const std::vector<ValueSlotName>& slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_group : coords_group_range ) {
      result.push_back( get_slot_ids_range( coords_group, slot_names ));
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_no_given(
      const typename Topology::coords_group_range_old& coords_group_range,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& no_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_group : coords_group_range ) {
      result.push_back( get_slot_ids_range_no_given( coords_group, slot_names, no_given_slot_names ));
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_with_given(
      const typename Topology::coords_group_range_old& coords_group_range,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& with_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_group : coords_group_range ) {
      result.push_back( get_slot_ids_range_with_given( coords_group, slot_names, with_given_slot_names ));
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges(
      const typename Topology::coords_meta_group_old& coords_meta_group,
      const std::vector<ValueSlotName>& slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_group_range : topology_.get_all_coords_groups({ coords_meta_group }) ) {
      auto range_result = get_slot_ids_range( coords_group_range, slot_names );
      result.push_back( range_result );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_no_given(
      const typename Topology::coords_meta_group_old& coords_meta_group,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& no_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_group_range : topology_.get_all_coords_groups({ coords_meta_group }) ) {
      auto range_result = get_slot_ids_range_no_given(
          coords_group_range, slot_names, no_given_slot_names );
      result.push_back( range_result );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_with_given(
      const typename Topology::coords_meta_group_old& coords_meta_group,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& with_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_group_range : topology_.get_all_coords_groups({ coords_meta_group }) ) {
      auto range_result = get_slot_ids_range_with_given(
          coords_group_range, slot_names, with_given_slot_names );
      result.push_back( range_result );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges(
      const std::vector<typename Topology::coords_meta_group_old>& coords_meta_groups,
      const std::vector<ValueSlotName>& slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_meta_group : coords_meta_groups ) {
      auto meta_group_result = get_slot_ids_ranges( coords_meta_group, slot_names );
      result.insert( result.end(), meta_group_result.begin(), meta_group_result.end() );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_no_given(
      const std::vector<typename Topology::coords_meta_group_old>& coords_meta_groups,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& no_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_meta_group : coords_meta_groups ) {
      auto meta_group_result = get_slot_ids_ranges_no_given(
          coords_meta_group, slot_names, no_given_slot_names );
      result.insert( result.end(), meta_group_result.begin(), meta_group_result.end() );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_with_given(
      const std::vector<typename Topology::coords_meta_group_old>& coords_meta_groups,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& with_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto coords_meta_group : coords_meta_groups ) {
      auto meta_group_result = get_slot_ids_ranges_with_given(
          coords_meta_group, slot_names, with_given_slot_names );
      result.insert( result.end(), meta_group_result.begin(), meta_group_result.end() );
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges(
      const Regions regions,
      const std::vector<ValueSlotName>& slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto region : regions ) {
      result.push_back( get_slot_ids_range( *( region.second ), slot_names ));
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_no_given(
      const Regions regions,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& no_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto region : regions ) {
      result.push_back( get_slot_ids_range_no_given( *( region.second ), slot_names, no_given_slot_names ));
    }
    return result;
  }

  template<typename Topology, typename ValueSlotName, typename GivensValueType, typename SolutionValueType>
  std::vector<std::vector<typename puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::ValueSlotId>>
  puzzle<Topology, ValueSlotName, GivensValueType, SolutionValueType>::get_slot_ids_ranges_with_given(
      const Regions regions,
      const std::vector<ValueSlotName>& slot_names,
      const std::vector<ValueSlotName>& with_given_slot_names) {
    std::vector<std::vector<ValueSlotId>> result;
    for ( auto region : regions ) {
      result.push_back( get_slot_ids_range_with_given( *( region.second ), slot_names, with_given_slot_names ));
    }
    return result;
  }*/
}
