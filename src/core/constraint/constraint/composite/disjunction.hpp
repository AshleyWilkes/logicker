#pragma once
#include "../composite.hpp"
#include <algorithm>

namespace logicker::core::constraint::composite {
  template<typename Grid>
  class disjunction : public composite<Grid> {
    public:
      bool is_satisfied_by( const Grid& grid ) const override;
  };

  template<typename Grid>
  bool
  disjunction<Grid>::is_satisfied_by( const Grid& grid ) const {
    return std::any_of( childs_.begin(), childs_.end(),
        [&grid]( const std::shared_ptr<constraint>& c ){ return c->is_satisfied_by( grid );});
  }
}
