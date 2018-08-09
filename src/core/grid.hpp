#pragma once
#include "field.hpp"
#include <iostream>
#include <map>
#include <vector>

namespace logicker::core {
  //toto bude potomek abstraktniho typu deduction...
  template<class FieldType, class Topology>
  class elimination_deduction {
    public:
      const typename Topology::coords coords_;
      const typename FieldType::value_type value_;
  };

  template<class FieldType, class Topology>
  class grid;

  template<class FieldType, class Topology>
  std::ostream& operator<<(std::ostream& os, const grid<FieldType, Topology>& grid);

  template<class FieldType, class Topology>
  class grid {
    public:
      typedef Topology topology_type;
      using size_type = typename Topology::topology_size_t;
      //typedef typename Topology::size size_type;
      typedef typename Topology::coords coords_type;
      typedef typename FieldType::value_type value_type;
      typedef elimination_deduction<FieldType, Topology> deduction_type;

      grid(size_type size, const FieldType& field_type);

      field<FieldType>& get_field(coords_type coords);
      const field<FieldType>& get_field(coords_type coords) const;

      void set_values(std::vector<value_type>& values);
      void set_value(coords_type coords, value_type value);

      topology_type topology() const { return topology_; }

      void perform_deduction(elimination_deduction<FieldType, Topology> deduction);
      
      friend std::ostream& operator<< <>(std::ostream& os, const grid<FieldType, Topology>& grid);
    private:
      //size_type size_;
      topology_type topology_;//nebo nejaky pointer? ale proc?
      //std::map<coords_type, field<FieldType>> fields_;
      std::vector<field<FieldType>> fields_;
  };

  template<class FieldType, class Topology>
  grid<FieldType, Topology>::grid(size_type size, const FieldType& field_type) : topology_{size} {
    for (int i = 0; i < topology_.fields_count(); ++i) {
      fields_.push_back( field<FieldType>(field_type) );
    }
  }

  template<class FieldType, class Topology>
  field<FieldType>&
  grid<FieldType, Topology>::get_field(coords_type coords) {
    return fields_.at(coords.index());
  }

  template<class FieldType, class Topology>
  const field<FieldType>&
  grid<FieldType, Topology>::get_field(coords_type coords) const {
    return fields_.at(coords.index());
  }

  template<class FieldType, class Topology>
  void
  grid<FieldType, Topology>::set_values(std::vector<value_type>& values) {
    auto value_it = values.begin();
    for (int i = 0; i < topology_.fields_count(); ++i) {
      value_type& value = *value_it++;
      fields_.at(i).set( value );
    }
  }

  template<class FieldType, class Topology>
  void
  grid<FieldType, Topology>::set_value(coords_type coords, value_type value) {
    get_field(coords).set(value);
  }

  template<class FieldType, class Topology>
  void
  grid<FieldType, Topology>::perform_deduction(elimination_deduction<FieldType, Topology> deduction) {
    auto& field = get_field( deduction.coords_ );
    field.eliminate( deduction.value_ );
  }

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
