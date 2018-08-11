#pragma once

namespace logicker::core {
  template<class PuzzleType>
  class puzzle_instance {
    public:
      typedef typename PuzzleType::field_type field_type;
      typedef typename PuzzleType::topology topology;
      typedef typename topology::coords coords_type;
      typedef typename core::grid<field_type, topology> grid_type;
      using deduction_type = typename grid_type::deduction_type;
      typedef typename core::condition_instance<field_type, topology> condition_instance;
      typedef typename std::shared_ptr<condition_instance> condition_instance_p;

      puzzle_instance(const topology& topology, const field_type& field_type);
      grid_type get_grid() const;
      grid_type& get_grid();
      std::vector<condition_instance_p>& get_condition_instances() const;
      std::vector<simple_condition_instance<field_type, topology>> get_simple_condition_instances() const;
    private:
      mutable std::vector<condition_instance_p> grid_conds_;
      mutable grid_type grid_;

      void init_grid_conds() const;
      void init_grid_conds(const puzzle::condition_description& cond_desc) const;
  };

  template<class PuzzleType>
  puzzle_instance<PuzzleType>::puzzle_instance(const topology& topology, const field_type& field_type) : grid_ { topology, field_type } {
    init_grid_conds();
  }

  template<class PuzzleType>
  typename puzzle_instance<PuzzleType>::grid_type
  puzzle_instance<PuzzleType>::get_grid() const {
    return grid_;
  }

  template<class PuzzleType>
  typename puzzle_instance<PuzzleType>::grid_type&
  puzzle_instance<PuzzleType>::get_grid() {
    return grid_;
  }

  template<class PuzzleType>
  std::vector<typename puzzle_instance<PuzzleType>::condition_instance_p>&
  puzzle_instance<PuzzleType>::get_condition_instances() const {
    return grid_conds_;
  }

  template<class PuzzleType>
  std::vector<simple_condition_instance<typename PuzzleType::field_type, typename PuzzleType::topology>>
  puzzle_instance<PuzzleType>::get_simple_condition_instances() const {
    //!!THIS IMPLEMENTATION requires rectangle and condition_instance be
    //moveable, i.e. not have const members!!
    std::vector<simple_condition_instance<field_type, topology>> result;
    for ( auto& grid_cond : grid_conds_ ) {
      std::vector<simple_condition_instance<field_type, topology>> grid_cond_simples = (*grid_cond).get_as_simple_instances_vector();
      result.insert( result.end(), grid_cond_simples.begin(), grid_cond_simples.end() );
    }
    return result;
  }



  template<class PuzzleType>
  void
  puzzle_instance<PuzzleType>::init_grid_conds() const {
    for (const auto& cond_desc : PuzzleType::get_condition_descriptions()) {
      init_grid_conds(cond_desc);
    }
  }

  template<class PuzzleType>
  void
  puzzle_instance<PuzzleType>::init_grid_conds(const puzzle::condition_description& cond_desc) const {
    for (const auto& group : grid_.topology().get_all_coords_groups(cond_desc.first)) {
      auto group_coords = grid_.topology().get_coords_in_group(group);
      std::vector<int> field_indices;
      for (size_t i = 0 ; i < group_coords.size(); ++i) {
        field_indices.push_back( group_coords.at(i).index() );
      }
      grid_conds_.push_back(
          condition_instance::create_instance( cond_desc.second, field_indices));
    }
  }
}
