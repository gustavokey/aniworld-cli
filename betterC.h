/////////////////////////////////////////////////////
//
// Description: cool shit.
// Source:      https://codeberg.org/m4ya/betterC
//
// @Note Default Arguments Source:
//   - https://www.youtube.com/watch?v=VdmeoMZjIgs
//   - https://x.com/vkrajacic/status/1749816169736073295

#ifndef _BETTER_C_H
#define _BETTER_C_H

// @Note the FULL package experience
#ifdef BETTER_C_ALL
#  define BETTER_C_IMPLEMENTATION
#  define BETTER_C_TYPES
#  define BETTER_C_ENABLE_COLORS
#endif // BETTER_C_ALL


/////////////////////////////////////////////////////
//
// @Workaround compiler specific stuff
//
// @Note gcc cuckery.
#if __GNUC__
#  ifndef _GNU_SOURCE
#    define _GNU_SOURCE
#  endif // _GNU_SOURCE
#  ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 199001L
#  endif // _POSIX_C_SOURCE
#endif // __GNUC__

#if !(__GNUC__ && __clang__)
// @Note tcc hack... actually implement this?
extern inline void __popcountdi2(void) { return; }
#endif // ! (__GNUC__ & __clang__)

/////////////////////////////////////////////////////
//
// :includes
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef BETTER_C_ASSERT
#  define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
#else
#  include <assert.h>
#endif // BETTER_C_ASSERT

#ifdef _WIN32
#  define  MKDIR(p) _mkdir(p)
#  define  CHDIR _chdir
#  define  WIN32_LEAN_AND_MEAN
#  define  _WINUSER_
#  define  _WINGDI_
#  define  _IMM_
#  define  _WINCON_
#  include <windows.h>
#  include <direct.h>
#else
#  define MKDIR(p) mkdir(p, 0777)
#  define CHDIR chdir
#  include <unistd.h>
#  include <alloca.h>
#  include <fcntl.h>
#  include <sys/wait.h>
#  include <sys/stat.h>
#  ifdef BETTER_C_NETWORK
#    include <netinet/in.h>
#    include <arpa/inet.h>
#    include <sys/socket.h>
#    include <netdb.h>
#  endif // BETTER_C_NETWORK
#endif // WIN32 | UNIX

/////////////////////////////////////////////////////
//
// :types
//
#ifndef _STDBOOL_H
#define _STDBOOL_H
#  define bool _Bool
#  define true 1
#  define false 0
#endif // _STDBOOL_H
#ifdef BETTER_C_TYPES
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef char s8;
typedef long double f128;
typedef double f64;
typedef float f32;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef uintptr_t uptr;
#endif // BETTER_C_TYPES

/////////////////////////////////////////////////////
//
// @Note Tweak.
//
#ifndef CC
#  define CC "gcc"
#endif // CC
#ifndef CC_PATH
#  define CC_PATH "/usr/bin/"CC
#endif // _WIN32
#ifndef CFLAGS
#  define CFLAGS "-Wall", "-Werror"
#endif // CFLAGS

#ifndef BETTER_C_MAX_FMT_BUFFERS
#  define BETTER_C_MAX_FMT_BUFFERS 16
#endif // BETTER_C_MAX_FMT_BUFFERS
#ifndef BETTER_C_MAX_FMT_BUFFER
#  define BETTER_C_MAX_FMT_BUFFER 128
#endif // BETTER_C_MAX_FMT_BUFFERS
#ifndef BETTER_C_PATH_BUFFER
#  define BETTER_C_PATH_BUFFER 1024
#endif

/////////////////////////////////////////////////////
//
// +macros
//
#define asm __asm__
#define interrupt(c) asm("int"#c)
#define __btrC_syscall4(rc, reg0, reg1, reg2, reg3) \
  asm __volatile__ ("syscall" : "=a"(rc) : "a"(reg0), "D"(reg1), "S"(reg2), "d"(reg3) : "rcx", "r11", "memory")
#define __btrC_syscall5(rc, reg0, reg1, reg2, reg3, reg4) \
	 asm __volatile__ ("syscall" : "=a"(rc) : "a"(reg0), "D"(reg1), "S"(reg2), "d"(reg3), "r"(reg4): "rcx", "r11", "memory")

// C compiler magic, taken from Quake III
#define STRUCT_OFFSET(type, x) (uintptr_t)&((type*)0)->x

#define KB(size) 1024 * size

#ifdef BETTER_C_ENABLE_COLORS
#  define ANSI_COLOR_RED     "\x1b[31m"
#  define ANSI_COLOR_GREEN   "\x1b[32m"
#  define ANSI_COLOR_YELLOW  "\x1b[33m"
#  define ANSI_COLOR_BLUE    "\x1b[34m"
#  define ANSI_COLOR_MAGENTA "\x1b[35m"
#  define ANSI_COLOR_CYAN    "\x1b[36m"
#  define ANSI_COLOR_RESET   "\x1b[0m"
#  define STRING_COLOR(s, c) ANSI_COLOR_##c s ANSI_COLOR_RESET
#else
#  define ANSI_COLOR_RED     ""
#  define ANSI_COLOR_GREEN   ""
#  define ANSI_COLOR_YELLOW  ""
#  define ANSI_COLOR_BLUE    ""
#  define ANSI_COLOR_MAGENTA ""
#  define ANSI_COLOR_CYAN    ""
#  define ANSI_COLOR_RESET   ""
#  define STRING_COLOR(s, c) s
#endif // BETTER_C_ENABLE_COLORS

