#pragma once

//takze diskuse k inicializaci gridu a podminek nize oduvodnuje,
//ze vlastne nic jako puzzle_instance neexistuje, misto ni
//ma existovat classa assignment s velmi podobnymi vlastnostmi
//tato classa, !!stejne jako checker a solver!!, ma byt inicializovana
//primo z input_nodu.
//
//??anebo assignment na nic nepotrebuju??
//
//kazdopadne tedy novy plan je vytvaret checker tovarnou <class PuzzleType>(input_node)
//a solver jinou takovou tovarnou
//instead of
//takovou tovarnou vytvarim puzzle_instance a tu posilam konstruktoru checkera a solvera
namespace logicker::core {
  template<class PuzzleType>
  class puzzle_instance_v1 {
    public:
      typedef typename PuzzleType::field_type field_type;
      typedef typename PuzzleType::topology topology;
      typedef typename topology::coords coords_type;
      typedef typename core::grid<field_type, topology> grid_type;
      using deduction_type = typename grid_type::deduction_type;
      typedef typename core::condition_instance<field_type> condition_instance;
      typedef typename std::shared_ptr<condition_instance> condition_instance_p;

      //this calls grid_type constructor, which is incorrect, grid initialization
      //depends not only in puzzle description, but also on why the puzzle
      //instance is being created:
      //1) to create an assingment, simple grid init is needed
      //2) to create a checker, simple grid init is needed
      //3) to create a solver, more complex grid init is needed
      //
      //(assignment and checker both use the same grid features (namely
      //the ability to place a value to a field), that's why they need
      //the same init method; solver needs to "understand" the grid
      //in context of conditions too, that's why it needs more complex
      //initialization)
      puzzle_instance_v1(const topology& topology, const field_type& field_type);
      grid_type get_grid() const;
      grid_type& get_grid();
      std::vector<condition_instance_p>& get_condition_instances() const;
      std::vector<simple_condition_instance<field_type>> get_simple_condition_instances() const;
    private:
      mutable std::vector<condition_instance_p> grid_conds_;
      mutable grid_type grid_;

      //this is wrong, because the conds init depends not only on puzzle description,
      //but also on why the puzzle instance is being created
      //1) to create an assignment, no conds init is needed
      //2) to create a checker, simple conds init is needed
      //3_ to create a solver, more complex conds init is needed
      void init_grid_conds() const;
      void init_grid_conds(const puzzle::condition_description& cond_desc) const;
  };

  template<class PuzzleType>
  puzzle_instance_v1<PuzzleType>::puzzle_instance_v1(const topology& topology, const field_type& field_type) : grid_ { topology, field_type } {
    init_grid_conds();
  }

  template<class PuzzleType>
  typename puzzle_instance_v1<PuzzleType>::grid_type
  puzzle_instance_v1<PuzzleType>::get_grid() const {
    return grid_;
  }

  template<class PuzzleType>
  typename puzzle_instance_v1<PuzzleType>::grid_type&
  puzzle_instance_v1<PuzzleType>::get_grid() {
    return grid_;
  }

  template<class PuzzleType>
  std::vector<typename puzzle_instance_v1<PuzzleType>::condition_instance_p>&
  puzzle_instance_v1<PuzzleType>::get_condition_instances() const {
    return grid_conds_;
  }

  template<class PuzzleType>
  std::vector<simple_condition_instance<typename PuzzleType::field_type>>
  puzzle_instance_v1<PuzzleType>::get_simple_condition_instances() const {
    //!!THIS IMPLEMENTATION requires rectangle and condition_instance be
    //moveable, i.e. not have const members!!
    std::vector<simple_condition_instance<field_type>> result;
    for ( auto& grid_cond : grid_conds_ ) {
      std::vector<simple_condition_instance<field_type>> grid_cond_simples = (*grid_cond).get_as_simple_instances_vector();
      result.insert( result.end(), grid_cond_simples.begin(), grid_cond_simples.end() );
    }
    return result;
  }



  template<class PuzzleType>
  void
  puzzle_instance_v1<PuzzleType>::init_grid_conds() const {
    for (const auto& cond_desc : PuzzleType::get_condition_descriptions()) {
      init_grid_conds(cond_desc);
    }
  }

  template<class PuzzleType>
  void
  puzzle_instance_v1<PuzzleType>::init_grid_conds(const puzzle::condition_description& cond_desc) const {
    for (const auto& group : grid_.topology().get_all_coords_groups(cond_desc.first)) {
      auto group_coords = grid_.topology().get_coords_in_group(group);
      std::vector<int> field_indices;
      for (size_t i = 0 ; i < group_coords.size(); ++i) {
        //field_indices.push_back( group_coords.at(i).index() );
        field_indices.push_back( grid_.topology().coords_index( group_coords.at(i) ));
      }
      grid_conds_.push_back(
          condition_instance::create_instance( cond_desc.second, field_indices));
    }
  }
}
