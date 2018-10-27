/*#pragma once
#include <boost/optional.hpp>
#include "condition.hpp"
#include "factory.hpp"

namespace logicker::core::solver {
  template<class PuzzleType>
  class solver {
    public:
      using value_t = typename PuzzleType::value_type;
      using field_t = typename PuzzleType::field_type;
      using topology_t = typename PuzzleType::topology;
      using grid_t = typename core::grid<field_t, topology_t>;
      using deduction_t = typename grid_t::deduction_type;
      using condition_t = solver_condition_instance<PuzzleType>;
      using condition_p = typename std::shared_ptr<condition_t>;

      solver(const grid_t& assignment_grid, const std::vector<condition_p>& conditions);
      const grid_t& get_solution() const;

      static solver<PuzzleType> create(const input_node_base& input);
    private:
      const grid_t assignment_grid_;
      mutable grid_t working_grid_;
      const std::vector<condition_p> conditions_;

      boost::optional<deduction_t> find_deduction();
      void perform_deduction( deduction_t& deduction );
  };

  template<class PuzzleType>
  solver<PuzzleType>::solver(const grid_t& assignment_grid, const std::vector<condition_p>& conditions) : assignment_grid_{ assignment_grid }, working_grid_{ assignment_grid_}, conditions_{ conditions } {}

  template<class PuzzleType>
  const typename solver<PuzzleType>::grid_t&
  solver<PuzzleType>::get_solution() const {
    while ( auto& deduction_opt = this->find_deduction() ) {
      this->perform_deduction( deduction_opt.get() );
    }
    return working_grid_;
  }

  template<class PuzzleType>
  solver<PuzzleType>
  solver<PuzzleType>::create(const input_node_base& input) {
    auto factory = solver_factory<
      typename PuzzleType::value_type,
      typename PuzzleType::topology,
      typename PuzzleType::condition
    >();

    auto field_type = factory.create_field( input );
    auto topology = factory.create_topology( input );
    puzzle_instance<PuzzleType> instance{ topology, field_type };
    auto grid = instance.get_grid();
    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    //castit na const& jako na predchozim radku??
    const composite_input_node& givens_input = cast_input.get<composite_input_node>( "Givens" );
    for (auto single_given_input : givens_input) {
      std::string single_given_name = single_given_input.first;
      //castit na const& ??
      const composite_input_node& single_given_node = givens_input.get<composite_input_node>( single_given_name );
      givens_processor<typename puzzle_instance<PuzzleType>::grid_type>::process_a_given( single_given_node, grid );
    }
    auto conditions = factory.create_conditions( input );
    solver<PuzzleType> result{ grid, conditions };
    return result;
  }
}*/