#define BETTER_CTODO(s, ...)                                                               \
    do {                                                                                   \
      printf("%s::"s":%d "ANSI_COLOR_GREEN"TODO: "ANSI_COLOR_RESET" ", __FILE__, __LINE__);\
      printf(__VA_ARGS__);                                                                 \
      printf("\n");                                                                        \
    } while (0)

#define BETTER_CLOG(s, ...)                                                            \
    do {                                                                               \
      printf("%s::"s":%d "ANSI_COLOR_GREEN">>"ANSI_COLOR_RESET" ", __FILE__, __LINE__);\
      printf(__VA_ARGS__);                                                             \
      printf("\n");                                                                    \
    } while (0)

#define BETTER_CERROR(s, ...)                                                                                           \
    do {                                                                                                                \
      printf("====[ "ANSI_COLOR_RED"ERROR"ANSI_COLOR_RESET" ]==================================================================================..." "\n");\
      printf("%s::"s":%d "ANSI_COLOR_RED">>"ANSI_COLOR_RESET" ", __FILE__, __LINE__);                                                                     \
      printf(__VA_ARGS__);                                                                                              \
      printf("\n");                                                                                                     \
      printf("===============================================================================================..." "\n");\
    } while (0)

// @Note https://github.com/tsoding/nob.h
#define da_reserve(da, expected_capacity)                                         \
    do {                                                                          \
      if ((expected_capacity) > (da)->capacity)                                   \
      {                                                                           \
        if ((da)->capacity == 0)                                                  \
          (da)->capacity = 256;                                                   \
                                                                                  \
        while ((expected_capacity) > (da)->capacity)                              \
          (da)->capacity *= 2;                                                    \
                                                                                  \
        (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));\
        assert((da)->items != NULL && "You ran out of RAM!");                     \
      }                                                                           \
    } while (0)

#define da_append(da, item)               \
    do {                                  \
      da_reserve((da), (da)->count + 1);  \
      (da)->items[(da)->count++] = (item);\
    } while (0)

#define Str(s)          strnew(s)
#define Strs(...)       strsnew(__VA_ARGS__, NULL)
#define fmt(s, ...)     strfmt(s, __VA_ARGS__, NULL)
#define da_free(da)     free((da).items)
#define bytes_free(s)   free((void *)s.data)
#define string_free(s)  free((void *)(s).ptr)
#define strings_free(s) free((s).items)

// @structures
typedef struct btrC_Rawbytes {
  unsigned char *data;
  long long int size;
} btrC_Rawbytes;

typedef struct btrC_String {
  char *ptr;
  long long int length;
} btrC_String;

typedef struct btrC_Strings {
  btrC_String *items;
  unsigned long long int count;
  unsigned long long int capacity;
} btrC_Strings;

struct strreplace_options {
  char reverse;
};

struct strnoext_options {
  int level;
};

struct strtos32_options {
  char end;
  int base;
};

struct cobject_options {
  char *includes;
};

