#ifndef _STRING_H_
#define _STRING_H_

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

extern char * strerror(int errno);

extern  char * strcpy(char * dest,const char *src);

extern  char * strncpy(char * dest,const char *src,int count);

extern  char * strcat(char * dest,const char * src);

extern  char * strncat(char * dest,const char * src,int count);

extern  int strcmp(const char * cs,const char * ct);

extern  int strncmp(const char * cs,const char * ct,int count);

extern  char * strchr(const char * s,char c);

extern  char * strrchr(const char * s,char c);

extern  int strspn(const char * cs, const char * ct);

extern  int strcspn(const char * cs, const char * ct);

extern  char * strpbrk(const char * cs,const char * ct);

extern  char * strstr(const char * cs,const char * ct);

extern  int strlen(const char * s);

extern char * ___strtok;

extern  char * strtok(char * s,const char * ct);

extern  void * memcpy(void * dest,const void * src, int n);

extern  void * memmove(void * dest,const void * src, int n);

extern  int memcmp(const void * cs,const void * ct,int count);

extern  void * memchr(const void * cs,char c,int count);

extern  void * memset(void * s,char c,int count);

#endif
