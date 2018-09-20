#pragma once
#include "../core/input_node.hpp"
#include "traits.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace logicker::core::checker {
  template<class PuzzleType>
  class checker {
    public:
      using value_t = typename PuzzleType::value_type;
      using field_t = typename PuzzleType::field_type;
      using topology_t = typename PuzzleType::topology;
      using grid_t = typename core::grid<field_t, topology_t>;
      using condition_p = typename std::shared_ptr<condition_instance<field_t>>;

      checker(const grid_t& assignment_grid, const std::vector<condition_p>& conditions);
      bool is_solved_by(const grid_t& grid) const;
      const grid_t& assignment_grid() const;

      static checker<PuzzleType> create(const input_node_base& input);
    private:
      const grid_t assignment_grid_;
      const std::vector<condition_p> conditions_;
  };

  template<class PuzzleType>
  checker<PuzzleType>::checker(const grid_t& assignment_grid, const std::vector<condition_p>& conditions) : assignment_grid_{ assignment_grid }, conditions_{ conditions } {}

  template<class PuzzleType>
  bool
  checker<PuzzleType>::is_solved_by(const grid_t& grid) const {
    if (! grid.is_subset_of( assignment_grid_ )) {
      //throw "checker::is_solved_by error: sanity check failed";
      return false;
    }
    auto values = grid.get_values();
    return std::all_of( conditions_.begin(), conditions_.end(),
        [&values]( auto cond_p ){ return cond_p->is_satisfied_by( values ); });
  }

  template<class PuzzleType>
  const typename checker<PuzzleType>::grid_t&
  checker<PuzzleType>::assignment_grid() const {
    return assignment_grid_;
  }

  template<class PuzzleType>
  checker<PuzzleType>
  checker<PuzzleType>::create(const core::input_node_base& input) {
    //abych mohl vytvorit grid, potrebuju vedet field type
    //field typy (prinejmensim v solveru) jsou ovlivneny potrebami
    //  podminek (resp. zpusobem, jak solver s temi podminkami pracuje)
    //
    //jenomze ??pravda je, ze field type nemusi byt stejny pro vsechna policka
    //  v gridu??. Pak by tedy musely (field typy) mit spolecneho predka, 
    //  kterym bych otypoval grid. Ale zatim to nekomplikujme zvytecne:
    //  udelam postup, ktery solveru umozni vytvorit si grid s pomocnymi
    //  policky, a teprve pak budu resit jeste obecnejsi implementaci.
    //
    //takze by mi mel stacit tento postup:
    //  1) z value_typu a conditions urcit skutecny typ field_type
    //    -- tohle dela jinak checker a jinak solver
    //  2) inicializovat instanci field_typu, ktera se rozkopiruje do vsech fieldu
    //  3) inicializovat grid<field_type>
    //  4) inicializovat podminky
    //    -- tohle dela jinak checker a jinak solver
    //  5) setnout givens
    //
    //A) latin_square
    //B) latin_square s pismeny
    //C) latin_square s mezerami
    //
    //1, 2)
    //  A) Check
    //    int, each_value_once => int{ 1, "Size" }
    //  A) Solve
    //    int, each_value_once => int{ 1, "Size" }
    //  B) Check
    //    char, each_value_once => char{ "A", "B", "C" }
    //  B) Solve
    //    char, each_value_once => char{ "A", "B", "C" }
    //  C) Check
    //    optional<int>, each_value_once => optional<int>{ 1, "MaxValue" }
    //  C) Solve
    //    optional<int>, each_value_once => <optional<int>, bool>{{ 1, "MaxValue" }, { true, false }}
    //3)
    //  A, B, C) Check, Solve
    //    square => rectangle{ "Size", "Size" }
    //4)
    //  A) Check
    //    each_value_once, int => checker::each_value_once<int>
    //  A) Solve
    //    each_value_once, int => solver::each_value_once<int>
    //  B) Check
    //    each_value_once, char => checker::each_value_once<char>
    //  B) Solve
    //    each_value_once, chat => solver::each_value_once<char>
    //  C) Check
    //    each_value_once, optional<int> => checker::each_value_once<optional<int>>
    //  C) Solve
    //    each_value_once, optional<int> => solver::each_value_once<optional<int>>

    //auto field_type = field_factory<PuzzleType::value_type, PuzzleType::condition>
    auto factory = checker_factory<
      typename PuzzleType::value_type, 
      typename PuzzleType::topology,
      typename PuzzleType::condition
    >();

    auto field_type = factory.create_field( input );
    //typename traits::field_t field_type{ input };
    auto topology = factory.create_topology( input );
    //typename traits::topology_t topology{ input };

    //nasledujici je skopceno ze stareho checkera za ucelem uvedeni do
    //provozu, nedobre:
    //konstruktor puzzle_instance inicializuje podminky, coz by nemel delat on
    //krom toho lepe by bylo inicializovat membery bez mezikroku puzzle_instance
    puzzle_instance_v1<PuzzleType> instance{ topology, field_type };
    auto grid = instance.get_grid();
    //setovani givensu, ja nevim, melo by mit aspon vlastni metodu,
    //takhle je to v teto metode celkem detail a pritom je to vic nez pulka kodu
    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    const composite_input_node& givens_input = cast_input.get<composite_input_node>("Givens");
    for (auto single_given_input : givens_input) {
      std::string single_given_name = single_given_input.first;
      const composite_input_node& single_given_node = givens_input.get<composite_input_node>(single_given_name);
      givens_processor<typename puzzle_instance_v1<PuzzleType>::grid_type>::process_a_given(single_given_node, grid);
    }
    checker<PuzzleType> result{ grid, instance.get_condition_instances() };
    return result;
  }
}