struct strsearch_options {
  bool recursive;
  int *count;
  char **ss;
};
/////////////////////////////////////////////////////
//
// -functions
//
// -math
// 
extern inline double   f64sign(double n);
extern inline float    f32sign(float n);
extern inline float    f32abs(float n);
extern inline long int s64sign(long int n);
extern inline int      s32sign(int n);
extern inline int      s32abs(int n);
extern inline void     s32swap(int *n, int *m);
extern inline bool     s32pwr2(int n);
/////////////////////////////////////////////////////
//
// -string
//
#define strnoext(s, ...)         _strnoext(s, (struct strnoext_options) { .level = 1, __VA_ARGS__ })
#define strreplace(s, c, r, ...) _strreplace(s, c, r, (struct strreplace_options) { .reverse = -1, __VA_ARGS__ })
#define strsearch(h, ...)        _strsearch(h, (struct strsearch_options){ __VA_ARGS__ })
#define strtos32(s, ...)         _strtos32(s, (struct strtos32_options) { __VA_ARGS__ })
extern inline char*    _strnoext(char *s, struct strnoext_options opts);
extern inline char*    _strreplace(char *s, char c, char r, struct strreplace_options opts);
extern inline void*    _strsearch(char *haystack, struct strsearch_options opts); // @Note allocates memory! use free() when done.
extern inline int      _strtos32(const char *s, struct strtos32_options opts);
extern inline char*    strfmt(char *text, ...);
extern inline void     strsout(char **s);
extern inline long int strslen(char *const s[]);
extern inline long int strscount(char *const s[]);
extern inline long int strcindex(char *s, char c);
extern inline char*    strconcat(char dst[], const char **srcs);
extern inline char**   strsplit(char *s, char c); // @Note allocates memory! use strsfree() when done.
extern inline void     strsfree(char **s);
extern inline char*    strstostr(char **s); // @Note allocates memory! use free().
extern inline char**   strscatstrs(char **s0, char **s1); // @Note allocates memory! use strsfree() when done.
extern inline void     str__find_all_occurrences(uintptr_t *data, int *count, char *haystack, char *needle);
/////////////////////////////////////////////////////
//
// -fs
//
extern inline void          mkdir_check(char *dir);
extern inline void          mkdirs(char **dirs);
extern inline bool          changedir(char *dir);
extern inline btrC_Rawbytes file_bytes(const char *path); // @Note allocates memory! use bytes_free().
extern inline btrC_String   file_string(const char *path); // @Note allocates memory! use string_free().
extern inline int           file_print(const char *path);
extern inline int           file_printhex(const char *path);
extern inline void          file_write(const char *path, char *data); // @Warning deprecated
extern inline long long int file_create(const char *path, long long int size, unsigned char *data);
extern inline long long int file_load_create(const char *dst, btrC_Rawbytes *bytes, char *src);
extern inline bool          file_copy(char *dst, char *src);
extern inline bool          file_copy_many(char *base, char **srcs);
extern inline long long int file_append(const char *path, long long int size, char *data);
extern inline long long int file_size(const char *path);
/////////////////////////////////////////////////////
//
// -system
//
#ifndef _WIN32
extern inline void catenv(char *env, char *new);
#endif // _WIN32
/////////////////////////////////////////////////////
//
// -nob.h
//
#define mkd(...)              mkdirs((char *[]){ __VA_ARGS__, NULL })
#define cmd(...)              cmd_run((char *[]){ __VA_ARGS__, NULL })
#define cmd2(s, ...)          cmd_run2(s, (char *[]){ __VA_ARGS__, NULL })
#define cbuild(...)           _cbuild((char *[]){ __VA_ARGS__, NULL })
#define cobject(out, in, ...) _cobject(in, out, (struct cobject_options){ __VA_ARGS__ })
#define clibstatic(out, ...)  _clibstatic((char*[]){__VA_ARGS__}, out)
extern inline int cmd_run(char *cmd[]);
extern inline int cmd_run2(char **s, char *cmd[]);
extern inline int cmd_run_str(char *cmd);
#ifdef _WIN32
extern inline int cmd_run_daemon(char *cmd[]);
#endif // _WIN32
extern inline int _cbuild(char *cmd[]);
extern inline int _cobject(char *in, char *out, struct cobject_options opts);
extern inline int clibdynamic(char *in, char *out);
extern inline int _clibstatic(char *in[], char *out);
/////////////////////////////////////////////////////
//
// -misc
//
extern inline void vnfree(unsigned int n, ...);
#ifdef BETTER_C_ASSERT
extern inline void __assert_fail(const char *expr, const char *file, int line, const char *func);
#endif
/////////////////////////////////////////////////////
//
// -network
//
#ifdef BETTER_C_NETWORK
extern inline int           net_tcp_socket(const char *ip, int port);
extern inline int           net_tcp_connect(char *ip, int port); // @Note returns file descriptor.
extern inline void          net_tcp_listen(int sockfd);
extern inline int           net_tcp_accept(int sockfd);
extern inline long long int net_tcp_send(int sockfd, const unsigned char msg[], long int size);
extern inline long long int net_tcp_recv(int sockfd, unsigned char msg[], long int size);
static inline bool          net_split_host_path(char *url, char host[], char urlpath[]);
extern inline int           net_tcp_resolve_hostname(char ip[], char *hostname);
extern inline int           net_close(int sockfd);
#endif // BETTER_C_NETWORK

#ifdef BETTER_C_IMPLEMENTATION
/////////////////////////////////////////////////////
//
// +functions
//
// +math
//
extern inline double f64sign(double n)
{
  long int ln = 0;

  ln = *(long int *)&n;

  return (double) ((!!ln) | (ln >> 63)); 
}

extern inline float f32sign(float n)
{
  int in = 0;

  in = *(int *)&n;

  return (float) ((!!in) | (in >> 31));
}

extern inline float f32abs(float n)
{
  union { float f; unsigned int i; } u = {0};

  u.f = n;
  u.i &= 0x7fffffff;

  return *(float *)&u.f;
}

extern inline long int s64sign(long int n)
{
  return ((!!n) | (n >> 63));
}

extern inline int s32sign(int n)
{
  return ((!!n) | (n >> 31));
}

extern inline int s32abs(int n)
{
  int const mask = (n >> sizeof(int)) * 8 - 1;

  return (n + mask) ^ mask;
}

extern inline void s32swap(int *n, int *m)
{
  *n ^= (*m);
  *m ^= (*n);
  *n ^= (*m);

  return;
}

extern inline bool s32pwr2(int n)
{
  return (n & (n - 1)) == 0;
}
/////////////////////////////////////////////////////
//
// +string
//
extern inline btrC_String strnew(char *ptr)
{
  long long int length = strlen(ptr);
  btrC_String s = (btrC_String){ .ptr = ptr, .length = length};
  return s;
}

extern inline btrC_Strings strsnew(char *first, ...)
{
  btrC_Strings s = {0};
  btrC_String temp = {0};

  char *arg;

  va_list args;
  va_start(args, first);

  temp.ptr = first;
  temp.length = strlen(first);
  da_append(&s, temp);

  while ( (arg = va_arg(args, char*)) != NULL )
  {
    temp.ptr = arg;
    temp.length = strlen(arg);
    da_append(&s, temp);
  }

  va_end(args);

  return s;
}

