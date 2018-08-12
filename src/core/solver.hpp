#pragma once
#include "condition_instance.hpp"
#include "deducer.hpp"
#include <memory>
#include <vector>

namespace logicker::core {
  template<class PuzzleInstanceType>
  class solver_condition_instance {
    public:
      typedef typename PuzzleInstanceType::coords_type coords_type;
      typedef typename PuzzleInstanceType::field_type field_type;
      typedef typename PuzzleInstanceType::topology topology_type;
      typedef simple_condition_instance<field_type> condition_type;
      using deduction_type = typename PuzzleInstanceType::deduction_type;
      using grid_type = typename PuzzleInstanceType::grid_type;
      using value_type = typename field_type::value_type;

      solver_condition_instance(std::vector<int> field_indices, std::vector<std::shared_ptr<deducer<value_type>>> deducers) : field_indices_{ field_indices }, deducers_{ deducers } {}

      //
      //pokud vratis boost::none, nastav processed nebo exhausted na true
      boost::optional<deduction_type> try_deduce(grid_type grid);

      bool is_processed() const { return processed_; }
      bool is_exhausted() const { return exhausted_; }
      std::vector<int> field_indices() const { return field_indices_; }
      void set_unprocessed() { processed_ = false; }

      static bool compare_conditions(const solver_condition_instance<PuzzleInstanceType>& lhs, const solver_condition_instance<PuzzleInstanceType>& rhs);
    private:
      std::vector<int> field_indices_;
      std::vector<std::shared_ptr<deducer<value_type>>> deducers_;
      bool processed_{ false }, exhausted_{ false };
  };

  template<class PuzzleInstanceType>
  boost::optional<typename PuzzleInstanceType::deduction_type>
  solver_condition_instance<PuzzleInstanceType>::try_deduce(typename PuzzleInstanceType::grid_type grid) {
    std::vector<field<field_type>> fields;
    for (auto index : field_indices_) {
      fields.push_back( grid.get_field( index ) );
    }
    for (auto deducer : deducers_) {
      auto ded_opt = deducer->try_deduce( fields );
      if ( ded_opt ) {
        auto origin = ded_opt.get();
        typename PuzzleInstanceType::deduction_type result{ field_indices_[origin.index_], origin.value_ };
        return result;
      }
    }
    processed_ = true;
    return boost::none;
  }

  template<class PuzzleInstanceType>
  bool
  solver_condition_instance<PuzzleInstanceType>::compare_conditions(const solver_condition_instance<PuzzleInstanceType>& lhs, const solver_condition_instance<PuzzleInstanceType>& rhs) {
    if ( lhs.is_processed() || lhs.is_exhausted() ) return false;
    if ( rhs.is_processed() || rhs.is_exhausted() ) return true;
    //prozatim pouzivam pro jednoduchost pouze primarni kriterium
    return lhs.field_indices().size() < rhs.field_indices().size();
  }

  template<class PuzzleInstanceType>
  class solver {
    public:
      typedef typename PuzzleInstanceType::coords_type coords_type;
      typedef typename PuzzleInstanceType::field_type field_type;
      typedef typename PuzzleInstanceType::grid_type grid_type;
      typedef typename PuzzleInstanceType::topology topology_type;
      using deduction_type = typename PuzzleInstanceType::deduction_type;
  
      solver(PuzzleInstanceType assignment, grid_type working_grid, std::vector<solver_condition_instance<PuzzleInstanceType>> solver_condition_instances) : assign_{ assignment }, working_grid_{ working_grid }, cond_insts_ { solver_condition_instances } {}
      typename PuzzleInstanceType::grid_type get_solution() const;
    private:
      solver_condition_instance<PuzzleInstanceType>& pick_a_condition() const;

      const PuzzleInstanceType assign_;
      mutable typename PuzzleInstanceType::grid_type working_grid_;
      mutable std::vector<solver_condition_instance<PuzzleInstanceType>> cond_insts_;
  };

