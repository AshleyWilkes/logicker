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
      typedef simple_condition_instance<field_type, topology_type> condition_type;
      using deduction_type = typename PuzzleInstanceType::deduction_type;
      using grid_type = typename PuzzleInstanceType::grid_type;

      solver_condition_instance(std::vector<coords_type> coords, std::vector<simple_condition_instance<field_type, topology_type>> conds) : coords_set_{ coords.begin(), coords.end() }, conds_vec_{ conds } {}
      //void add(const simple_condition_instance<field_type, topology_type>& sci) { conds_set_.insert( sci ); }
      //
      //pokud vratis boost::none, nastav processed nebo exhausted na true
      boost::optional<deduction_type> try_to_find_deduction(grid_type grid);

      bool is_processed() const { return processed_; }
      bool is_exhausted() const { return exhausted_; }
      size_t coords_set_size() const { return coords_set_.size(); }
      std::set<coords_type> coords_set() { return coords_set_; }
      void set_unprocessed() { processed_ = false; }

      static bool compare_conditions(const solver_condition_instance<PuzzleInstanceType>& lhs, const solver_condition_instance<PuzzleInstanceType>& rhs);
    private:
      std::set<coords_type> coords_set_;
      std::vector<simple_condition_instance<field_type, topology_type>> conds_vec_;
      bool processed_{ false }, exhausted_{ false };
  };

  template<class PuzzleInstanceType>
  boost::optional<typename PuzzleInstanceType::deduction_type>
  solver_condition_instance<PuzzleInstanceType>::try_to_find_deduction(typename PuzzleInstanceType::grid_type grid) {
    for ( auto simple_cond : conds_vec_ ) {
      auto deduction_opt = simple_cond.try_to_find_deduction( grid );
      if ( deduction_opt ) {
        return deduction_opt;
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
    return lhs.coords_set_size() < rhs.coords_set_size();
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
      void init_cond_insts() const;
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

      std::vector<simple_condition_instance<typename PuzzleInstanceType::field_type, typename PuzzleInstanceType::topology>> simple_instances_;
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
          if ( it->coords_set().count( deduction.coords_ ) == 1) {
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
    //vytvorim jeden solver_instance pro kazdou elementary_instance a jinak  zatim
    //nic. Jako dalsi krok se jevi bud dotvorit teorii natolik, ze by se sem
    //dal opravdu pripsat generator solver_instanci, jak jsem to chtel udelat,
    //anebo do condition_instance.hpp pripsat mechanismus, jak solver_instance
    //generovat rucne.
    for ( auto sci : simple_instances_ ) {
      std::vector<simple_condition_instance<field_type, topology_type>> conds;
      conds.push_back( sci );
      solver_instances.push_back( solver_condition_instance<PuzzleInstanceType>( sci.get_coords(), conds ));
    }
    return solver<PuzzleInstanceType>{ assignment, assignment.get_grid(), solver_instances };
  }

  template<class PuzzleInstanceType>
  void
  solver<PuzzleInstanceType>::init_cond_insts() const {
    //
    //nejdriv potrebuju sebrat vsechny simple_condition_instance, ktere existuji
    //
    //z nich si postavim deducing_instance:
    //  ??mozna umi nahradit si interne rozhodnute policko primo za hodnotu v nem??
    //  urcite umi hledat backtrackem dedukce: dvojici [policko, hodnota], o ktere
    //    se da rozhodnout, ze nemuze byt spravne
    //  bool processed, exhausted;
    //  int num_fields;//primarni porovnavaci kriterium
    //
    //
    //  int state_space_size;//sekundarni porovnavaci kriterium
    //novy, lepsi postup:
    //prochazim vsechny podmnoziny mnoziny vsech policek
    //pro kazdou podmnozinu rozhodnu, zda zaslouzi mit deducing_instanci
    //  ??(1) existuje simple_instance s presne touto mnozinou policek??
    //  pro kazde policko
    //    ??(2) kolik simple_instanci, ktere obsahuji toto policko, se vejde do teto mnoziny??
    //pokud ano, vytvorim deducing_instanci a umistim do ni (odkazy na) vsechny simple_instance, ktere do ni patri
    //  (3) ktere simple_instance se vejdou do teto mnoziny
    //
    //Takze se nam to zuzuje takto:
    //-- je treba zkoumat vlastnosti mnozin policek, a to dva druhy vlastnosti
    //--    je tato mnozina stejna jako jina mnozina (pro bod 1)
    //--    je tato mnozina podmnozinou jine mnoziny (pro body 2, 3)
    //-- odpovedi na obe otazky urcite umi poskytnout std container set<coords>
    //
    //Takze implementace teto metody obsahuje tyto kroky:
    //1) sebrat informace o vsech simple_conditionach do datove struktury
    //    to udela assign_.get_simple_condition_instances();
    //2) generovat podmnoziny mnoziny vsech policek
    //  3) pro kazdou podmnozinu vyhodnotit podminku (1), tj. projit vsechny simply a pro kady se podivat, jestli nahodou nema mnozinu stejnou jako aktualni podmnozina
    //  4) pokud (1) neplati, pro kazde policko z podmnoziny
    //    5) 
    //
    /*std::map<std::set<coords_type>, solver_condition_instance<PuzzleInstanceType>> temp_insts_;
    //??iterate over simple_condition_instances in assign_??
    for ( const simple_condition_instance<field_type, topology_type>& sci : assign_.get_simple_condition_instances() ) {
      auto& sci_coords_vec = sci.get_coords();
      std::set<coords_type> coords{ sci_coords_vec.begin(), sci_coords_vec.end() };
      temp_insts_[coords].add( sci );
      std::cout << "Iteration over sci\n";
    }*/
  }

  template<class PuzzleInstanceType>
  solver_condition_instance<PuzzleInstanceType>&
  solver<PuzzleInstanceType>::pick_a_condition() const {
    typename std::vector<solver_condition_instance<PuzzleInstanceType>>::iterator best_cond_it = std::min_element( cond_insts_.begin(), cond_insts_.end(), solver_condition_instance<PuzzleInstanceType>::compare_conditions );
    return *best_cond_it;
  }
}
