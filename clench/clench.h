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
