#pragma once
#include "integer_field_type.hpp"
#include "input_node.hpp"
#include "puzzle_instance.hpp"

namespace logicker::core {
  template<class PuzzleType>
  class puzzle_instance_factory {
    public:
      //should return const object; not now kvuli checker( std::move(puzzle) );
      static puzzle_instance<PuzzleType> create(const input_node_base& input);

  };

  template<class GridType>
  class givens_processor {
    public:
      static void process_a_given(const composite_input_node& given_input_node, GridType& grid);
  };

  template<class Topology>
  class coords_parser {
    public:
      static typename Topology::coords parse_coords(const composite_input_node& given_input_node, Topology topology);
  };

  template<class ValueType>
  class value_parser {
    public:
      static ValueType parse_value(const composite_input_node& given_input_node);
  };

  template<class PuzzleType>
  puzzle_instance<PuzzleType>
  puzzle_instance_factory<PuzzleType>::create(const input_node_base& input) {
    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    const int_input_node& size_input = cast_input.get<int_input_node>("Size");
    int size = size_input.get();
    typename puzzle_instance<PuzzleType>::field_type field_type{ 1, size }; 
    typename puzzle_instance<PuzzleType>::topology::topology_size_t topology_size{ size, size };
    typename puzzle_instance<PuzzleType>::topology topology{ topology_size };
    puzzle_instance<PuzzleType> result{ topology, field_type };
    const composite_input_node& givens_input = cast_input.get<composite_input_node>("Givens");
    for (auto single_given_input : givens_input) {
      std::string single_given_name = single_given_input.first;
      const composite_input_node& single_given_node = givens_input.get<composite_input_node>(single_given_name);
      givens_processor<typename puzzle_instance<PuzzleType>::grid_type>::process_a_given(single_given_node, result.get_grid());
    }
    return result;
  }

  template<class GridType>
  void
  givens_processor<GridType>::process_a_given(const composite_input_node& given_input_node, GridType& grid) {
    typename GridType::coords_type coords = coords_parser<typename GridType::topology_type>::parse_coords(given_input_node, grid.topology());
    typename GridType::value_type value = value_parser<typename GridType::value_type>::parse_value(given_input_node);
    grid.set_value(coords, value);
  }

  template<>
  typename rectangle::coords
  coords_parser<rectangle>::parse_coords(const composite_input_node& given_input_node, rectangle topology) {
    const int_input_node& row_node = given_input_node.get<int_input_node>("Row");
    const int_input_node& col_node = given_input_node.get<int_input_node>("Col");
    return { topology.size(), row_node.get(), col_node.get() };
  }

  template<>
  int
  value_parser<int>::parse_value(const composite_input_node& given_input_node) {
    const int_input_node& value_node = given_input_node.get<int_input_node>("Value");
    return value_node.get();
  }
}
