#pragma once

namespace logicker::core::checker {
  template<class ValueType, class Topology, class Conditions>
  struct checker_traits {
    using field_t = core::field_type<ValueType>;
    using topology_t = Topology;
  };

  template<class ValueType, class Topology, class Conditions>
  class checker_factory {
    public:
      typename checker_traits<ValueType, Topology, Conditions>::field_t create_field(const core::input_node_base& input); 
      typename checker_traits<ValueType, Topology, Conditions>::topology_t create_topology(const core::input_node_base& input);
  };

  template<class Conditions>
  class checker_factory<int, core::rectangle, Conditions> {
    public:
      typename checker_traits<int, core::rectangle, Conditions>::field_t create_field(const core::input_node_base& input);
      typename checker_traits<int, core::rectangle, Conditions>::topology_t create_topology(const core::input_node_base& input);
  };

  template<class Conditions>
  typename checker_traits<int, core::rectangle, Conditions>::field_t
  checker_factory<int, core::rectangle, Conditions>::create_field(const core::input_node_base& input) {
    throw "checker_factory::create_field<rectangle> not impled";
  }

  template<class Conditions>
  typename checker_traits<int, core::rectangle, Conditions>::topology_t
  checker_factory<int, core::rectangle, Conditions>::create_topology(const core::input_node_base& input) {
    throw "checker_factory::create_topology<rectangle> not impled";
  }

  template<class Conditions>
  class checker_factory<int, core::square, Conditions> {
    public:
      typename checker_traits<int, core::square, Conditions>::field_t create_field(const core::input_node_base& input);
      typename checker_traits<int, core::square, Conditions>::topology_t create_topology(const core::input_node_base& input);
  };

  template<class Conditions>
  typename checker_traits<int, core::square, Conditions>::field_t
  checker_factory<int, core::square, Conditions>::create_field(const core::input_node_base& input) {
    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    const int_input_node& size_input = cast_input.get<int_input_node>("Size");
    int size = size_input.get();
    return typename checker_traits<int, core::square, Conditions>::field_t{ 1, size };
  }

  template<class Conditions>
  typename checker_traits<int, core::square, Conditions>::topology_t
  checker_factory<int, core::square, Conditions>::create_topology(const core::input_node_base& input) {
    const composite_input_node& cast_input = dynamic_cast<const composite_input_node&>( input );
    const int_input_node& size_input = cast_input.get<int_input_node>("Size");
    int size = size_input.get();
    return typename checker_traits<int, core::square, Conditions>::topology_t{ size };
  }
}
