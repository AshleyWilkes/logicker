#pragma once
#include "../composite.hpp"
#include <algorithm>

namespace logicker::core::constraint::composite {
  template<typename Grid>
  class conjunction : public composite<Grid> {
    public:
      constraint_status is_satisfied_by( const Grid& grid ) const override;
  };

  template<typename Grid>
  constraint_status
  conjunction<Grid>::is_satisfied_by( const Grid& grid ) const {
    constraint_status result;
    for ( auto child : this->childs_ ) {
      result.merge( child->is_satisfied_by( grid ), true );
    }
    return result;
  }
}
