#pragma once
#include <memory>
#include <string>
#include <vector>

namespace logicker::core::constraint {
  using constraint_status_message = std::vector<std::string>;
  class constraint_status {
    public:
      void add_message( const constraint_status_message& msg ) { msgs_.push_back( msg ); }
      void merge( const constraint_status& other, bool merge_broken ) { 
        msgs_.insert( msgs_.begin(), other.msgs_.begin(), other.msgs_.end() );
        if ( merge_broken ) {
          is_broken_ |= other.is_broken_;
        }
      }
      void set_broken( bool broken ) { is_broken_ = broken; }
      bool is_broken() const { return is_broken_; }
      friend std::ostream& operator<<( std::ostream& os, const constraint_status& status );
    private:
      std::vector<constraint_status_message> msgs_;
      bool is_broken_ = false;
  };

  std::ostream& operator<<( std::ostream& os, const constraint_status& status ) {
    std::string prefix;
    for ( auto msg : status.msgs_ ) {
      prefix = "";
      for ( auto str : msg ) {
        os << prefix << str << '\n';
        prefix = "    ";
      }
    }
    return os;
  }

  template<typename Grid>
  class constraint {
    public:
      virtual constraint_status is_satisfied_by( const Grid& grid ) const = 0;
      virtual ~constraint() = default;
  };
}
