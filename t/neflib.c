
#include <windows.h>

int __cdecl printf(const char * format, ...)
{
    char szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;

    va_start( argptr, format );
    retValue = wvsprintf( szBuff, format, argptr );
    va_end( argptr );

    WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuff, retValue,
                &cbWritten, 0 );

    return retValue;
}

int __cdecl sprintf(char * buf, const char * format, ...)
{
    int retValue;
    DWORD cbWritten;
    va_list argptr;

    va_start( argptr, format );
    retValue = wvsprintf( buf, format, argptr );
    va_end( argptr );

    return retValue;
}

int memcmp(const void* s1, const void* s2,size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    while(n--)
        if( *p1 != *p2 )
            return *p1 - *p2;
        else
            *p1++,*p2++;
    return 0;
}

void *memcpy(void *dest, const void *src, size_t count)
{
    char *dst8 = (char *)dest;
    char *src8 = (char *)src;

    while (count--) {
    	*dst8++ = *src8++;
    }

    return dest;
}

int atoi(const char * c) {
    int x = 0;
	while((*c>='0') & (*c<='9'))
		x = x * 10 + *c++ - '0';
	return *c ? 0 : x;
}

char* strchr(const char *x, int y)
{
  while ( (*x) && (*x != y) ) x++;
  return *x == y ? x : 0;
}

int strexs(char* s1, char* s2) {
	while(*s1) {
		while(*s2 == *s1) {
			s1++; s2++;
			if(!*s2) {
				return 1;
			}
			if(!*s1) {
				return 0;
			}
		}
		s1++;
	}
	return 0;
}

int mystrlen(char* s) {
	return strchr(s, 0) - s;
}

void _stackprobe(int depth) {
	__asm("movl 8(%ebp), %eax")
	__asm("neg %eax")
	__asm("movl (%esp,%eax*4), %eax")
}
