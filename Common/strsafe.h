#ifndef STRSAFE_H
#define STRSAFE_H

#define STRCAT_S(d, s) do { strcat_s((d), (sizeof(d) / sizeof(d[0])), (s));} while ( 0 )
#define STRCPY_S(d, s) strcpy_s((d), (sizeof(d) / sizeof(d[0])), (s))

size_t strncpy_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count);
size_t strncat_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count);
int strncasecmp(const char *s1, const char *s2, size_t n);
int strcasecmp(const char *s1, const char *s2);

static inline size_t strcpy_s(char *strDestination, size_t numberOfElements, const char *strSource)
{
	return strncpy_s(strDestination, numberOfElements, strSource, -1);
}

static inline size_t strcat_s(char *strDestination, size_t numberOfElements, const char *strSource)
{
	return strncat_s(strDestination, numberOfElements, strSource, -1);
}

#endif
