#pragma once
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
  };

  class composite_input_node : public input_node_base {
    public:
      composite_input_node(std::string name, std::map<std::string, std::shared_ptr<input_node_base>> parts)
          : name_{ name }, parts_{ std::move(parts) } {}
      std::string name() const override { return name_; }
      input_node_base* get(std::string name) const { return parts_.find(name)->second.get(); }
    private:
      std::string name_;
      std::map<std::string, std::shared_ptr<input_node_base>> parts_;
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
}
