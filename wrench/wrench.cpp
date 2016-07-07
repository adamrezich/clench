// wrench.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../clench/clench.h"
#include <iostream>


int main() {

  using namespace clench;
  using namespace clench::core;

  app.BeginInit();

  Compdef::Define("Transform", {
    { "x", Val::Float(0) },
    { "y", Val::Float(0) }
  });

  app.EndInit();

  Elem *world = app.root.Append("world");
  
  world->Attach("Transform");


  app.Begin();

  return 0;
}