// @Note from raylib.
char *strfmt(char *text, ...)
{
  static char buffers[BETTER_C_MAX_FMT_BUFFERS][BETTER_C_MAX_FMT_BUFFER] = { 0 };
  static int index = 0;
  va_list args     = {0};
  char *current    = NULL;
  char *truncate   = NULL;
  int bytes        = 0;

  current = buffers[index];
  memset(current, 0, BETTER_C_MAX_FMT_BUFFER);

  va_start(args, text);
  bytes = vsnprintf(current, BETTER_C_MAX_FMT_BUFFER, text, args);
  va_end(args);

  if (bytes >= BETTER_C_MAX_FMT_BUFFER)
  {
#ifdef BETTER_C_DEBUG
    BETTER_CERROR("betterC", "static buffer overwrite!");
#endif
    truncate = buffers[index] + BETTER_C_MAX_FMT_BUFFER - 4;
    sprintf(truncate, "...");
  }

  index += 1;
  if (index >= BETTER_C_MAX_FMT_BUFFERS)
  {
#ifdef BETTER_C_DEBUG
    BETTER_CERROR("betterC", "static buffers overwrite!");
#endif
    index = 0;
  }

  return current;
}

extern inline void strsout(char **s)
{
  char **s_begin = NULL;

  s_begin = s;
  while (*s != NULL)
  {
    printf("'%s'\n", *s);
    ++s;
  }
  s = s_begin;

  return;
}

extern inline long int strslen(char *const s[])
{
  long int i = 0;
  long int length = 0;
  do {
    length += strlen(s[i]);
    ++i;
  } while (*(s + i) != NULL);
  return length;
}

extern inline char* _strnoext(char *s, struct strnoext_options opts)
{
  char *ext = NULL;
  char *tmp = NULL;

  if (s == NULL)
  {
    BETTER_CERROR("betterC", "(s8 *)null: invalid string");
    return NULL;
  }

  while (opts.level)
  {
    tmp = ext;

    ext = strrchr(s, '.');

    if (!ext)
    {
      BETTER_CERROR("betterC", "(s8 *)null: '.' not found");
      return NULL;
    }

    if (tmp != NULL)
      *tmp = '.';

    *ext = '\0';

    --opts.level;
  }

  *ext = '\0';

  return ext;
}

extern inline long int strscount(char *const s[])
{
  char * const*ptr = NULL;
  long int count = 0;

  if (s == NULL)
    return 0;

  ptr = s;

  while (*ptr != NULL)
  {
    ++count;
    ++ptr;
  }

  return count;
}

extern inline long int strcindex(char *s, char c)
{
  char *s_begin = NULL;

  s_begin = s;

  for (long int i = 0; *s != '\0'; ++i)
  {
    if (*s == c) 
    {
      s = s_begin;
      return i;
    }
    ++s;
  }

  s = s_begin;

  return -1;
}

extern inline char* _strreplace(char *s, char c, char r, struct strreplace_options opts)
{
  char *end = NULL;

  if (s == NULL)
  {
    BETTER_CERROR("betterC", "(s8 *)null: invalid string");
    return NULL;
  }

  if (opts.reverse < 0)
  {
    end = strchr(s, c);
  }
  else if (opts.reverse == 1)
  {
    end = strrchr(s, c);
  }
  else if (opts.reverse >= 0)
  {
    end = s;

    end = strchr(s, opts.reverse);

    while (*end != c && end != s)
      --end;
  }

  if (end == NULL)
  {
    BETTER_CERROR("betterC", "(s8 *)null: '%c' not found", c);
    return NULL;
  }

  *end = r;

  return end;
}

extern inline char* strconcat(char dst[], const char **srcs)
{
  char *end = NULL;
  const char **psrcs = NULL;

  if (!(*srcs))
    return NULL;

  psrcs = srcs;

  while (*psrcs != NULL)
  {
    end = strchr(dst, '\0');
    memcpy(dst + (end - dst), *psrcs, sizeof(char) * strlen(*psrcs));
    ++psrcs;
  }

  return dst;
}

extern inline char **strsplit(char *s, char c)
{
  char *s_begin                 = NULL;
  char *s_nospace               = NULL;
  char **splits                 = NULL;
  unsigned long long int count  = 0;
  unsigned long long int length = 0;
  unsigned long long int size   = 0;

  length = strlen(s);

  s_nospace = alloca(sizeof(char*) * (length + 1));

  s_begin = s;
  while (*s != '\0')
  {
    *s_nospace = *s;

    if (*s == c)
    {
      *s_nospace = '\0';
      ++count;
    }

    ++s;
    ++s_nospace;
  }
  *s_nospace = '\0';
  s          = s_begin;
  s_nospace  = s_nospace - length;

  count += 1;

  splits = malloc(sizeof(char *) * (count + 1));

  size = strlen(s_nospace);

  *splits = malloc(sizeof(char *) * size);

  memcpy(*splits, s_nospace, size);

  while (s != s_begin + length - 1)
  {
    if (*s_nospace == '\0')
    {
      ++splits;
      size = strlen(s_nospace + 1);
      *splits = malloc(sizeof(char) * size);
      memcpy(*splits, s_nospace + 1, size);
    }

    ++s_nospace;
    ++s;
  }
  *splits = NULL; // end
  s       = s_begin;
  splits  = splits - count + 1;

  return splits;
}

