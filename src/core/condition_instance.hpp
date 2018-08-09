#pragma once
#include "condition.hpp"
#include "grid.hpp"
#include <algorithm>
#include <boost/optional.hpp>
#include <memory>
#include <vector>

//novy, lepsi plan, jezto tento neni dost flexibilni a zaroven je ponekud overengineered
//to co mam, je vlastne spravne: hierarchie cond_instanci je jednourovnova: kazda slovy popsatelna podminka kombinovana policky (aka named_instance) je reprezentovana jednou composite_instanci, ktera jako sve childy obsahuje vyhradne simple_instance (aka elementary_instance);
//
//pro checkera je to plne dostacujici a take already implementovano
//
//solver si na zaklade dodanych named_instanci a jejich elementary_instanci vytvori deducing_instance (drive zvane tez wrapper); deducing_instance je mnozina policek + *vsechny* elementary_instance, jejichz mnoziny policek jsou podmnozinami teto mnoziny; deducing instance vytvorim pro kazdou mnozinu policek, pro kterou to ma smysl (viz dale), spravne udelan tento pristup umoznuje backtrackem generovat komplikovanejsi uvahy nez plynou z jedine elementary_instance a zaroven udrzet deducing_instance pouze nezbytne velike a tedy backtrackovat zbytecne mnoho
//
//podmnozina mnoziny vsech policek ulohy ma mit svou deducing_instanci, pokud je splnena jedna ze dvou podminek:
//1) pokud mnozina je prave mnozina policek nejake elementary_instance, 
//2) pokud kazde jeji policko je soucasti alespon dvou elementary_instanci, jejichz vsechna policka jsou soucasti zvazovane mnoziny (mnoziny policek kazde elemntary_instance jsou podmnozinami zvazovane mnoziny)
//
//
//
//btw poznamka: predstavme si mrakodrapy 7x7, ktere maji pro nektery smer zadanou hodnou 7. Pak je jiste, ze v polickach musi byt vzestupne hodnoty 1..7. Jak tohle udelat, aby se vysledek poznal bez backtrackovani vsech 7! (resp 7^7) moznosti?
//cesta snad vede pres neviditelne extra hodnoty v polickach: pro kazdy smer bude v policku 1, pokud je v danem smeru obsazene viditelnou budovou, 0 pokud neviditelnou, a podminkama bude prolinkovany vyznam vsech podminek. proted je to prilis pokrocila zalezitost, napred neco snadnejsiho, napr kodovane obrazky?

//some preprocessing should be present in this (most importantly excluding empties,
//i.e. not putting empties into values array)
namespace logicker::core {
  template<class FieldType, class Topology>
  class simple_condition_instance;

  template<class FieldType, class Topology>
  class condition_instance {
    public:
      virtual bool is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const = 0;
      virtual boost::optional<typename grid<FieldType, Topology>::deduction_type>
        try_to_find_deduction(const grid<FieldType, Topology>& grid) const = 0;
      virtual std::vector<simple_condition_instance<FieldType, Topology>>
        get_as_simple_instances_vector() const = 0;
      
      static std::unique_ptr<condition_instance>
      create_instance(std::string, const std::vector<typename Topology::coords>& fields);
  };

  template<class FieldType, class Topology>
  class simple_condition_instance : public condition_instance<FieldType, Topology> {
    public:
      simple_condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<typename Topology::coords>& fields);

