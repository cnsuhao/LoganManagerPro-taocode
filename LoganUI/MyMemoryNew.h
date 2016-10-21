#pragma once

//#define  OVERWRITE_NEW_DELETE
#ifndef OVERWRITE_NEW_DELETE 
 #ifdef _DEBUG
 #include <crtdbg.h>
 #define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
 #else
 #define DEBUG_NEW new
 #endif
#else

#define DEBUG_NEW new(__FILE__, __LINE__)

void * __cdecl operator new[](size_t);
void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void *);

void * __cdecl operator new(size_t);
void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete(void *);
#endif