extern inline void strsfree(char **s)
{
  char **s_begin = NULL;

  s_begin = s;

  while(*s != NULL)
  {
    free(*s);
    *s = NULL;
    ++s;
  }

  free(s_begin);

  return;
}

extern inline char *strstostr(char **s)
{
  long int length = 0;
  char **s_begin = NULL;
  char *new = NULL;

  s_begin = s;

  new = malloc(sizeof(char) * (strslen(s) + 1));

  while (*s_begin != NULL)
  {
    length = strlen(*s_begin);
    memcpy(new + strlen(new), *s_begin, sizeof(char) * (length + 1));
    ++s_begin;
  }

  return new;
}

extern inline char **strscatstrs(char **s0, char **s1)
{
  char **cs0      = NULL;
  char **cs1      = NULL;
  char **new      = NULL;
  char **begin    = NULL;
  long int length = 0;

  if (s0 == NULL && s1 == NULL)
    return NULL;

  new = (char **)malloc(sizeof(char *) * (strscount(s0) + strscount(s1) + 1));

  begin = new;
  cs0 = s0;
  cs1 = s1;

  while (cs0 != NULL && *cs0 != NULL)
  {
    length = strlen(*cs0);
    *new = (char *)malloc(sizeof(char) * (length + 1));
    memcpy(*new, *cs0, sizeof(char) * (length + 1));
    ++new;
    ++cs0;
  }

  while (cs1 != NULL && *cs1 != NULL)
  {
    length = strlen(*cs1);
    *new = (char *)malloc(sizeof(char) * (length + 1));
    memcpy(*new, *cs1, sizeof(char) * (length + 1));
    ++new;
    ++cs1;
  }

  *new = NULL;

  return begin;
}

extern inline void *_strsearch(char *haystack, struct strsearch_options opts)
{
  uintptr_t *ptrs = NULL;
  int size = 0;

  if (opts.ss && !opts.recursive)
  {
    for (int i = 0; *(opts.ss + i) != NULL; ++i)
      ++size;

    ptrs = (uintptr_t*)malloc(sizeof(uintptr_t) * (size + 1));

    for (int i = 0; i < size; ++i)
    {
      *(ptrs + i) = (uintptr_t)strstr(haystack, *(opts.ss + i));
      *(ptrs + i + 1) = 0;
    }

    if (opts.count) *opts.count = size;

    return (void*)ptrs;
  }
  else if (opts.ss && opts.recursive)
  {
    for (int i = 0; *(opts.ss + i) != NULL; ++i)
      str__find_all_occurrences(NULL, &size, haystack, *(opts.ss + i));

    ptrs = (uintptr_t*)malloc(sizeof(uintptr_t) * (size + 1));
    size = 0;

    for (int i = 0; *(opts.ss + i) != NULL; ++i)
      str__find_all_occurrences(ptrs, &size, haystack, *(opts.ss + i));

    if (opts.count) *opts.count = size;

    return (void*)ptrs;
  }

  return NULL;
}

extern inline void str__find_all_occurrences(uintptr_t *data, int *count, char *haystack, char *needle)
{
  char *ptr = NULL;

  ptr = haystack;

  while ( *ptr && (ptr = strstr(ptr, needle)) )
  {
    if (data) *(data + *count) = (uintptr_t)ptr;
    ++ptr;
    ++(*count);
  }
}

extern inline int _strtos32(const char *s, struct strtos32_options opts)
{
  int   number = 0;
  char *buffer = NULL;
  char *s_end  = NULL;

  opts.end = !opts.end ? '\0' : opts.end;
  opts.base = !opts.base ? 10 : opts.base;

  s_end = strchr(s, opts.end);

  buffer = alloca( sizeof(char) * ((s_end - s) + 1) );

  memset(buffer, 0, sizeof(char) * ((s_end - s) + 1));

  memcpy(buffer, s, sizeof(char) * (s_end - s));

  number = strtol(buffer, NULL, opts.base);

  return number;
}

/////////////////////////////////////////////////////
//
// +fs
//
extern inline void mkdir_check(char *dir)
{
  int rc = 0;

  rc = MKDIR(dir);

  if (rc == 0)
    BETTER_CLOG("betterC", "Creating directory '%s'.", dir);

  return;
}

extern inline void mkdirs(char **dirs)
{
  for (int paths = 0; dirs[paths] != NULL; ++paths)
  {
    int len = strlen( dirs[paths] );
    char buffer[len + 1];

    memset(buffer, 0, len + 1);

    for (int i = 0; dirs[paths][i] != '\0'; ++i)
    {
      if (dirs[paths][i - 1] == '/')
      {
        memcpy(buffer, dirs[paths], i);
        if (strcmp(buffer, "./") != 0) mkdir_check(buffer);
      }
    }

    memcpy(buffer, dirs[paths], len);
    mkdir_check(buffer);
  }
  return;
}

extern inline bool changedir(char *dir)
{
  int rc = false;

  rc = CHDIR(dir);

  if (rc != 0)
    BETTER_CERROR("betterC", "Failed to change working directory: '%s'", dir);
  else 
    BETTER_CLOG("betterC", "Working directory: '%s'", dir);

  return (rc == 0);
}

