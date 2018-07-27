//#include <algorithm>
#include <map>
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
  template<class Value, class FieldCoords>
  class GridCondition {
    public:
      GridCondition(const Condition<Value>& cond, const std::vector<FieldCoords>& fields);
      bool is_satisfied_by(const SolutionGrid<Value, FieldCoords>& grid) const;
    private:
      const Condition<Value>& cond_;
      const std::vector<FieldCoords>& fields_;
  };

  template<class Value, class FieldCoords>
  GridCondition<Value, FieldCoords>::GridCondition(const Condition<Value>& cond, const std::vector<FieldCoords>& fields) : cond_{cond}, fields_{fields} {}

  template<class Value, class FieldCoords>
  bool
  GridCondition<Value, FieldCoords>::is_satisfied_by(const SolutionGrid<Value, FieldCoords>& grid) const {
    std::vector<Value> vals;
    //std::transform(fields_.begin(), fields_.end(), std::back_inserter(vals),
    //    [grid](const FieldCoords& coord) { return grid.get_value(coord); });
    for (auto coord : fields_) {
      vals.push_back(grid.get_value(coord));
    }
    return cond_.is_satisfied_by(vals);
  }
}

namespace Logicker::Topology {
  template<class ElemType>
  using Container = std::vector<ElemType>;
  template<class ElemType>
  using ElemIt = typename Container<ElemType>::iterator;
  template<class ElemType>
  using ElemRange = std::pair<ElemIt<ElemType>, ElemIt<ElemType>>;

  using CoordsMetaGroup = std::string;
  using Index = int;
  using Direction = bool;//true for normal direction, false for reverse

  using CoordsGroup = std::tuple<CoordsMetaGroup, Index, Direction>;
  using CoordsGroupIt = Container<CoordsGroup>::iterator;
  using CoordsGroupRange = std::pair<CoordsGroupIt, CoordsGroupIt>;

  static CoordsMetaGroup CMG_Rows = "Rows";
  static CoordsMetaGroup CMG_Cols = "Cols";
  static std::vector<CoordsMetaGroup> coords_meta_groups { CMG_Rows, CMG_Cols };

  class Rectangle {
    public:
      typedef std::pair<int, int> Coords ;
      typedef ElemRange<Coords> CoordsRange ;

      //tohle vsechno udelat static; topologie nebudou instanciovatelne, neni proc
      Rectangle(int rows, int cols);
      CoordsRange get_all_coords();

      CoordsGroupRange get_all_coords_groups();
      CoordsRange get_coords_in_group(CoordsGroup group);

      CoordsGroupRange get_coords_groups(CoordsMetaGroup, Direction);
    
  };
}

namespace Logicker {
  template<class FieldType, class Topology>
  class Grid {
    public:
      Grid(int num_rows, int num_cols);
  };
}
