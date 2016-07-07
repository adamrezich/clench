// clench.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "clench.h"


// This is an example of an exported variable
CLENCH_API int nclench = 0;

// This is an example of an exported function.
CLENCH_API int fnclench(void) {
  return 42;
}

// This is the constructor of a class that has been exported.
// see clench.h for the class definition
Cclench::Cclench() {
  return;
}
