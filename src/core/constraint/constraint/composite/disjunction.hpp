#pragma once
#include "../composite.hpp"
#include <algorithm>

namespace logicker::core::constraint::composite {
  template<typename Grid>
  class disjunction : public composite<Grid> {
    public:
      constraint_status is_satisfied_by( const Grid& grid ) const override;
  };

  template<typename Grid>
  constraint_status
  disjunction<Grid>::is_satisfied_by( const Grid& grid ) const {
    /*return std::any_of( this->childs_.begin(), this->childs_.end(),
        [&grid]( const std::shared_ptr<constraint>& c ){ return c->is_satisfied_by( grid );});*/
    constraint_status result;
    for ( auto child : this->childs_ ) {
      auto cur_result = child->is_satisfied_by( grid );
      if ( cur_result.is_broken() ) {
        result.merge( cur_result, true );
      } else {
        return constraint_status{};
      }
    }
    return result;
  }
}