extern inline btrC_Rawbytes file_bytes(const char *path)
{
  FILE *f;
  btrC_Rawbytes file = {0};
  long long int size = 0;

  f = fopen(path, "rb");

  if (!f)
  { BETTER_CLOG("betterC", "Failed to load: '%s'", path);
    return (btrC_Rawbytes) {0}; }

  size = file_size(path) + 1;

  file.data = (unsigned char*)malloc(sizeof(unsigned char) * (size + 1));
  file.size = fread(file.data, sizeof(unsigned char), (size + 1), f);

  fclose(f);

  return file;
}

extern inline btrC_String file_string(const char *path)
{
  FILE *f;
  btrC_String s = {0};
  long long int i = 0;
  long long int size = 0;
  char c;

  f = fopen(path, "r");
  if (f == NULL) return (btrC_String){0};

  size = file_size(path);
  s.length = size;
  s.ptr = (char *)malloc(sizeof(char *) * size + 4);

  while( (c = fgetc(f)) != EOF )
  {
    s.ptr[i] = c;
    ++i;
  }

  fclose(f);
  return s;
}

extern inline int file_print(const char *path)
{
  FILE *f = NULL;
  char c;

  f = fopen(path, "r");

  if (f == NULL)
  { BETTER_CERROR("betterC", "(s8 *)null: empty path");
    return 1; }

  while ((c = fgetc(f)) != EOF)
    printf("%c", c);

  fclose(f);
  return 0;
}

extern inline int file_printhex(const char *path)
{
  btrC_Rawbytes bytes = {0};
  long long int i = 0;

  if (path == NULL)
  { BETTER_CERROR("betterC", "(s8 *)null: empty path");
    return 1; }

  bytes = file_bytes(path);

  if (bytes.data == NULL)
  { BETTER_CERROR("betterC", "(u8 *)null: empty data");
    return 1; }

  while (i != bytes.size)
  {
    if (*(bytes.data + i) <= 0xF)
      printf(" \\0x0%X", *(bytes.data + i));
    else
      printf(" \\0x%X", *(bytes.data + i));

    ++i;
  }
  printf("\n");

  bytes_free(bytes);

  return 0;
}

extern inline void file_write(const char *path, char *data)
{
  FILE *f = NULL;

  f = fopen(path, "w");

  fprintf(f, "%s", data);

  fclose(f);
}

extern inline long long int file_create(const char *path, long long int size, unsigned char *data)
{
  FILE *f = NULL;
  long long int bytes = 0;

  f = fopen(path, "wb");

  bytes = fwrite(data, sizeof(unsigned char), size, f);

  if (!bytes)
  { BETTER_CERROR("betterC", "No bytes were written to file");
    return -1; }

  fclose(f);

  return bytes;
}

extern inline long long int file_load_create(const char *dst, btrC_Rawbytes *bytes, char *src)
{
  if (!src)
  { BETTER_CERROR("betterC", "(s8 *)null: Source is empty");
    return -1; }

  if (!dst)
  { BETTER_CERROR("betterC", "(s8 *)null: Destination is empty");
    return -1; }

  if (bytes->data)
  {
    memset(bytes->data, 0, bytes->size);
    free(bytes->data);
  }

  *bytes = file_bytes(src);

  if (!bytes->data)
    return -1;

  return ( file_create(dst, bytes->size, bytes->data) );
}

extern inline bool file_copy(char *dst, char *src)
{
  btrC_Rawbytes file = {0};

  file_load_create(dst, &file, src);

  if (!file.data)
    return false;

  bytes_free(file);

  return true;
}

extern inline bool file_copy_many(char *base, char **srcs)
{
  char **files = NULL;
  char full[BETTER_C_PATH_BUFFER] = {0};
  char *finaldir = NULL;
  long long int base_length = 0;
  long long int files_length = 0;
  btrC_Rawbytes content = {0};

  base_length = strlen(base);
  files = srcs;

  memcpy(full, base, sizeof(char) * (base_length + 1));
  *(full + base_length) = '/';

  while (*files != NULL)
  {
    files_length = strlen(*files);

    if (base_length + files_length + 4 >= 1024)
    { BETTER_CERROR("betterC", "Stack Overflow: base_length + files_length + 4 >= 1024");
      return false; }

    memcpy(full + base_length + 1, *files, sizeof(char) * (files_length + 1));

    finaldir = strrchr(full, '/');
    *finaldir = '\0';
    mkdirs((char *[]){ full, NULL });
    *finaldir = '/';

    content = file_bytes(*files);
    file_create(full, content.size, content.data);

    memset(content.data, 0, sizeof(char) * content.size);
    bytes_free(content);

    BETTER_CLOG("betterC", "'%s' -> '%s'", *files, full);

    ++files;
  }

  return true;
}

extern inline long long int file_append(const char *path, long long int size, char *data)
{
  FILE *f = NULL;
  long long int bytes = 0;

  f = fopen(path, "ab");

  bytes = fwrite(data, sizeof(unsigned char), size, f);

  fclose(f);

  return bytes;
}

