//#include <algorithm>
#include <boost/optional.hpp>
#include <map>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

//it is implicitly expected values form a linear structure, while that's
//not the case (the structure can be nonexistent (set of values when
//order doesn't matter) or nonlinear (e.g. square for e.g. loops))
namespace Logicker {
  template<class Value>
  class Condition {
    public:
      Condition(bool (*eval)(const std::vector<Value>&));
      bool is_satisfied_by(const std::vector<Value>& values) const;
    private:
      bool (*const eval_)(const std::vector<Value>&);
  };

  template<class Value>
  Condition<Value>::Condition(bool (*eval)(const std::vector<Value>&)) : eval_{eval} {};

  template<class Value>
  bool
  Condition<Value>::is_satisfied_by(const std::vector<Value>& values) const {
    return eval_(values);
  }

  auto neq_lambda = [](const std::vector<int>& values) {
    return values[0] != values[1];
  };

  Condition<int> neq {neq_lambda};
}

namespace Logicker::Topology {
  using CoordsMetaGroup = std::string;
  using Index = int;
  using Direction = bool;//true for normal direction, false for reverse

  static CoordsMetaGroup CMG_Rows = "Rows";
  static CoordsMetaGroup CMG_Cols = "Cols";
  static std::vector<CoordsMetaGroup> coords_meta_groups { CMG_Rows, CMG_Cols };

  class Rectangle {
    public:
      typedef std::pair<int, int> Size ;
      typedef std::pair<int, int> Coords ;
      typedef std::vector<Coords> CoordsRange ;
      typedef std::tuple<CoordsMetaGroup, Index, Direction> CoordsGroup ;
      typedef std::vector<CoordsGroup> CoordsGroupRange ;

      //tohle vsechno udelat static; topologie nebudou instanciovatelne, neni proc
      static CoordsRange get_all_coords(Size size);

      static CoordsGroupRange get_all_coords_groups(Size size);
      static CoordsRange get_coords_in_group(Size size, CoordsGroup group);

      static CoordsGroupRange get_coords_groups(Size size, CoordsMetaGroup cmg, Direction direction);
  };

  Rectangle::CoordsRange
  Rectangle::get_all_coords(Size size) {
    std::vector<Coords> result_vec;
    for (int row = 0; row < size.first; ++row) {
      for (int col = 0; col < size.second; ++col) {
        result_vec.push_back({row, col});
      }
    }
    return result_vec;
  }

  Rectangle::CoordsGroupRange
  Rectangle::get_all_coords_groups(Size size) {
    std::vector<CoordsGroup> result_vec;
    for (int i = 0; i < size.first; ++i) {
      result_vec.push_back( {CMG_Rows, i, true } );
    }
    for (int i = 0; i < size.second; ++i) {
      result_vec.push_back( {CMG_Cols, i, true } );
    }
    return result_vec;
  }

  Rectangle::CoordsRange
  Rectangle::get_coords_in_group(Size size, CoordsGroup group) {
    std::vector<Coords> result_vec;
    CoordsMetaGroup cmg{ std::get<0>(group) };
    Index index{ std::get<1>(group) };
    Direction dir{ std::get<2>(group) };
    if (cmg == CMG_Rows) {
      for (int col = 0; col < size.second; ++col) {
        result_vec.push_back({index, col});
      }
    } else if (cmg == CMG_Cols) {
      for (int row = 0; row < size.first; ++row) {
        result_vec.push_back({row, index});
      }
    }
    return result_vec;
  }
}

namespace Logicker {
  template<class FieldType, class Topology>
  class Grid {
    public:
      Grid(typename Topology::Size size);

      FieldType& get_field(typename Topology::Coords coords);
      const FieldType& get_field(typename Topology::Coords coords) const;

      void set_values(std::vector<int>& values);
    private:
      typename Topology::Size size_;
      std::map<typename Topology::Coords, FieldType> fields_;
  };

  template<class FieldType, class Topology>
  Grid<FieldType, Topology>::Grid(typename Topology::Size size) : size_{size} {
    for (auto coords : Topology::get_all_coords(size_)) {
      fields_[coords] = FieldType();
    }
  }

  template<class FieldType, class Topology>
  FieldType& Grid<FieldType, Topology>::get_field(typename Topology::Coords coords) {
    return fields_.at(coords);
  }

  template<class FieldType, class Topology>
  const FieldType& Grid<FieldType, Topology>::get_field(typename Topology::Coords coords) const {
    return fields_.at(coords);
  }

  template<class FieldType, class Topology>
  void Grid<FieldType, Topology>::set_values(std::vector<int>& values) {
    auto all_coords = Topology::get_all_coords(size_);
    for (auto coords_it = all_coords.begin(), values_it = values.begin();
        coords_it != all_coords.end(); ++coords_it, ++values_it) {
      fields_[*coords_it].set(*values_it);
    }
  }
}

namespace Logicker {
  template<int min, int max>
  class IntegerField {
    public:
      typedef int ValueType;

      void set(int value);
      int get() const;
    private:
      boost::optional<int> set_value_ { boost::none };
  };

  template<int min, int max>
  void
  IntegerField<min, max>::set(int value) {
    if (set_value_) {
      throw std::domain_error("field already set");
    } else if (value < min || value > max) {
      throw std::domain_error("value out of range");
    } else {
      set_value_ = value;
    }
  }

  template<int min, int max>
  int
  IntegerField<min, max>::get() const {
    if (set_value_) {
      return set_value_.get();
    } else {
      throw std::domain_error("value not set");
    }
  }
}

//some preprocessing should be present in this (most importantly excluding empties,
//i.e. not putting empties into values array)
namespace Logicker::Checker {
  template<class FieldType, class Topology>
  class GridCondition {
    public:
      GridCondition(const Condition<typename FieldType::ValueType>& cond, const std::vector<typename Topology::Coords>& fields);
      bool is_satisfied_by(const Grid<FieldType, Topology>& grid) const;
    private:
      const Condition<typename FieldType::ValueType>& cond_;
      const std::vector<typename Topology::Coords> fields_;
  };

  template<class FieldType, class Topology>
  GridCondition<FieldType, Topology>::GridCondition(const Condition<typename FieldType::ValueType>& cond, const std::vector<typename Topology::Coords>& fields) : cond_{cond}, fields_{fields} {}

  //this assumes each solution field contains exactly 1 value
  //that's clearly not general enough, we can have empties
  template<class FieldType, class Topology>
  bool
  GridCondition<FieldType, Topology>::is_satisfied_by(const Grid<FieldType, Topology>& grid) const {
    std::vector<typename FieldType::ValueType> vals;
    //std::transform(fields_.begin(), fields_.end(), std::back_inserter(vals),
    //    [grid](const Topology::Coords& coord) { return grid.get_field(coord).get(); });
    for (auto coord : fields_) {
      vals.push_back(grid.get_field(coord).get());
    }
    return cond_.is_satisfied_by(vals);
  }
}
