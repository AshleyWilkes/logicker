#pragma once
#include <algorithm>

namespace logicker::core {
  template<class PuzzleType>
  class checker {
    public:
      checker(puzzle_instance<PuzzleType>&& assignment);
      bool is_solved_by(typename puzzle_instance<PuzzleType>::grid_type grid) const;
    private:
      const puzzle_instance<PuzzleType> assign_;
  };

  template<class PuzzleType>
  checker<PuzzleType>::checker(puzzle_instance<PuzzleType>&& assignment) : assign_{ std::move(assignment) } {}

  template<class PuzzleType>
  bool
  checker<PuzzleType>::is_solved_by(typename puzzle_instance<PuzzleType>::grid_type grid) const {
    auto& conds = assign_.get_condition_instances();
    return std::all_of(conds.begin(), conds.end(),
        [grid](auto& cond_p){ return cond_p->is_satisfied_by( grid ); });
  }
}
