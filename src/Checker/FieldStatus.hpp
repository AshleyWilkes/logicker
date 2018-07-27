//#include <algorithm>
#include <map>
#include <tuple>
#include <utility>
#include <vector>

/*namespace Logicker::Util {
  template<class ElemType>
  using Container = std::vector<ElemType>;
  template<class ElemType>
  using ElemIt = typename Container<ElemType>::iterator;
  template<class ElemType>
  class ElemRange {
    public:
      ElemRange(const ElemIt<ElemType>& begin, const ElemIt<ElemType>& end);
      ElemIt<ElemType> begin() const;
      ElemIt<ElemType> end() const;
    private:
      const ElemIt<ElemType> begin_;
      const ElemIt<ElemType> end_;
  };

  template<class ElemType>
  ElemRange<ElemType>::ElemRange(const ElemIt<ElemType>& begin, const ElemIt<ElemType>& end) : begin_{begin}, end_{end} {}

  template<class ElemType>
  ElemIt<ElemType> ElemRange<ElemType>::begin() const {
    return begin_;
  }

  template<class ElemType>
  ElemIt<ElemType> ElemRange<ElemType>::end() const {
    return end_;
  }
}*/

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

//this assumes each solution fiels contains exactly 1 value
//that's clearly not general enough, we can have empties
//
//the aforementioned assumption manifests in Field (as an abstraction
//between Grid and Value) missing
namespace Logicker::Checker {
  template<class Value, class FieldCoords>
  class SolutionGrid {
    public:
      SolutionGrid(const std::map<FieldCoords, Value>& grid_values);
      const Value& get_value(const FieldCoords& coords) const;
    private:
      const std::map<FieldCoords, Value>& vals_;
  };

  template<class Value, class FieldCoords>
  SolutionGrid<Value, FieldCoords>::SolutionGrid(const std::map<FieldCoords, Value>& grid_values) : vals_{grid_values} {}

  template<class Value, class FieldCoords>
  const Value& SolutionGrid<Value, FieldCoords>::get_value(const FieldCoords& coords) const {
    return vals_.find(coords)->second;
  }
}

//some preprocessing should be present in this (most importantly excluding empties,
//i.e. not putting empties into values array)
namespace Logicker::Checker {
  template<class Value, class Topology>
  class GridCondition {
    public:
      GridCondition(const Condition<Value>& cond, const std::vector<typename Topology::Coords>& fields);
      bool is_satisfied_by(const SolutionGrid<Value, typename Topology::Coords>& grid) const;
    private:
      const Condition<Value>& cond_;
      const std::vector<typename Topology::Coords> fields_;
  };

  template<class Value, class Topology>
  GridCondition<Value, Topology>::GridCondition(const Condition<Value>& cond, const std::vector<typename Topology::Coords>& fields) : cond_{cond}, fields_{fields} {}

  template<class Value, class Topology>
  bool
  GridCondition<Value, Topology>::is_satisfied_by(const SolutionGrid<Value, typename Topology::Coords>& grid) const {
    std::vector<Value> vals;
    //std::transform(fields_.begin(), fields_.end(), std::back_inserter(vals),
    //    [grid](const Topology::Coords& coord) { return grid.get_value(coord); });
    for (auto coord : fields_) {
      vals.push_back(grid.get_value(coord));
    }
    return cond_.is_satisfied_by(vals);
  }
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

      typename Topology::CoordsGroupRange get_all_coords_groups();
      typename Topology::CoordsRange get_coords_in_group(
          typename Topology::CoordsGroup group);
    private:
      typename Topology::Size size_;
  };

  template<class FieldType, class Topology>
  Grid<FieldType, Topology>::Grid(typename Topology::Size size) : size_{size} {}

  template<class FieldType, class Topology>
  typename Topology::CoordsGroupRange Grid<FieldType, Topology>::get_all_coords_groups() {
    return Topology::get_all_coords_groups(size_);
  }

  template<class FieldType, class Topology>
  typename Topology::CoordsRange Grid<FieldType, Topology>::get_coords_in_group(
      typename Topology::CoordsGroup group) {
    return Topology::get_coords_in_group(size_, group);
  }
}