      const std::vector<typename Topology::coords>& get_coords() const { return fields_; }
      bool is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const override;
      boost::optional<typename grid<FieldType, Topology>::deduction_type>
        try_to_find_deduction(const grid<FieldType, Topology>& grid) const override;
      virtual std::vector<simple_condition_instance<FieldType, Topology>>
        get_as_simple_instances_vector() const override;
    private:
      condition<typename FieldType::value_type> cond_;
      std::vector<typename Topology::coords> fields_;
  };

  template<class FieldType, class Topology>
  class composite_condition_instance : public condition_instance<FieldType, Topology> {
    public:
      void add(std::unique_ptr<condition_instance<FieldType, Topology>> part);

      bool is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const override;
      boost::optional<typename grid<FieldType, Topology>::deduction_type>
        try_to_find_deduction(const grid<FieldType, Topology>& grid) const override;
      virtual std::vector<simple_condition_instance<FieldType, Topology>>
        get_as_simple_instances_vector() const override;
    private:
      std::vector<std::shared_ptr<condition_instance<FieldType, Topology>>> parts_;
  };

  template<class FieldType, class Topology>
  simple_condition_instance<FieldType, Topology>::simple_condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<typename Topology::coords>& fields) : cond_{cond}, fields_{fields} {}

  template<class FieldType, class Topology>
  bool 
  simple_condition_instance<FieldType, Topology>::is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const {
    std::vector<typename FieldType::value_type> cond_vals;
    for (auto coord : fields_) {
      cond_vals.push_back(values.at(coord.index()));
    }
    return cond_.is_satisfied_by(cond_vals);
  }

  template<class FieldType, class Topology>
  boost::optional<typename grid<FieldType, Topology>::deduction_type>
  simple_condition_instance<FieldType, Topology>::try_to_find_deduction(const grid<FieldType, Topology>& grid) const {
    if (fields_.size() != 2) {
      throw "simple_condition_instance::try_to_find_deduction -- only 2 fields deductioning impled";
    }
    //tady predpokladam, ze kdyz je field is_set, tak obsahuje hodnotu, ono vubec integer field je na pikacu
    field<FieldType> f1 = grid.get_field(fields_[0]);
    field<FieldType> f2 = grid.get_field(fields_[1]);
    if (f1.is_set() && !f2.is_set() && f2.is_value_option(f1.get())) {
      return typename grid<FieldType, Topology>::deduction_type { fields_[1], f1.get() };
    } else if (f2.is_set() && !f1.is_set() && f1.is_value_option(f2.get())) {
      return typename grid<FieldType, Topology>::deduction_type { fields_[0], f2.get() };
    }
    return boost::none;
  }

  template<class FieldType, class Topology>
  std::vector<simple_condition_instance<FieldType, Topology>>
  simple_condition_instance<FieldType, Topology>::get_as_simple_instances_vector() const {
    std::vector<simple_condition_instance<FieldType, Topology>> result;
    result.push_back(*this);
    return result;
  }

  template<class FieldType, class Topology>
  void
  composite_condition_instance<FieldType, Topology>::add(std::unique_ptr<condition_instance<FieldType, Topology>> part) {
    parts_.push_back(std::move(part));
  }

  template<class FieldType, class Topology>
  bool 
  composite_condition_instance<FieldType, Topology>::is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const {
    for (auto it = parts_.begin(); it != parts_.end(); ++it) {
      if (! (*it)->is_satisfied_by(values)) {
        return false;
      }
    }
    return true;
  }

  template<class FieldType, class Topology>
  boost::optional<typename grid<FieldType, Topology>::deduction_type>
  composite_condition_instance<FieldType, Topology>::try_to_find_deduction(const grid<FieldType, Topology>& grid) const {
    for (auto it = parts_.begin(); it != parts_.end(); ++it) {
      auto potential_deduction = (*it)->try_to_find_deduction(grid);
      if (potential_deduction) {
        return potential_deduction;
      }
    }
    return boost::none;
  }

  template<class FieldType, class Topology>
  std::vector<simple_condition_instance<FieldType, Topology>>
  composite_condition_instance<FieldType, Topology>::get_as_simple_instances_vector() const {
    std::vector<simple_condition_instance<FieldType, Topology>> result;
    for ( auto& part : parts_ ) {
      auto part_result = (*part).get_as_simple_instances_vector();
      result.insert( result.end(), part_result.begin(), part_result.end() );
    }
    return result;
  }

  template<class FieldType, class Topology>
  std::unique_ptr<condition_instance<FieldType, Topology>>
  condition_instance<FieldType, Topology>::create_instance(core::ConditionDescription desc, const std::vector<typename Topology::coords>& fields) {
    std::unique_ptr<composite_condition_instance<FieldType, Topology>> result = std::make_unique<composite_condition_instance<FieldType, Topology>>();
    if (desc == "EachValueOnce") {
      int fields_size = fields.size();
      for (int i = 0; i < fields_size; ++i) {
        for (int j = i + 1; j < fields_size; ++j) {
          std::vector<typename Topology::coords> temp_fields{ fields.at(i), fields.at(j) };
          auto temp_instance = std::make_unique<simple_condition_instance<FieldType, Topology>>( neq, temp_fields );
          //result.add(std::make_unique<simple_condition_instance<FieldType, Topology>>(temp_instance));
          result->add(std::move(temp_instance));
        }
      }
    } else {
      throw "condition::create_instance: unknown description";
    }
    return result;
  }
}
