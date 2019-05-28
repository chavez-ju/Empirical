/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2019
 *
 *  @file  VarMap.h
 *  @brief VarMaps track arbitrary data by name (slow) or id (faster).
 *  @note Status: ALPHA
 */

#ifndef EMP_VAR_MAP_H
#define EMP_VAR_MAP_H

#include <string>

#include "../base/assert.h"
#include "../base/Ptr.h"
#include "../base/unordered_map.h"
#include "../base/vector.h"
#include "../meta/TypeID.h"
#include "../tools/map_utils.h"
#include "../tools/string_utils.h"

namespace emp {

  class VarMap {
  private:
    struct VarBase {
      std::string name;                              ///< Name of this variable.
      size_t type_value;                             ///< Unique ID for the underlying type.

      VarBase(const std::string & in_name, size_t in_type_value)
      : name(in_name), type_value(in_type_value) { ; }
    };

    template <typename T>
    struct VarInfo : public VarBase {
      T value;                                       ///< Current value of this variable.

      VarInfo(const std::string & name, T && in_value)
      : VarBase(name, GetTypeValue<T>())
      , value( std::forward<T>(in_value) )
      { ; }
    };

    emp::vector<emp::Ptr<VarBase>> vars;             ///< Vector of all current variables.
    emp::unordered_map<std::string, size_t> id_map;  ///< Map of names to vector positions.

  public:
    VarMap() { ; }

    const std::string & GetName(size_t id) const { return vars[id]->name; }
    size_t GetID(const std::string & name) const { return Find(id_map, name, (size_t) -1); }

    template <typename T>
    size_t Add(const std::string & name, T && value) {
      emp_assert( emp::Has(id_map, name) == false );
      const size_t id = vars.size();
      emp::Ptr<VarInfo<T>> new_ptr = NewPtr<VarInfo<T>>( name, std::forward<T>(value) );
      vars.push_back(new_ptr);
      id_map[name] = id;
      return id;
    }

    size_t AddString(const std::string & name, const std::string & value) {
      return Add<std::string>(name, value);
    }

    size_t AddInt(const std::string & name, int value) {
      return Add<int>(name, value);
    }

    size_t AddDouble(const std::string & name, double value) {
      return Add<double>(name, value);
    }

    size_t AddChar(const std::string & name, char value) {
      return Add<char>(name, value);
    }

    size_t AddBool(const std::string & name, bool value) {
      return Add<bool>(name, value);
    }

    template <typename T>
    T & Get(size_t id) {
      emp_assert(id < vars.size());
      emp_assert(vars[id].type_value = emp::GetTypeValue<T>());
      emp::Ptr<VarInfo<T>> ptr = vars[id].Cast<VarInfo<T>>();
      return ptr->value;
    }

    template <typename T>
    T & Get(const std::string & name) {
      emp_assert( emp::Has(id_map, name) );
      const size_t id = id_map[name];
      return Get<T>(id);
    }

    template <typename T>
    const T & Get(size_t id) const {
      emp_assert(id < vars.size());
      emp_assert(vars[id].type_value = emp::GetTypeValue<T>());
      emp::Ptr<VarInfo<T>> ptr = vars[id].Cast<VarInfo<T>>();
      return ptr->value;
    }

    template <typename T>
    const T & Get(const std::string & name) const {
      emp_assert( emp::Has(id_map, name) );
      const size_t id = emp::Find(id_map, name, (size_t) -1);
      return Get<T>(id);
    }

    // Accessors    
    std::string & GetString(const std::string & name) { return Get<std::string>(name); }
    int & GetInt(const std::string & name) { return Get<int>(name); }
    double & GetDouble(const std::string & name) { return Get<double>(name); }
    char & GetChar(const std::string & name) { return Get<char>(name); }
    bool & GetBool(const std::string & name) { return Get<bool>(name); }

    // Const accessors
    const std::string & GetString(const std::string & name) const { return Get<std::string>(name); }
    int GetInt(const std::string & name) const { return Get<int>(name); }
    double GetDouble(const std::string & name) const { return Get<double>(name); }
    char GetChar(const std::string & name) const { return Get<char>(name); }
    bool GetBool(const std::string & name) const { return Get<bool>(name); }
  };

}

#endif
