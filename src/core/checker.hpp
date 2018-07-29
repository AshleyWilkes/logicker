#pragma once
#include <algorithm>

namespace logicker::core {
  template<class PuzzleInstanceType>
  class checker {
    public:
      checker(PuzzleInstanceType&& assignment);
      bool is_solved_by(typename PuzzleInstanceType::grid_type grid) const;
    private:
      const PuzzleInstanceType assign_;
  };

  template<class PuzzleInstanceType>
  checker<PuzzleInstanceType>::checker(PuzzleInstanceType&& assignment) : assign_{ std::move(assignment) } {}

  template<class PuzzleInstanceType>
  bool
  checker<PuzzleInstanceType>::is_solved_by(typename PuzzleInstanceType::grid_type grid) const {
    auto& conds = assign_.get_condition_instances();
    return std::all_of(conds.begin(), conds.end(),
        [grid](auto& cond_p){ return cond_p->is_satisfied_by( grid ); });
  }
}
