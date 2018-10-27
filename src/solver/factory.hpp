/*#pragma once

namespace logicker::core::solver {
  template<class ValueType, class Topology, class Conditions>
  struct solver_traits {
    using field_t = core::field_type<ValueType>;
    using topology_t = Topology;
    using grid_t = core::grid<field_t, topology_t>;
    using deduction_t = typename grid_t::deduction_type;
    //using condition_t = solver_condition_instance<P
  };

  template<class ValueType, class Topology, class Conditions>
  class solver_factory {
    public:
      typename solver_traits<ValueType, Topology, Conditions>::field_t create_field(const core::input_node_base& input);
      typename solver_traits<ValueType, Topology, Conditions>::topology_t create_topology(const core::input_node_base& input);
  };
}*/