extern inline long long int file_size(const char *path)
{
  FILE *f = fopen(path, "rb");
  long long int size = 0;
  fseek(f, 0, SEEK_END);

  if ( ( size = ftell(f) ) < 1)
    return -1;

  fclose(f);

  return size;
}
/////////////////////////////////////////////////////
//
// +system
//
#ifndef _WIN32
extern inline void catenv(char *env, char *new)
{
  char *old = NULL;
  char *result = NULL;
  long long int oldlen = 0;
  long long int newlen = 0;

  old = getenv(env);

  if (old != NULL)
  {
    oldlen = strlen(old);
    newlen = strlen(new);
    result = alloca( sizeof(char) * (oldlen + newlen + 2) );
    memcpy(result, old, oldlen + 1);
    *(result + oldlen) = ' ';
    memcpy(result + oldlen + 1, new, newlen + 1);
  }
  else
  {
    newlen = strlen(new);
    result = alloca( sizeof(char) * (newlen + 1) );
    memcpy(result, new, newlen + 1);
  }

  BETTER_CLOG("betterC", "Working environment: %s='%s'", env, result);

  setenv(env, result, 1);
}
#endif // _WIN32
/////////////////////////////////////////////////////
//
// +nob.h
//
extern inline int cmd_run(char *cmd[])
{
#ifdef _WIN32
  printf(__FILE__"::betterC:%d "ANSI_COLOR_GREEN">>"ANSI_COLOR_RESET" Exec ", __LINE__);

  for (int i = 0; cmd[i] != NULL; ++i)
    printf("%s ", cmd[i]);

  printf("\n");

  STARTUPINFO siStartInfo;
  ZeroMemory(&siStartInfo, sizeof(siStartInfo));
  siStartInfo.cb = sizeof(STARTUPINFO);

  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION piProcInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  // @Note string array to string.
  long int cmdsize = strslen(cmd) + 1;
  char cmdstr[cmdsize];
  memset(cmdstr, 0, cmdsize);
  for (long int i = 0; cmd[i] != NULL; ++i)
  {
    strcat(cmdstr, cmd[i]);
    strcat(cmdstr, " ");
  }

  // @Note spawn cmd process.
  BOOL bSuccess = CreateProcessA(NULL, cmdstr, NULL, NULL, FALSE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

  if (!bSuccess)
  { BETTER_CERROR("betterC", "Failed to start process!");
    exit(1); }

  CloseHandle(piProcInfo.hThread);

  return 0;
#else
  int rc = 0;
  pid_t pid = 0;

  printf(__FILE__"::betterC:%d "  STRING_COLOR(">>", GREEN)  " Exec ", __LINE__);
  for (int i = 0; cmd[i] != NULL; ++i)
    printf("%s ", cmd[i]);

  printf("\n");

  pid = fork();

  if (pid == 0)
  {
    if (execv(*cmd, cmd + 1) < 0)
    { BETTER_CERROR("betterC", "Failed to start process!");
      exit(1); }
  }
  else
  {
    waitpid(pid, &rc, 0);

    if (WIFEXITED(rc))
      return WEXITSTATUS(rc);
    else
      return 1;
  }

  return 1;
#endif // WIN32 | UNIX
}

extern inline int cmd_run2(char **s, char *cmd[]) // @Improvement better name
{
  char **arguments = NULL;
  int rc = 0;

  if (s == NULL && cmd == NULL)
    return 1;

  arguments = strscatstrs(cmd, s);

  rc = cmd_run(arguments);

  strsfree(arguments);

  return rc;
}

extern inline int cmd_run_str(char *cmd)
{
  bool skip                     = false;
  char *cmd_begin               = NULL;
  char *cmd_nospace             = NULL;
  char **arguments              = NULL;
  unsigned long long int count  = 0;
  unsigned long long int length = 0;

  length = strlen(cmd);

  cmd_nospace = alloca(sizeof(char) * (length + 1));

  cmd_begin = cmd;
  while (*cmd != '\0')
  {
    *cmd_nospace = *cmd;

    if (*cmd == '\'')
      skip = !skip;

    if (*cmd == ' ' && !skip)
    {
      *cmd_nospace = '\0';
      ++count;
    }

    ++cmd;
    ++cmd_nospace;
  }
  *cmd_nospace = '\0';
  cmd          = cmd_begin;
  cmd_nospace  = cmd_nospace - length;

  count += 1;

  arguments = alloca(sizeof(char *) * (count + 1));

  *arguments = cmd_nospace;

  while (cmd != cmd_begin + length)
  {
    if (*cmd_nospace == '\0')
    {
      ++arguments;
      *arguments = alloca(sizeof(char *));
      *arguments = cmd_nospace + 1;
      if (**arguments == '\'')
      {
        **arguments =  ' ';
        _strreplace(*arguments, '\'', ' ', (struct strreplace_options) { .reverse = 1 });
      }
    }

    ++cmd_nospace;
    ++cmd;
  }
  *(arguments + 1) = NULL; // end
  cmd              = cmd_begin;
  arguments        = arguments - count + 1;

  cmd_run(arguments);
  return 0;
}

#ifndef _WIN32
extern inline int cmd_run_daemon(char *cmd[])
{
  int rc = 0;
  pid_t pid = 0;

  printf(__FILE__"::betterC:%d " STRING_COLOR(">>", GREEN) " Daemon ", __LINE__);
  for (int i = 0; cmd[i] != NULL; ++i)
    printf("%s ", cmd[i]);

  printf("\n");

  changedir("/");

  rc = daemon(0, 0);

  if (rc)
  { BETTER_CERROR("betterC", "Failed: daemon did not start");
    exit(1); }

  pid = fork();

  if (pid == 0)
  {
    if (execv(*cmd, cmd + 1) < 0)
    { BETTER_CERROR("betterC", "Failed: process did not start");
      exit(1); }
  }

  return 0;
}
#endif // _WIN32

extern inline int _cbuild(char *cmd[])
{
  int rc = 0;

  rc = cmd_run2(cmd, (char *[]){CC_PATH, CC, CFLAGS, NULL});

  return rc;
}

extern inline int _cobject(char *in, char *out, struct cobject_options opts)
{
  int rc = 0;

  if (opts.includes)
  {
    rc = cmd_run((char *[]){CC_PATH, CC, CFLAGS, opts.includes, "-fPIC", "-DPIC", "-O", "-c", in, "-o", out, NULL});
    return rc;
  }

  rc = cmd_run((char *[]){CC_PATH, CC, CFLAGS, "-fPIC", "-DPIC", "-O", "-c", in, "-o", out, NULL});

  return rc;
}

extern inline int clibdynamic(char *in, char *out)
{
  int rc = 0;

  rc = cmd_run((char *[]){CC_PATH, CC, "-shared", "-o", out, in, NULL});
  return rc;
}

extern inline int _clibstatic(char *in[], char *out)
{
  int rc = 0;

  rc = cmd_run2(in, (char *[]){"/usr/bin/ar", "ar", "rcs", out, NULL});
  return rc;
}
/////////////////////////////////////////////////////
//
// +misc
//
extern inline void vnfree(unsigned int n, ...)
{
  va_list args;
  va_start(args, n);
  for (unsigned int i = 0; i < n; i++)
      free(va_arg(args, void*));
  va_end(args);
}

#ifdef BETTER_C_ASSERT
extern inline void __assert_fail(const char *expr, const char *file, int line, const char *func)
{
  fprintf(stderr, "Assertion failed: %s (%s: %s: %d)\n", expr, file, func, line);
  abort();
}
#endif //BETTER_C_ASSERT
/////////////////////////////////////////////////////
//
// +network
//
#ifdef BETTER_C_NETWORK
extern inline int net_tcp_socket(const char *ip, int port)
{
  int rc     = 0;
  int sockfd = 0;
  struct sockaddr_in address = { 0 };

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family      = AF_INET;
  address.sin_addr.s_addr = inet_addr(ip);
  address.sin_port        = htons(port);

  BETTER_CLOG("betterC", "Binding: tcp://%s:%d", ip, port);

  rc = bind(sockfd, (struct sockaddr *)&address, sizeof(address));

  if ( rc )
  {
    BETTER_CERROR("main", "Failed: couldn't bind address to socket");

    return -1;
  }

  return sockfd;
}

extern inline int net_tcp_connect(char *ip, int port)
{
  int sockfd = 0;
  int rc = 0;
  struct sockaddr_in server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_port = htons(port);

  rc = connect(sockfd, (struct sockaddr *)&server, sizeof(server));

  if (rc)
    return -1;

  return sockfd;
}

extern inline void net_tcp_listen(int sockfd)
{
  BETTER_CLOG("betterC", "Listening on SocketFD: %d", sockfd);

  listen(sockfd, 5);
}

extern inline int net_tcp_accept(int sockfd)
{
  int connfd = 0;
  struct sockaddr_in address = { 0 };
  socklen_t address_size = 0;
  char ip_address[INET_ADDRSTRLEN] = { 0 };

  connfd = accept(sockfd, (struct sockaddr *)&address, &address_size);

  inet_ntop(AF_INET, (struct sockaddr *)&address.sin_addr, ip_address, INET_ADDRSTRLEN);

  BETTER_CLOG("betterC", "Accepted: tcp://%s:%d", ip_address, address.sin_port);

  return connfd;
}

extern inline long long int net_tcp_send(int sockfd, const unsigned char msg[], long int size)
{
  return send(sockfd, msg, size, 0);
}

extern inline long long int net_tcp_recv(int sockfd, unsigned char msg[], long int size)
{
  return recv(sockfd, msg, size, 0);
}

static inline bool net_split_host_path(char *url, char host[], char urlpath[])
{
  char *host_begin    = NULL;
  char *urlpath_begin = NULL;

  if (!url)
    return false;

  host_begin = url + 8;
  strncpy( host, host_begin, strcindex(host_begin, '/') );

  urlpath_begin = strchr(host_begin, '/');

  if (!urlpath_begin)
    return false;

  memcpy( urlpath, urlpath_begin, sizeof(char) * strlen(urlpath_begin) );

  return true;
}

extern inline int net_tcp_resolve_hostname(char ip[], char *hostname)
{
  struct addrinfo hints     = {0};
  struct addrinfo *servinfo = NULL;
  struct addrinfo *p        = NULL;
  struct sockaddr_in *h     = NULL;
  int    rc                 = 0;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  rc = getaddrinfo(hostname , "http", &hints, &servinfo);

  if (rc) 
    return 1;

  for (p = servinfo; p != NULL; p = p->ai_next) 
  {
    h = (struct sockaddr_in *) p->ai_addr;
    strcpy(ip, inet_ntoa(h->sin_addr));
  }

  freeaddrinfo(servinfo);

  return 0;
}

extern inline int net_close(int sockfd)
{
  return close(sockfd);
}
#endif // BETTER_C_NETWORK

#endif   // BETTER_C_IMPLEMENTATION
#endif  // _BETTER_C_H

// - Hello person reading this :)
