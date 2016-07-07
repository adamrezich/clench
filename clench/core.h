#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "mem.h"

namespace clench {

  namespace core {

    enum class Proptype {
      Bool,
      Int,
      Float,
      String
    };

    enum class Functype {
      Native,
      Stench
    };

    struct Val {
      Proptype type;
      union {
        bool val_bool;
        int val_int;
        float val_float;
      };
      std::string val_string;

      std::string ToString() const;

      bool operator==(const Val &rhs) const;
      static Val Bool(bool value);
      static Val Int(int value);
      static Val Float(float value);
      static Val String(const std::string &value);
    };

    struct Propdef {
      std::string name;
      Val default;
      Propdef(const std::string &name_, Val default_) : name(name_), default(default_) {}
    };

    struct Prop {
      Propdef *propdef;
      Val value;

      bool IsDefault() const;
    };

    struct Funcdef {

    };

    struct Func {

    };

    struct Elem;

    struct Compdef {
      std::string name;
      std::list<Propdef> propdefs;

      static Compdef *Define(std::string name, std::list<Propdef> propdefs);

      Compdef() {}

    private:
      Compdef(std::string name_, std::list<Propdef> propdefs_) : name(name_), propdefs(propdefs_) {}
    };

    struct Comp {
      const Compdef *compdef;
      const Elem *owner;
      std::unordered_map<std::string, Val> props;

      Comp(Compdef *compdef_, Elem *owner_);

      const Val &operator[](const std::string &propName) const;
      Val &operator[](const std::string &propName);
    };

    struct Elem {
      std::string name;

      Elem *parent;
      std::list<Elem *> children;

      Elem *elemdef = nullptr;

      std::unordered_map<std::string, Comp *> comps;

      Elem *Append();
      Elem *Append(std::string name);
      Elem *Append(Elem *elem);

      Comp *Attach(Compdef *compdef);
      Comp *Attach(Compdef *compdef, std::unordered_map<std::string, Val> propvals);

      Comp *GetComp(const std::string &compName);

      Elem *CloneTo(Elem *elem) const;

      void Destroy();

      void Dump(int indent = 0) const;

      std::string GetPath();

      Comp &operator[](const std::string &compName);


      Comp *GetRel(const std::string &compName);

    };

    struct Pkg {
      std::unordered_map<std::string, Compdef> compdefs;
      std::unordered_map<std::string, Elem> elemdefs;

      Pkg() {
        compdefs.reserve(128);
        elemdefs.reserve(128);
      }
    };

    struct App : Pkg {
      Elem root;

      App() {
        root.parent = nullptr;
        root.name = "_root";
        compdefs.reserve(512);
        elemdefs.reserve(512);
      }
    };

  }

  extern core::App app;

  /// Get things going
  inline void Initialize() {
  }

}
