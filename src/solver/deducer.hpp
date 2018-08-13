#pragma once
#include <algorithm>
#include <boost/optional.hpp>
#include <vector>

namespace logicker::core {
  template<class ValueType>
  struct basic_deduction {
    int index_;
    ValueType value_;
  };

  template<class ValueType>
  class deducer {
    public:
      virtual boost::optional<basic_deduction<ValueType>> try_deduce(std::vector<field<field_type<ValueType>>> fields) const = 0;
  };

  template<class ValueType>
  class neq_deducer : public deducer<ValueType> {
    public:
      boost::optional<basic_deduction<ValueType>> try_deduce(std::vector<field<field_type<ValueType>>> fields) const override;
  };

  template<class ValueType>
  boost::optional<basic_deduction<ValueType>>
  neq_deducer<ValueType>::try_deduce(std::vector<field<field_type<ValueType>>> fields) const {
    if (fields.size() != 2) {
      throw "neq_deducer::try_deduce -- only 2 fields deductioning impled";
    }
    //tady predpokladam, ze kdyz je field is_set, tak obsahuje hodnotu, ono vubec integer field je na pikacu
    field<field_type<ValueType>> f1 = fields[0];
    field<field_type<ValueType>> f2 = fields[1];
    if (f1.is_set() && !f2.is_set() && f2.is_value_option(f1.get())) {
      return basic_deduction<ValueType>{ 1, f1.get() };
    } else if (f2.is_set() && !f1.is_set() && f1.is_value_option(f2.get())) {
      return basic_deduction<ValueType>{ 0, f2.get() };
    }
    return boost::none;
  }

  /*template<class ValueType>
  class generic_deducer : public deducer<ValueType> {
    public:
      //toz tady je treba urcit, ??kolik policek mam ocekavat??
      //a jak nad nimi dedukovat
      //
      //genericky deducer ma pro kazdou hodnotu ve fieldech zjistit,
      //jestli existuje nejaka kombinace s hodnotami v ostatnich fieldech,
      //ktera vyhovuje vsem podminkam
      //
      //tj. udelam si "mapu" pripustnych hodnot v jednotlivych polickach, neco jako bitset
      //na zacatku jsou vsechny bity 0
      //vyberu si nejakou hodnotu a pokusim se najit reseni vsech podminek,
      //kdyz bude platit tahle hodnota.
      //1. optimalizace: vsechny ostatni hodnoty v nalezenem reseni lze rovnou taky
      //oznacit za pripustne
      //2. optimalizace: do jinych policek vybirat prednostne hodnoty,
      //ktere jeste nejsou rozhodnute
      //
      generic_deducer(std::vector<condition<ValueType>> conds);
      boost::optional<basic_deduction<ValueType>> try_deduce(std::vector<field<field_type<ValueType>>> fields) const override;
    private:
      std::vector<condition<ValueType>> conds_;
  };

  //tak napred udelejme variantu s dvema policky:
  template<class ValueType>
  generic_deducer<ValueType>::generic_deducer(std::vector<condition<ValueType>> conds) : conds_{conds} {}

  template<class ValueType>
  boost::optional<basic_deduction<ValueType>>
  generic_deducer<ValueType>::try_deduce(std::vector<field<field_type<ValueType>>> fields) const {
    //tupe, zcela bez optimalizace
    for (ValueType val1 : fields.at(0).available_values()) {
      if (std::none_of(fields.at(1).available_values().begin(), fields.at(1).available_values.end(), 
            [val1, conds_ = this->conds_](const ValueType& val2){
                  std::vector<ValueType> vals{ val1, val2 };
                  for (auto cond : conds_) {
                    if ( cond.is_satisfied_by( vals ) ) {
                      return true;
                    }
                  }
                  return false;
            })) {
        return { 0, val1 };
      }
    }
    return boost::none;

    //a tady totez jeste jednou, jen s prehozenymi field1, field2
  }*/
}
