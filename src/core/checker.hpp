#pragma once
#include <algorithm>

namespace logicker::core {
  template<class PuzzleInstanceType>
  class checker {
    public:
      checker(PuzzleInstanceType&& assignment);
      //todle dvojity is_solved_by vypada jako spatne vyresene zavislosti
      //nebo mozna ne, grid pouziva coords-part souradneho systemu,
      //vector zase index-part tohoto systemu
      //chci pouzivat obe casti souradneho systemu: coordsy
      //jsou uzitecne pro debug a reporting,
      //vector zase pro posilani vstupu jako vectoru Values
      bool is_solved_by(typename PuzzleInstanceType::grid_type grid) const;
      bool is_solved_by(const std::vector<typename PuzzleInstanceType::field_type::value_type>& values) const;
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

  template<class PuzzleInstanceType>
  bool
  checker<PuzzleInstanceType>::is_solved_by(const std::vector<typename PuzzleInstanceType::field_type::value_type>& values) const {
    auto& conds = assign_.get_condition_instances();
    return std::all_of(conds.begin(), conds.end(),
        [values](auto& cond_p){ return cond_p->is_satisfied_by( values ); });
  }
}
