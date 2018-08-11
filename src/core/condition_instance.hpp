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
  template<class FieldType>
  class simple_condition_instance;

  template<class FieldType>
  class condition_instance {
    public:
      virtual bool is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const = 0;
      virtual boost::optional<typename field_container<FieldType>::deduction_type>
        try_to_find_deduction(const field_container<FieldType>& field_container) const = 0;
      virtual std::vector<simple_condition_instance<FieldType>>
        get_as_simple_instances_vector() const = 0;
      
      static std::unique_ptr<condition_instance>
      create_instance(std::string, const std::vector<int>& fields);
  };

  template<class FieldType>
  class simple_condition_instance : public condition_instance<FieldType> {
    public:
      simple_condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<int>& field_indices);

      const std::vector<int>& get_field_indices() const { return fields_; }
      bool is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const override;
      boost::optional<typename field_container<FieldType>::deduction_type>
        try_to_find_deduction(const field_container<FieldType>& field_container) const override;
      virtual std::vector<simple_condition_instance<FieldType>>
        get_as_simple_instances_vector() const override;
    private:
      condition<typename FieldType::value_type> cond_;
      std::vector<int> fields_;
  };

  template<class FieldType>
  class composite_condition_instance : public condition_instance<FieldType> {
    public:
      void add(std::unique_ptr<condition_instance<FieldType>> part);

      bool is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const override;
      boost::optional<typename field_container<FieldType>::deduction_type>
        try_to_find_deduction(const field_container<FieldType>& field_container) const override;
      virtual std::vector<simple_condition_instance<FieldType>>
        get_as_simple_instances_vector() const override;
    private:
      std::vector<std::shared_ptr<condition_instance<FieldType>>> parts_;
  };

  template<class FieldType>
  simple_condition_instance<FieldType>::simple_condition_instance(const condition<typename FieldType::value_type>& cond, const std::vector<int>& field_indices) : cond_{cond}, fields_{field_indices} {}

  template<class FieldType>
  bool 
  simple_condition_instance<FieldType>::is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const {
    std::vector<typename FieldType::value_type> cond_vals;
    for (auto index : fields_) {
      cond_vals.push_back(values.at(index));
    }
    return cond_.is_satisfied_by(cond_vals);
  }

  template<class FieldType>
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
  }

  template<class FieldType>
  std::vector<simple_condition_instance<FieldType>>
  simple_condition_instance<FieldType>::get_as_simple_instances_vector() const {
    std::vector<simple_condition_instance<FieldType>> result;
    result.push_back(*this);
    return result;
  }

  template<class FieldType>
  void
  composite_condition_instance<FieldType>::add(std::unique_ptr<condition_instance<FieldType>> part) {
    parts_.push_back(std::move(part));
  }

  template<class FieldType>
  bool 
  composite_condition_instance<FieldType>::is_satisfied_by(const std::vector<typename FieldType::value_type>& values) const {
    for (auto it = parts_.begin(); it != parts_.end(); ++it) {
      if (! (*it)->is_satisfied_by(values)) {
        return false;
      }
    }
    return true;
  }

  template<class FieldType>
  boost::optional<typename field_container<FieldType>::deduction_type>
  composite_condition_instance<FieldType>::try_to_find_deduction(const field_container<FieldType>& field_container) const {
    for (auto it = parts_.begin(); it != parts_.end(); ++it) {
      auto potential_deduction = (*it)->try_to_find_deduction(field_container);
      if (potential_deduction) {
        return potential_deduction;
      }
    }
    return boost::none;
  }

  template<class FieldType>
  std::vector<simple_condition_instance<FieldType>>
  composite_condition_instance<FieldType>::get_as_simple_instances_vector() const {
    std::vector<simple_condition_instance<FieldType>> result;
    for ( auto& part : parts_ ) {
      auto part_result = (*part).get_as_simple_instances_vector();
      result.insert( result.end(), part_result.begin(), part_result.end() );
    }
    return result;
  }

  template<class FieldType>
  std::unique_ptr<condition_instance<FieldType>>
  condition_instance<FieldType>::create_instance(core::ConditionDescription desc, const std::vector<int>& fields) {
    std::unique_ptr<composite_condition_instance<FieldType>> result = std::make_unique<composite_condition_instance<FieldType>>();
    if (desc == "EachValueOnce") {
      int fields_size = fields.size();
      for (int i = 0; i < fields_size; ++i) {
        for (int j = i + 1; j < fields_size; ++j) {
          std::vector<int> temp_fields{ fields.at(i), fields.at(j) };
          auto temp_instance = std::make_unique<simple_condition_instance<FieldType>>( neq, temp_fields );
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
