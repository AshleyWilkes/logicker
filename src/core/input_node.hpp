#pragma once
#include <json/json.h>
#include <map>
#include <memory>
#include <string>

namespace logicker::core {
  class input_node_base {
    public:
      input_node_base() = default;
      //??mam dobre tyhle deklarace??
      input_node_base(const input_node_base&) = delete;
      input_node_base(input_node_base&&) = delete;
      input_node_base& operator=(const input_node_base&) = delete;
      input_node_base& operator=(input_node_base&&) = delete;
      virtual ~input_node_base() = default;

      virtual std::string name() const = 0;

      static std::shared_ptr<input_node_base> json_to_node( std::string name, const Json::Value& value );
  };

  class composite_input_node : public input_node_base {
    public:
      composite_input_node(std::string name, std::map<std::string, std::shared_ptr<input_node_base>> parts)
          : name_{ name }, parts_{ std::move(parts) } {}
      composite_input_node(std::string name, const Json::Value& value);
      composite_input_node(const composite_input_node& rhs) : name_{ rhs.name_ }, parts_{ rhs.parts_ } {}
      composite_input_node& operator=(composite_input_node&& rhs) { name_ = std::move( rhs.name_ ); parts_ = std::move( rhs.parts_ ); }
      std::string name() const override { return name_; }

      template<class target_input_node>
      const target_input_node& get(std::string name) const;

      typedef std::map<std::string, std::shared_ptr<input_node_base>> map;
      typedef map::const_iterator iterator;
      iterator begin() const { return parts_.begin(); }
      iterator end() const { return parts_.end(); }
    private:
      std::string name_;
      map parts_;
  };

  class int_input_node : public input_node_base {
    public:
      int_input_node(std::string name, int value) : name_{ name }, value_{ value } {}
      int_input_node(const int_input_node& rhs) : name_{ rhs.name_ }, value_{ rhs.value_ } {}
      int_input_node& operator=(int_input_node&& rhs) { name_ = std::move( rhs.name_ ); value_ = rhs.value_; }
      std::string name() const override { return name_; }
      int get() const { return value_; }
    private:
      std::string name_;
      int value_;
  };

  std::shared_ptr<input_node_base>
  input_node_base::json_to_node( std::string name, const Json::Value& value ) {
    if ( value.isObject() || value.isArray() ) {
      return std::make_shared<composite_input_node>(composite_input_node{ name, value });
    } else if ( value.isInt() ) {
      return std::make_shared<int_input_node>(int_input_node{ name, value.asInt() });
    } else {
      throw "input_node_base::json_to_node() error: unsupported value type";
    }
  }

  composite_input_node::composite_input_node(std::string name, const Json::Value& value) : name_{ name } {
    if ( value.isObject() ) {
      for ( auto member_name : value.getMemberNames() ) {
        parts_.insert({ member_name, input_node_base::json_to_node( member_name, value[member_name] ) });
      }
    } else if ( value.isArray() ) {
      for ( int i = 0; i < value.size(); ++i ) {
        std::string name{ "#" + i };
        parts_.insert({ name, input_node_base::json_to_node( name, value[i] ) });
      }
    } else {
      throw "composite_input_node::constr() error: unsupported value type";
    }
  }

  template<class target_input_node>
  const target_input_node&
  composite_input_node::get(std::string name) const {
    auto it = parts_.find(name);
    if (it != parts_.end()) {
      const input_node_base* base_ptr = it->second.get();
      return dynamic_cast<const target_input_node&>(*base_ptr);
    } else {
      throw std::string("part named \"") + name + "\" not found in node \"" + name_ + "\"";
    }
  }
}
