#include "puzzle/latin_square.hpp"
#include "core/input_node.hpp"
#include "core/puzzle_instance.hpp"
#include "core/checker.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <vector>

int main() {
  std::cout << "This is Logicker!\n";
  //tak to by neslo, potrebuju classu solver (a napred checker)
  /*try {
    using puzzle_type = logicker::puzzle::latin_square<2>;
    using field_type = puzzle_type::field_type;
    using topology = puzzle_type::topology;
    puzzle_type puzzle;

    puzzle.get_field({0, 0}).set_value(1);
    while ( auto deduction = puzzle.try_to_find_deduction()) {
      puzzle.apply_deduction(deduction);
    }
    std::cout << puzzle.get_grid();
  } catch (char const* e) {
    std::cout << e;
  }*/

  try {
  using logicker::puzzle::latin_square;
  using logicker::core::puzzle_instance;
  using puzzle_t = puzzle_instance<latin_square>;
  using checker_t = logicker::core::checker<puzzle_t>;

  using base_input = logicker::core::input_node_base;
  using int_input = logicker::core::int_input_node;
  using composite_input = logicker::core::composite_input_node;
  std::shared_ptr<base_input> size_input = std::make_shared<int_input>(int_input{ "Size", 4 });
  std::map<std::string, std::shared_ptr<base_input>> map;
  map.insert({ "Size", std::move(size_input) });
  composite_input input{ "Size", map };
  puzzle_t puzzle_inst = puzzle_t::primitive_create( input );
  checker_t checker{ std::move(puzzle_inst) };

  std::vector<int> vals {
    1,2,3,4,
    3,4,1,2,
    2,3,4,1,
    4,1,2,3};

  std::cout << std::boolalpha << checker.is_solved_by(vals) << '\n';

  /*puzzle_t puzzle({ 4, 4 });
  checker_t checker{ std::move(puzzle) };

  std::cout << std::boolalpha << checker.is_solved_by(vals) << '\n';

  //using field_type = puzzle_t::field_type;
  using field_type = logicker::core::field_type<int>;
  field_type field_t{ 1, 4 };
  using topology = puzzle_t::topology;

  logicker::core::grid<field_type, topology> sol_grid { puzzle.grid_size(), field_t };
  for (auto coords : topology::get_all_coords( puzzle.grid_size() )) {
    //sol_grid.get_field(coords).set_limits(1, 4);
  }
  sol_grid.set_values(vals);

  std::cout << std::boolalpha << checker.is_solved_by(sol_grid) <<'\n';
  std::cout << sol_grid;*/

  } catch (char const* e) {
    std::cout << e;
  }
}
