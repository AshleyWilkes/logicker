#pragma once
#include "condition_instance.hpp"
#include <set>

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

      solver_condition_instance(std::vector<int> field_indices, std::vector<simple_condition_instance<field_type>> conds) : fields_set_{ field_indices.begin(), field_indices.end() }, conds_vec_{ conds } {}
      //void add(const simple_condition_instance<field_type, topology_type>& sci) { conds_set_.insert( sci ); }
      //
      //pokud vratis boost::none, nastav processed nebo exhausted na true
      boost::optional<deduction_type> try_to_find_deduction(grid_type grid);
      boost::optional<deduction_type> try_to_find_deduction(simple_condition_instance<field_type> cond, grid_type grid);

      bool is_processed() const { return processed_; }
      bool is_exhausted() const { return exhausted_; }
      size_t fields_set_size() const { return fields_set_.size(); }
      std::set<int> fields_set() { return fields_set_; }
      void set_unprocessed() { processed_ = false; }

      static bool compare_conditions(const solver_condition_instance<PuzzleInstanceType>& lhs, const solver_condition_instance<PuzzleInstanceType>& rhs);
    private:
      std::set<int> fields_set_;
      std::vector<simple_condition_instance<field_type>> conds_vec_;
      bool processed_{ false }, exhausted_{ false };
  };

  template<class PuzzleInstanceType>
  boost::optional<typename PuzzleInstanceType::deduction_type>
  solver_condition_instance<PuzzleInstanceType>::try_to_find_deduction(typename PuzzleInstanceType::grid_type grid) {
    for ( auto simple_cond : conds_vec_ ) {
      auto deduction_opt = try_to_find_deduction( simple_cond, grid );
      if ( deduction_opt ) {
        return deduction_opt;
      }
    }
    processed_ = true;
    return boost::none;
  }

  //ma byt rozdeleno na dve casti
  //prvni cast vybere z vectoru policka odpovidajici indexum, ktere se conditiony
  //tykaji
  //
  //druha cast na nich provede prislusne dedukce
  //
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /*template<class FieldType>
  boost::optional<typename field_container<FieldType>::deduction_type>
  simple_condition_instance<FieldType>::try_to_find_deduction(const field_container<FieldType>& field_container) const {
    if (fields_.size() != 2) {
      throw "simple_condition_instance::try_to_find_deduction -- only 2 fields deductioning impled";
    }
    //tady predpokladam, ze kdyz je field is_set, tak obsahuje hodnotu, ono vubec integer field je na pikacu
    field<FieldType> f1 = field_container.get_field(fields_[0]);
    field<FieldType> f2 = field_container.get_field(fields_[1]);
    if (f1.is_set() && !f2.is_set() && f2.is_value_option(f1.get())) {
      return typename field_container<FieldType>::deduction_type { fields_[1], f1.get() };
    } else if (f2.is_set() && !f1.is_set() && f1.is_value_option(f2.get())) {
      return typename field_container<FieldType>::deduction_type { fields_[0], f2.get() };
    }
    return boost::none;
  }*/
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  template<class PuzzleInstanceType>
  boost::optional<typename PuzzleInstanceType::deduction_type>
  solver_condition_instance<PuzzleInstanceType>::try_to_find_deduction(simple_condition_instance<field_type> cond, grid_type grid) {
    auto indices = cond.get_field_indices();
    if (indices.size() != 2) {
      throw "solver_condition_instance::try_to_find_deduction -- only 2 fields deductioning impled";
    }
    //tady predpokladam, ze kdyz je field is_set, tak obsahuje hodnotu, ono vubec integer field je na pikacu
    field<field_type> f1 = grid.get_field(indices[0]);
    field<field_type> f2 = grid.get_field(indices[1]);
    if (f1.is_set() && !f2.is_set() && f2.is_value_option(f1.get())) {
      return typename field_container<field_type>::deduction_type { indices[1], f1.get() };
    } else if (f2.is_set() && !f1.is_set() && f1.is_value_option(f2.get())) {
      return typename field_container<field_type>::deduction_type { indices[0], f2.get() };
    }
    return boost::none;
  }
  
  template<class PuzzleInstanceType>
  bool
  solver_condition_instance<PuzzleInstanceType>::compare_conditions(const solver_condition_instance<PuzzleInstanceType>& lhs, const solver_condition_instance<PuzzleInstanceType>& rhs) {
    if ( lhs.is_processed() || lhs.is_exhausted() ) return false;
    if ( rhs.is_processed() || rhs.is_exhausted() ) return true;
    //prozatim pouzivam pro jednoduchost pouze primarni kriterium
    return lhs.fields_set_size() < rhs.fields_set_size();
  }

  template<class PuzzleInstanceType>
  class solver {
    public:
      typedef typename PuzzleInstanceType::coords_type coords_type;
      typedef typename PuzzleInstanceType::field_type field_type;
      typedef typename PuzzleInstanceType::grid_type grid_type;
      typedef typename PuzzleInstanceType::topology topology_type;
      using deduction_type = typename PuzzleInstanceType::deduction_type;
      //solver(PuzzleInstanceType&& assignment);
      solver(PuzzleInstanceType assignment, grid_type working_grid, std::vector<solver_condition_instance<PuzzleInstanceType>> solver_condition_instances) : assign_{ assignment }, working_grid_{ working_grid }, cond_insts_ { solver_condition_instances } {}
      typename PuzzleInstanceType::grid_type get_solution() const;
    private:
      solver_condition_instance<PuzzleInstanceType>& pick_a_condition() const;

      const PuzzleInstanceType assign_;
      mutable typename PuzzleInstanceType::grid_type working_grid_;
      mutable std::vector<solver_condition_instance<PuzzleInstanceType>> cond_insts_;
  };

  template<class PuzzleInstanceType>
  class solver_factory {
    public:
      typedef typename PuzzleInstanceType::field_type field_type;
      typedef typename PuzzleInstanceType::topology topology_type;

      solver<PuzzleInstanceType> create_solver_for_assignment(PuzzleInstanceType assignment);
    private:
      boost::optional<solver_condition_instance<PuzzleInstanceType>> solver_condition_instance_for_field_set(std::set<typename PuzzleInstanceType::coords_type> field_set);

      std::vector<simple_condition_instance<typename PuzzleInstanceType::field_type>> simple_instances_;
  };

  /*template<class PuzzleInstanceType>
  solver<PuzzleInstanceType>::solver(PuzzleInstanceType&& assignment) : assign_{ std::move(assignment) }, working_grid_{ assign_.get_grid() } {}*/

  template<class PuzzleInstanceType>
  typename PuzzleInstanceType::grid_type
  solver<PuzzleInstanceType>::get_solution() const {
    //
    //potrebuju container (asi multimapu) mapujici policko->vsechny deducing_instance,
    //    ktere policko ovlivnuji; jeho pomoci pri kazde eliminaci oznacim
    //    podminky patrici k eliminovanemu policku jako nezpracovane
    //
    //postup solveni je repeat steps:
    //1) pick a deducing_instance, ask it for a deduction
    //    najdi podminku, ktera umi dedukovat, neni processed a je minimalni
    //    podle primarniho i sekundarniho porovnavaciho kriteria
    //2) if a deduction was found, perform it on the working grid
    //
    if ( assign_.get_condition_instances().empty() ) {
      return working_grid_;
    }
    //std::cout << "There are " << cond_insts_.size() << " solver_cond_instances\n";

    //for (solver_condition_instance<PuzzleInstanceType>& cond2 = this->pick_a_condition(); !cond2.is_processed() && !cond2.is_exhausted(); cond2 = this->pick_a_condition()) {
    while (true) {
      auto& cond{this->pick_a_condition()};
      if (cond.is_processed() || cond.is_exhausted()) break;
      auto deduction_opt = cond.try_to_find_deduction( working_grid_ );
      if ( deduction_opt ) {
        //throw "solver::get_solution() error: using deduction not impled";
        //perform the deduction on the working_grid_
        auto deduction = deduction_opt.get();
        working_grid_.perform_deduction( deduction );
        //wake up appropriate solver_instances
        for ( auto it = cond_insts_.begin(); it != cond_insts_.end(); ++it ) {
          if ( it->fields_set().count( deduction.field_index_ ) == 1) {
            it->set_unprocessed();
          }
        }
      }
    }
    return working_grid_;
  }

  template<class PuzzleInstanceType>
  solver<PuzzleInstanceType>
  solver_factory<PuzzleInstanceType>::create_solver_for_assignment(PuzzleInstanceType assignment) {
    simple_instances_ = assignment.get_simple_condition_instances();
    std::vector<solver_condition_instance<PuzzleInstanceType>> solver_instances;
    //iterovat over podmnoziny mnoziny vsech policek, pro kazdou
    //  zavolat solver_condition_instance_for_field_set
    //  pokud vracena hodnota neni boost::none, vrazit ji do solver_instances
    //
    //iterovat pres podmnoziny je nonsense, milostiva; uz pro normalni sudoku je
    //  takovych podmnozin 2^81. Coz je dost hodne, je treba na to jit jinak
    //
    //Zadani: mam prvky a_i, z nich se skladaji mnoziny A_i. Vstupem je sada mnozin A_i,
    //vystupem je sada mnozin XX tak, ze XX == A_i pro nejake i anebo 
    //kazdy prvek XX lezi v alespon dvou A_i takovych, ze A_i subset XX.
    //
    //podminka s tim, ze kazde policko musi byt soucasti alespon 2 elementaries
    //je taky spatne, napr. kriz o peti polickach ve tvaru znamenka plus do solveni
    //sudoku nic nepridava prave tak jako tri policka v pravem uhlu, ale touto
    //podminkou by prosel
    //
    //Zda se, ze toto (rozhodnout, pro ktere skupiny policek vytvaret solver_instance)
    //je velmi tezky problem, nechavam ted byt a jdu nejjednodussi cestou:
    //vytvorim jeden solver_instance pro kazdou elementary_instance a jinak zatim
    //nic. Jako dalsi krok se jevi bud dotvorit teorii natolik, ze by se sem
    //dal opravdu pripsat generator solver_instanci, jak jsem to chtel udelat,
    //anebo ??do condition_instance.hpp?? pripsat mechanismus, jak solver_instance
    //generovat rucne.
    for ( auto sci : simple_instances_ ) {
      std::vector<simple_condition_instance<field_type>> conds;
      conds.push_back( sci );
      solver_instances.push_back( solver_condition_instance<PuzzleInstanceType>( sci.get_field_indices(), conds ));
    }
    return solver<PuzzleInstanceType>{ assignment, assignment.get_grid(), solver_instances };
  }

//chci mit jednu solver_condition_instanco pro kazdou skupinu policek,
//pro kterou to ma smysl (viz diskuse

  template<class PuzzleInstanceType>
  solver_condition_instance<PuzzleInstanceType>&
  solver<PuzzleInstanceType>::pick_a_condition() const {
    typename std::vector<solver_condition_instance<PuzzleInstanceType>>::iterator best_cond_it = std::min_element( cond_insts_.begin(), cond_insts_.end(), solver_condition_instance<PuzzleInstanceType>::compare_conditions );
    return *best_cond_it;
  }
}
