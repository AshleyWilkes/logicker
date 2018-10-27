#pragma once
#include "../constraint.hpp"
#include <memory>
#include <vector>

namespace logicker::core::constraint::composite {
  template<typename Grid>
  class composite : public constraint<Grid> {
    public:
      //prida novou podminku k jiz pritomnym
      //??potrebujeme i disjunkci??
      template<typename Constraint>
      void add_constraint( const Constraint& constraint_ );

      constraint_status is_satisfied_by( const Grid& grid ) const override = 0;
    protected:
      //tady bych mel radsi unique_ptr, ale nekompiluje to
      //a nechapu proc
      std::vector<std::shared_ptr<constraint<Grid>>> childs_;
  };

  template<typename Grid>
  template<typename Constraint>
  void
  composite<Grid>::add_constraint( const Constraint& constraint_ ){
    //auto sh = std::make_shared<Constraint>( constraint_ );
    //childs_.push_back( sh );
    childs_.push_back( std::make_shared<Constraint>( constraint_ ));
  }
}
