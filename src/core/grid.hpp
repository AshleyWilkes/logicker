#pragma once
#include "field.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

namespace logicker::core {
  //toto bude potomek abstraktniho typu deduction...
  template<class FieldType>
  class elimination_deduction {
    public:
      int field_index_;
      const typename FieldType::value_type value_;
  };

  template<class FieldType>
  class field_container {
    public:
      typedef typename FieldType::value_type value_type;
      typedef elimination_deduction<FieldType> deduction_type;
      field_container(int fields_count, const FieldType& field_type);

      field<FieldType>& get_field(int index);
      const field<FieldType>& get_field(int index) const;

      void set_values(const std::vector<value_type>& values);
      void set_value(int index, const value_type& value);

      //each field must contain exactly 1 value, this is not nearly general enough
      //ok for now
      std::vector<value_type> get_values() const;

      void clear();

      bool is_subset_of(const field_container& other) const;

      void perform_deduction(elimination_deduction<FieldType> deduction);
    private:
      std::vector<field<FieldType>> fields_;
  };

  template<class FieldType, class Topology>
  class grid;

  template<class FieldType, class Topology>
  std::ostream& operator<<(std::ostream& os, const grid<FieldType, Topology>& grid);

  template<class FieldType, class Topology>
  class grid : public field_container<FieldType> {
    public:
      typedef Topology topology_type;
      using size_type = typename Topology::topology_size_t;
      typedef typename Topology::coords coords_type;
      typedef typename FieldType::value_type value_type;

      grid(const Topology& topology, const FieldType& field_type);

      //tenhle using by tu nemel byt, jim importovane metody
      //by mel pouzivat jenom grid a tam, kde je pouziva,
      //se daji kvalifikovat naprimo
      using field_container<FieldType>::get_field;
      field<FieldType>& get_field(coords_type coords);
      const field<FieldType>& get_field(coords_type coords) const;

      void set_value(coords_type coords, value_type value);

      topology_type topology() const { return topology_; }

      bool is_subset_of(const grid& other) const;
      
      friend std::ostream& operator<< <>(std::ostream& os, const grid<FieldType, Topology>& grid);
    private:
      topology_type topology_;//nebo nejaky pointer? ale proc?
  };

  template<class FieldType>
  field_container<FieldType>::field_container(int fields_count, const FieldType& field_type) {
    for (int i = 0; i < fields_count; ++i) {
      fields_.push_back( field<FieldType>(field_type) );
    }
  }

  template<class FieldType>
  field<FieldType>&
  field_container<FieldType>::get_field(int index) {
    return fields_.at( index );
  }

  template<class FieldType>
  const field<FieldType>&
  field_container<FieldType>::get_field(int index) const {
    return fields_.at( index );
  }

  template<class FieldType>
  void
  field_container<FieldType>::set_values( const std::vector<value_type>& values ) {
    for (size_t i = 0; i < fields_.size(); ++i) {
      set_value( i, values.at( i ) );
    }
  }

  template<class FieldType>
  void
  field_container<FieldType>::set_value(int index, const value_type& value) {
    fields_.at(index).set(value);
  }

  template<class FieldType>
  std::vector<typename FieldType::value_type>
  field_container<FieldType>::get_values() const {
    std::vector<value_type> result;
    for (size_t i = 0; i < fields_.size(); ++i) {
      result.push_back( fields_.at(i).get() );
    }
    return result;
  }

  template<class FieldType>
  void
  field_container<FieldType>::clear() {
    std::for_each( fields_.begin(), fields_.end(),
        [](auto& field){
            field.clear();
        });
  }

  template<class FieldType>
  bool
  field_container<FieldType>::is_subset_of(const field_container& other) const {
    assert( fields_.size() == other.fields_.size() );
    auto mismatch = std::mismatch( fields_.begin(), fields_.end(),
        other.fields_.begin(), other.fields_.end(),
        [](const field<FieldType>& field1, const field<FieldType>& field2) { 
            return field1.is_subset_of( field2 );
        });
    return mismatch.first == fields_.end();
  }

  template<class FieldType>
  void
  field_container<FieldType>::perform_deduction(elimination_deduction<FieldType> deduction) {
    auto& field = get_field( deduction.field_index_ );
    field.eliminate( deduction.value_ );
  }

  template<class FieldType, class Topology>
  grid<FieldType, Topology>::grid(const Topology& topology, const FieldType& field_type) : field_container<FieldType>{topology.fields_count(), field_type}, topology_{topology} {}

  template<class FieldType, class Topology>
  field<FieldType>&
  grid<FieldType, Topology>::get_field(coords_type coords) {
    return get_field( coords.index() );
  }

  template<class FieldType, class Topology>
  const field<FieldType>&
  grid<FieldType, Topology>::get_field(coords_type coords) const {
    return get_field( coords.index() );
  }

  /*template<class FieldType, class Topology>
  void
  grid<FieldType, Topology>::set_values(std::vector<value_type>& values) {
    auto value_it = values.begin();
    for (int i = 0; i < topology_.fields_count(); ++i) {
      value_type& value = *value_it++;
      fields_.at(i).set( value );
    }
  }*/

  template<class FieldType, class Topology>
  void
  grid<FieldType, Topology>::set_value(coords_type coords, value_type value) {
    get_field(coords).set(value);
  }

  template<class FieldType, class Topology>
  bool
  grid<FieldType, Topology>::is_subset_of(const grid& other) const {
    if ( topology_ == other.topology() ) {
      return field_container<FieldType>::is_subset_of( other );
    }
    return false;
  }

  /*template<class FieldType, class Topology>
  std::vector<typename grid<FieldType, Topology>::value_type>
  grid<FieldType, Topology>::get_values() {
    std::vector<value_type> result;
    for (int i = 0; i < topology_.fields_count(); ++i) {
      result.push_back( fields_.at(i).get() );
    }
    return result;
  }*/

  template<class FieldType, class Topology>
  std::ostream&
  operator<<(std::ostream& os, const grid<FieldType, Topology>& grid) {
    for (auto row : grid.topology().get_all_coords_groups({ "Rows" })) {
      for (auto coords : grid.topology().get_coords_in_group(row)) {
        os << grid.get_field(coords) << " ";
        //auto field = grid.get_field(coords);
        //os << field.get() << " ";
      }
      os << '\n';
    }
    return os;
  }
}
