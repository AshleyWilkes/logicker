#pragma once
#include <map>
#include <vector>

namespace logicker::core {
  template<class FieldType, class Topology>
  class grid {
    public:
      grid(typename Topology::size size);

      FieldType& get_field(typename Topology::coords coords);
      const FieldType& get_field(typename Topology::coords coords) const;

      void set_values(std::vector<int>& values);
    private:
      typename Topology::size size_;
      std::map<typename Topology::coords, FieldType> fields_;
  };

  template<class FieldType, class Topology>
  grid<FieldType, Topology>::grid(typename Topology::size size) : size_{size} {
    for (auto coords : Topology::get_all_coords(size_)) {
      fields_[coords] = FieldType();
    }
  }

  template<class FieldType, class Topology>
  FieldType& grid<FieldType, Topology>::get_field(typename Topology::coords coords) {
    return fields_.at(coords);
  }

  template<class FieldType, class Topology>
  const FieldType& grid<FieldType, Topology>::get_field(typename Topology::coords coords) const {
    return fields_.at(coords);
  }

  template<class FieldType, class Topology>
  void grid<FieldType, Topology>::set_values(std::vector<int>& values) {
    auto all_coords = Topology::get_all_coords(size_);
    for (auto coords_it = all_coords.begin(), values_it = values.begin();
        coords_it != all_coords.end(); ++coords_it, ++values_it) {
      fields_[*coords_it].set(*values_it);
    }
  }
}
