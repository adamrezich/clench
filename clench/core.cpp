#include "stdafx.h"
#include "clench.h"
#include <iostream>
#include <SDL.h>

namespace clench {

  namespace core {

    // VAL //
    bool Val::operator==(const Val &rhs) const {
      if (type != rhs.type)
        return false;
      return true; // TODO: Actually compare the two
    }
    const char *Val::ToString() const {
      switch (type) {
      case Proptype::Bool:
        return val_bool ? "True" : "False";
      case Proptype::Int:
        return std::to_string(val_int).c_str();
      case Proptype::Float:
        return std::to_string(val_float).c_str();
      case Proptype::String:
        return val_string.c_str();
      }
      throw std::exception("somehow a val got corrupted");
    }
    Val Val::Bool(bool value) {
      Val val;
      val.type = Proptype::Bool;
      val.val_bool = value;
      return val;
    }
    Val Val::Int(int value) {
      Val val;
      val.type = Proptype::Int;
      val.val_int = value;
      return val;
    }
    Val Val::Float(float value) {
      Val val;
      val.type = Proptype::Float;
      val.val_float = value;
      return val;
    }
    Val Val::String(const char *value) {
      Val val;
      val.type = Proptype::String;
      val.val_string = value;
      return val;
    }

    // PROPDEF //
    Propdef::Propdef(const char *name_, Val default_) : name(name_), default(default_) {}

    // PROP //
    bool Prop::IsDefault() const {
      return value == propdef->default;
    }

    // COMPDEF //
    Compdef::Compdef() {}
    Compdef::~Compdef() {}
    void Compdef::Cleanup() {}
    Compdef::Compdef(const char *name_, std::list<Propdef> propdefs_) : name(name_), propdefs(propdefs_) {}
    Compdef *Compdef::Define(const char *name, std::list<Propdef> propdefs) {
      app.compdefs.insert({ name, Compdef(name, propdefs) });
      return &(app.compdefs[name]);
    }

    // COMP //
    Comp::Comp(Compdef *compdef_, Elem *owner_) : compdef(compdef_), owner(owner_) {
      for (auto &propdef : compdef->propdefs) {
        props.insert({ propdef.name, propdef.default });
      }
    }

    const Val &Comp::operator[](const std::string &propName) const {
      auto prop = props.find(propName);
      if (prop == props.end())
        throw std::exception("comp does not contain prop");
      else
        return prop->second;
    }

    Val &Comp::operator[](const std::string &propName) {
      auto prop = props.find(propName);
      if (prop == props.end())
        throw std::exception("comp does not contain prop");
      else
        return prop->second;
    }

    // ELEM //
    Elem *Elem::Append() {
      Elem *elem = new Elem();
      elem->parent = this;
      children.push_back(elem);
      return elem;
    }

    Elem *Elem::Append(const char *name) {
      Elem *elem = new Elem();
      elem->parent = this;
      elem->name = name;
      children.push_back(elem);
      return elem;
    }

    Elem *Elem::Append(Elem *elem) {
      elem->parent = this;
      children.push_back(elem);
      return elem;
    }

    Comp *Elem::Attach(Compdef *compdef) {
      Comp *comp = new Comp(compdef, this);
      comps.insert({ compdef->name, comp });
      return comp;
    }

    Comp *Elem::Attach(Compdef *compdef, std::unordered_map<std::string, Val> propvals) {
      Comp *comp = new Comp(compdef, this);
      for (auto &propval : propvals) {
        if (comp->props.find(propval.first) == comp->props.end())
          throw std::exception("tried to set value of prop which was not in the propdef");
        comp->props[propval.first] = propval.second;
      }
      comps.insert({ compdef->name, comp });
      return comp;
    }

    Comp *Elem::Attach(const char *compdefName) {
      //Compdef *compdef = app.compdefs.find(compdefName);
      return nullptr;
    }

    Elem *Elem::CloneTo(Elem *dest) const {
      Elem *elem = new Elem();
      elem->name = name;
      elem->comps = comps; // scary...!
      for (auto &child : children) {
        child->CloneTo(elem);
      }
      dest->Append(elem);
      return elem;
    }

    void Elem::Destroy() {
      // TODO
    }

    void Elem::Dump(int indent) const {
      int indentScale = 2;
      std::cout << std::string(indent * indentScale, ' ') << name << std::endl;
      ++indent;
      if (!comps.empty()) {
        std::cout << std::string(indent * indentScale, ' ') << "[" << std::endl;
        ++indent;
        for (auto comp : comps) {
          std::cout << std::string(indent * indentScale, ' ') << comp.second->compdef->name;

          if (!comp.second->props.empty()) {
            std::cout << " {" << std::endl;
            ++indent;
            for (auto prop : comp.second->props) {
              std::cout << std::string(indent * indentScale, ' ') << prop.first;
              std::cout << " : " << prop.second.ToString() << std::endl;
            }
            --indent;
            std::cout << std::string(indent * indentScale, ' ') << '}' << std::endl;
          }
          else {
            std::cout << std::endl;
          }
        }
        --indent;
        std::cout << std::string(indent * indentScale, ' ') << ']' << std::endl;
      }
      if (!children.empty()) {

        for (auto child : children) {
          child->Dump(indent);
        }
      }
      --indent;
    }

    std::string Elem::GetPath() {
      return "TODO";
    }

    Comp *Elem::GetComp(const char *compName) {
      auto comp = comps.find(compName);
      if (comp != comps.end())
        return comp->second;
      return nullptr;
    }

    Comp &Elem::operator[](const char *compName) {
      Comp *comp = GetComp(compName);
      if (!comp)
        throw std::exception("comp not found in elem");
      return *comp;
    }

    Comp *Elem::GetRel(const char *compName) {
      Comp *comp = GetComp(compName);
      if (!comp && parent) {
        comp = parent->GetRel(compName);
      }
      return comp;
    }

    // PKG //
    Pkg::Pkg() {
      compdefs.reserve(128);
      elemdefs.reserve(128);
    }

    // APP //
    App::App() {
      root.parent = nullptr;
      root.name = "_root";
      compdefs.reserve(512);
      elemdefs.reserve(512);
    }

    void App::BeginInit() {
      std::cout << "Beginning init..." << std::endl;
    }

    void App::EndInit() {
      std::cout << "Init ended." << std::endl;
    }

    void App::Begin() {
      SDL_Init(SDL_INIT_VIDEO);

      window = SDL_CreateWindow("clench", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
      renderer = SDL_CreateRenderer(window, -1, 0);

      while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) {
            break;
          }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, 32, 32, 128, 128);

        SDL_RenderPresent(renderer);
      }

      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }

    void App::Tick(float dt) {
      
    }

  }
  // CORE //
  CLENCH_API core::App app;

}
