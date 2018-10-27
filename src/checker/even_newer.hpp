#pragma once
#include "../core/puzzle_instance.hpp"

namespace logicker::checker {
  template<typename PuzzleInstance>
  class checker {
    public:
      explicit checker( const PuzzleInstance& assignment );
      bool check( const typename PuzzleInstance::grid& solution );
    private:
      const PuzzleInstance assignment_;
  };

  template<typename PuzzleInstance>
  checker<PuzzleInstance>::checker( const PuzzleInstance& assignment ) : assignment_{ assignment } {}

  template<typename PuzzleInstance>
  bool
  checker<PuzzleInstance>::check( const typename PuzzleInstance::grid& solution ) {
    //overit sanity
    //  ze solution obsahuje prave stejne klice value slotu,
    //  ze v kazdem value slotu jsou prave stejne values
    //  !!ted nedelam, je treba premyslet o tom, jestli by
    //  na tohle taky nemely byt constrainty a proc
    //overit completeness
    auto completeness_status = assignment_.completeness_constraint_.is_satisfied_by( solution );
    if ( completeness_status.is_broken() ) {
      std::cout << completeness_status;
      throw "checker/even_newer:check: completeness_constraint_ is broken\n";
    }
    std::cout << "    completeness complete\n";
    //zavolat solver a sehnat si kompletni reseni
    //  !!ted nedelam
    //overit givens
    auto givens_status = assignment_.givens_constraint_.is_satisfied_by( solution );
    if ( givens_status.is_broken() ) {
      std::cout << givens_status;
      throw "checker/even_newer:check: givens_constraint_ is broken\n";
    }
    std::cout << "    givens complete\n";
    //overit puzzle
    auto puzzle_status = assignment_.puzzle_constraint_.is_satisfied_by( solution );
    if ( puzzle_status.is_broken() ) {
      std::cout << puzzle_status;
      throw "checker/even_newer:check: puzzle_constraint_ is broken\n";
    }
    return true;
  }
}
