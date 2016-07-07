// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLENCH_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLENCH_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CLENCH_EXPORTS
#define CLENCH_API __declspec(dllexport)
#else
#define CLENCH_API __declspec(dllimport)
#endif

// This class is exported from the clench.dll
class CLENCH_API Cclench {
public:
  Cclench(void);
  // TODO: add your methods here.
};

extern CLENCH_API int nclench;

CLENCH_API void test(void);

#pragma once
#include <list>
#include <string>
#include <unordered_map>
#include "mem.h"

struct SDL_Window;
struct SDL_Renderer;

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

    struct CLENCH_API Val {
      Proptype type;
      union {
        bool val_bool;
        int val_int;
        float val_float;
      };
      std::string val_string;

      const char *ToString() const;

      bool operator==(const Val &rhs) const;
      static Val Bool(bool value);
      static Val Int(int value);
      static Val Float(float value);
      static Val String(const char *value);
    };

    struct CLENCH_API Propdef {
      const char *name;
      Val default;
      Propdef(const char *name_, Val default_);
    };

    struct CLENCH_API Prop {
      Propdef *propdef;
      Val value;

      bool IsDefault() const;
    };

    struct CLENCH_API Funcdef {

    };

    struct CLENCH_API Func {

    };

    struct Elem;

    struct CLENCH_API Compdef : clench::mem::Managed<Compdef, 1024> {
      const char *name;
      std::list<Propdef> propdefs;

      static Compdef *Define(const char *name, std::list<Propdef> propdefs);

      Compdef();
      ~Compdef();
      void Cleanup();

    private:
      Compdef(const char *name_, std::list<Propdef> propdefs_);
    };

    struct CLENCH_API Comp {
      const Compdef *compdef;
      const Elem *owner;
      std::unordered_map<std::string, Val> props;

      Comp(Compdef *compdef_, Elem *owner_);

      const Val &operator[](const std::string &propName) const;
      Val &operator[](const std::string &propName);
    };

    struct CLENCH_API Elem {
      std::string name;

      Elem *parent;
      std::list<Elem *> children;

      Elem *elemdef = nullptr;

      std::unordered_map<std::string, Comp *> comps;

      Elem *Append();
      Elem *Append(const char *name);
      Elem *Append(Elem *elem);

      Comp *Attach(Compdef *compdef);
      Comp *Attach(Compdef *compdef, std::unordered_map<std::string, Val> propvals);
      Comp *Attach(const char *compdefName);

      Comp *GetComp(const char *compName);

      Elem *CloneTo(Elem *elem) const;

      void Destroy();

      void Dump(int indent = 0) const;

      std::string GetPath();

      Comp &operator[](const char *compName);


      Comp *GetRel(const char *compName);

    };

    struct CLENCH_API Pkg {
      friend struct Compdef;
      friend struct Elem;
      Pkg();

    protected:
      std::unordered_map<std::string, Compdef> compdefs;
      std::unordered_map<std::string, Elem> elemdefs;
    };

    struct CLENCH_API App : Pkg {
      Elem root;
      SDL_Window *window;
      SDL_Renderer *renderer;

      App();
      void BeginInit();
      void EndInit();
      void Begin();

    private:
      void Tick(float dt);
    };

  }

  extern CLENCH_API core::App app;

}



