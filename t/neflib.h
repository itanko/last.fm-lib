/*
  Neflib - libc.c win32 wrapper

*/

#ifndef NEFLIB_C
#define NEFLIB_C

#define exit(code)         ExitProcess(code);
#define strcpy             lstrcpy
#define strcmp			   lstrcmp
#define strcmpi            lstrcmpi
#define strcat             lstrcat
#define stricmp            strcmpi


//---------------------------------------------------------------------------

#define malloc(x)       ((void*)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, x))
#define free(x)         GlobalFree(x)

int __cdecl printf(const char * format, ...);
int __cdecl sprintf(char * buf, const char * format, ...);
int memcmp(const void* s1, const void* s2,size_t n);
int atoi(const char* c);
char *strchr(const char *str, int ch);
int strexs(char* s1, char* s2);
int mystrlen(char* s);
void _stackprobe(int depth);

#endif