  template<class PuzzleInstanceType>
  typename PuzzleInstanceType::grid_type
  solver<PuzzleInstanceType>::get_solution() const {
    //postup solveni je repeat steps:
    //1) pick a deducing_instance, ask it for a deduction
    //    najdi podminku, ktera umi dedukovat, neni processed a je minimalni
    //    podle primarniho i sekundarniho porovnavaciho kriteria
    //2) if a deduction was found, perform it on the working grid, then wake
    //    conditions containing the deduction's field
    
    //for (solver_condition_instance<PuzzleInstanceType>& cond2 = this->pick_a_condition(); !cond2.is_processed() && !cond2.is_exhausted(); cond2 = this->pick_a_condition()) {
    while (true) {
      auto& cond{this->pick_a_condition()};
      if (cond.is_processed() || cond.is_exhausted()) break;
      auto deduction_opt = cond.try_deduce( working_grid_ );
      if ( deduction_opt ) {
        //perform the deduction on the working_grid_
        auto deduction = deduction_opt.get();
        working_grid_.perform_deduction( deduction );
        //wake up appropriate solver_instances
        for ( auto it = cond_insts_.begin(); it != cond_insts_.end(); ++it ) {
          auto field_indices = it->field_indices();
          if ( std::find( field_indices.begin(), field_indices.end(), deduction.field_index_ ) != field_indices.end() ) {
          //if ( it->field_indices().find( deduction.field_index_ ) != it->field_indices().end()) {
            it->set_unprocessed();
          }
        }
      }
    }
    return working_grid_;
  }

  template<class PuzzleInstanceType>
  solver_condition_instance<PuzzleInstanceType>&
  solver<PuzzleInstanceType>::pick_a_condition() const {
    typename std::vector<solver_condition_instance<PuzzleInstanceType>>::iterator best_cond_it = std::min_element( cond_insts_.begin(), cond_insts_.end(), solver_condition_instance<PuzzleInstanceType>::compare_conditions );
    return *best_cond_it;
  }

  template<class PuzzleInstanceType>
  class solver_factory {
    public:
      typedef typename PuzzleInstanceType::field_type field_type;
      typedef typename PuzzleInstanceType::topology topology_type;

      solver<PuzzleInstanceType> create_solver_for_assignment(PuzzleInstanceType assignment);
  };

  //Zda se, ze rozhodnout, pro ktere skupiny policek vytvaret solver_instance
  //je velmi tezky problem, nechavam ted byt a jdu nejjednodussi cestou:
  //vytvorim jeden solver_instance pro kazdou elementary_instance a jinak zatim
  //nic. Jako dalsi krok se jevi bud dotvorit teorii natolik, ze by se sem
  //dal opravdu pripsat generator solver_instanci, jak jsem to chtel udelat,
  //anebo ??do condition_instance.hpp?? pripsat mechanismus, jak solver_instance
  //generovat rucne.
  template<class PuzzleInstanceType>
  solver<PuzzleInstanceType>
  solver_factory<PuzzleInstanceType>::create_solver_for_assignment(PuzzleInstanceType assignment) {
    const auto& simple_instances_ = assignment.get_simple_condition_instances();
    if ( simple_instances_.empty() ) {
      throw "Can't create solver for assignment with no conditions";
    }
    std::vector<solver_condition_instance<PuzzleInstanceType>> solver_instances;
    for ( auto sci : simple_instances_ ) {
      std::shared_ptr<deducer<int>> deducer_ptr = std::make_shared<neq_deducer<int>>( neq_deducer<int>{} );
      std::vector<std::shared_ptr<deducer<int>>> deducers{ deducer_ptr };
      solver_instances.push_back( solver_condition_instance<PuzzleInstanceType>( sci.get_field_indices(), deducers ));
    }
    return solver<PuzzleInstanceType>{ assignment, assignment.get_grid(), solver_instances };
  }
}
