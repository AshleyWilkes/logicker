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
      typedef typename Topology::size size_type;
      typedef typename Topology::coords coords_type;
      typedef typename FieldType::value_type value_type;
      typedef elimination_deduction<FieldType, Topology> deduction_type;

      grid(size_type size, const FieldType& field_type);

      field<FieldType>& get_field(coords_type coords);
      const field<FieldType>& get_field(coords_type coords) const;

      void set_values(std::vector<value_type>& values);
      void set_value(coords_type coords, value_type value);

      size_type size() const;
      
      friend std::ostream& operator<< <>(std::ostream& os, const grid<FieldType, Topology>& grid);
    private:
      size_type size_;
      std::map<coords_type, field<FieldType>> fields_;
  };

  template<class FieldType, class Topology>
  grid<FieldType, Topology>::grid(size_type size, const FieldType& field_type) : size_{size} {
    for (auto coords : Topology::get_all_coords(size_)) {
      fields_.insert({ coords, field<FieldType>(field_type) });
    }
  }

  template<class FieldType, class Topology>
  field<FieldType>& grid<FieldType, Topology>::get_field(coords_type coords) {
    return fields_.at(coords);
  }

  template<class FieldType, class Topology>
  const field<FieldType>& grid<FieldType, Topology>::get_field(coords_type coords) const {
    return fields_.at(coords);
  }

  template<class FieldType, class Topology>
  void grid<FieldType, Topology>::set_values(std::vector<value_type>& values) {
    auto all_coords = Topology::get_all_coords(size_);
    for (auto coords_it = all_coords.begin(), values_it = values.begin();
        coords_it != all_coords.end(); ++coords_it, ++values_it) {
      set_value(*coords_it, *values_it);
    }
  }

  template<class FieldType, class Topology>
  void grid<FieldType, Topology>::set_value(coords_type coords, value_type value) {
    fields_.at(coords).set(value);
  }

  template<class FieldType, class Topology>
  typename grid<FieldType, Topology>::size_type
  grid<FieldType, Topology>::size() const {
    return size_;
  }

  template<class FieldType, class Topology>
  std::ostream&
  operator<<(std::ostream& os, const grid<FieldType, Topology>& grid) {
    for (auto row : Topology::get_all_coords_groups(grid.size(), { "Rows" })) {
      for (auto coords : Topology::get_coords_in_group(grid.size(), row)) {
        os << grid.get_field(coords) << " ";
        //auto field = grid.get_field(coords);
        //os << field.get() << " ";
      }
      os << '\n';
    }
    return os;
  }
}
