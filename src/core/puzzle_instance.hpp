#pragma once

namespace logicker::core {
  template<class PuzzleType>
  class puzzle_instance {
    public:
      typedef typename PuzzleType::field_type field_type;
      typedef typename PuzzleType::topology topology;
      typedef typename core::grid<field_type, topology> grid_type;
      typedef typename core::condition_instance<field_type, topology> condition_instance;
      typedef typename std::unique_ptr<condition_instance> condition_instance_p;

      puzzle_instance(typename topology::size size, const field_type& field_type);
      typename topology::size grid_size() const;
      grid_type get_grid() const;
      grid_type& get_grid();
      std::vector<condition_instance_p>& get_condition_instances() const;
    private:
      const typename topology::size grid_size_;
      mutable std::vector<condition_instance_p> grid_conds_;
      mutable grid_type grid;

      void init_grid_conds() const;
      void init_grid_conds(const puzzle::condition_description& cond_desc) const;
  };

  template<class PuzzleType>
  puzzle_instance<PuzzleType>::puzzle_instance(typename topology::size size, const field_type& field_type) : grid_size_{ size }, grid { size, field_type } {}

  template<class PuzzleType>
  typename puzzle_instance<PuzzleType>::topology::size
  puzzle_instance<PuzzleType>::grid_size() const {
    return grid_size_;
  }

  template<class PuzzleType>
  typename puzzle_instance<PuzzleType>::grid_type
  puzzle_instance<PuzzleType>::get_grid() const {
    return grid;
  }

  template<class PuzzleType>
  typename puzzle_instance<PuzzleType>::grid_type&
  puzzle_instance<PuzzleType>::get_grid() {
    return grid;
  }

  template<class PuzzleType>
  std::vector<typename puzzle_instance<PuzzleType>::condition_instance_p>&
  puzzle_instance<PuzzleType>::get_condition_instances() const {
    if (grid_conds_.empty()) {
      init_grid_conds();
    }
    return grid_conds_;
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
    for (const auto& group : topology::get_all_coords_groups(grid_size(), cond_desc.first)) {
      grid_conds_.push_back(
          condition_instance::create_instance(
              cond_desc.second, 
              topology::get_coords_in_group(
                  grid_size(), group)));
    }
  }
}
