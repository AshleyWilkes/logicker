#pragma once

namespace logicker::core::constraint {
  template<typename ValueType, typename Grid>
  class value_constraint : public constraint<Grid> {
    public:
      constraint_status is_satisfied_by( const Grid& grid ) const override = 0;
  };
}
