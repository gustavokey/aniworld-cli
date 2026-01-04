#ifndef SSAK_H_INCLUDE
#define SSAK_H_INCLUDE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>

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

struct __InternalDAStrings {
  char **items;
  size_t count;
  size_t capacity;
};

static struct __InternalDAStrings ____internal_formated_strings_buffer = {0};

extern inline char *string_vformat(const char *format, va_list args);
extern inline char *string_format(const char *format, ...);
extern inline char *string_file(const char *path);
extern inline char *string_slice(const char *format, const char *string, const char *delimiter);
extern inline char *string_jump_to(char *string, const char *go);
extern inline char *string_jump_over(char *string, const char *go);
extern inline char *string_scratch(size_t size);
extern inline char *string_reverse(unsigned char *data);
extern inline char *string_shift(char *data, int shift);
extern inline char *string_remove(char *data, const char *pattern[]);
extern inline char *string_rot13(char *decoded, size_t size);
extern inline void string_reset(void);
extern inline void string_pop(void);
extern inline void string_pop_pro(int count);
extern inline void string_free(void);

#ifdef SSAK_IMPLEMENTATION
extern inline char *string_vformat(const char *format, va_list args)
{
  va_list args_copy = {0};
  char *result = NULL;
  int n = 0;

  va_copy(args_copy, args);
  n = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (n < 0) return NULL;

  result = (char*)malloc(sizeof(char)*(n + 1));
  da_append(&____internal_formated_strings_buffer, result);
  vsnprintf(result, n + 1, format, args);

  return result;
}

extern inline char *string_format(const char *format, ...)
{
  va_list args = {0};
  char *result = NULL;

  va_start(args, format);
  result = string_vformat(format, args);
  va_end(args);

  return result;
}

extern inline char *string_file(const char *path)
{
  FILE *f;
  char *content = NULL;
  long long int size = 0;

  f = fopen(path, "r");

  if (!f) return NULL;

  fseek(f, 0L, SEEK_END);
  size = ftell(f);
  rewind(f);

  content = (char *)malloc(sizeof(char) * (size + 1));
  fread(content, sizeof(char), size, f);
  da_append(&____internal_formated_strings_buffer, content);

  fclose(f);

  return content;
}

extern inline char *string_slice(const char *format, const char *string, const char *delimiter)
{
  char *slice = NULL;
  char *tail = NULL;

  tail = strstr(string, delimiter);

  if (!tail) return NULL;

  slice = string_format(format, tail - string + strlen(delimiter), string);

  return slice;
}

extern inline char *string_jump_to(char *string, const char *go)
{
  char *to = NULL;

  return strstr(string, go);
}

extern inline char *string_jump_over(char *string, const char *go)
{
  char *to = NULL;

  to = strstr(string, go);

  if (!to) return NULL;

  return to + strlen(go);
}

extern inline char *string_reverse(unsigned char *data)
{
  size_t size   = 0;
  size_t length = 0;

  size = strlen((const char*)data) - 1;
  length = size;

  for (size_t i = 0; i < length/2 + 1; ++i)
  {
    unsigned char c = data[i];
    data[i] = data[size];
    data[size] = c;
    size--;
  }

  return (char*)data;
}

extern inline char *string_shift(char *data, int shift)
{
  for (size_t i = 0; data[i]; ++i)
    data[i] = (unsigned char)(data[i] + shift);

  return data;
}

extern inline char *string_remove(char *data, const char *pattern[])
{
  char *replaced = NULL;
  char *found = NULL;
  size_t size = 0;

  size = strlen(data);
  replaced = string_scratch(size + 1);

  for (int i = 0; pattern[i]; ++i)
  {
    memcpy(replaced, data, size + 1);
    size = strlen(data);

    do {
      found = strstr(replaced, pattern[i]);

      if (found) memset(found, '\b', strlen(pattern[i]));
    } while (found);

    for (size_t i = 0, j = 0; replaced[i]; ++i)
    {
      if (replaced[i] != '\b') data[j++] = replaced[i];
      if (!replaced[i + 1]) memset(data + j, 0, size - j);
    }
  }

  string_pop();

  return data;
}

// Source: https://hea-www.harvard.edu/~fine/Tech/rot13.html
extern inline char *string_rot13(char *decoded, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    if ((decoded[i] >= 'A' && decoded[i] <= 'Z') || (decoded[i] >= 'a' && decoded[i] <= 'z'))
      decoded[i] = ~(~decoded[i])-1/(~(~decoded[i]|32)/13*2-11)*13;

  return decoded;
}

extern inline char *string_scratch(size_t size)
{
  char *string = NULL;

  string = (char*)malloc(size*sizeof(char));
  memset(string, 0, size);
  da_append(&____internal_formated_strings_buffer, string);

  return string;
}

extern inline void string_reset(void)
{
  for (size_t i = 0; i < ____internal_formated_strings_buffer.count; ++i)
    free(____internal_formated_strings_buffer.items[i]);

  ____internal_formated_strings_buffer.count = 0;
}

extern inline void string_pop(void)
{
  if (____internal_formated_strings_buffer.count)
  {
    ____internal_formated_strings_buffer.count -= 1;
    free(____internal_formated_strings_buffer.items[____internal_formated_strings_buffer.count]);
    ____internal_formated_strings_buffer.items[____internal_formated_strings_buffer.count] = NULL;
  }
}

extern inline void string_pop_pro(int count)
{
  while (count-- && ____internal_formated_strings_buffer.count)
  {
    ____internal_formated_strings_buffer.count -= 1;
    free(____internal_formated_strings_buffer.items[____internal_formated_strings_buffer.count]);
    ____internal_formated_strings_buffer.items[____internal_formated_strings_buffer.count] = NULL;
  }
}

extern inline void string_free(void)
{
  for (size_t i = 0; i < ____internal_formated_strings_buffer.count; ++i)
    free(____internal_formated_strings_buffer.items[i]);

  ____internal_formated_strings_buffer.items    = NULL;
  ____internal_formated_strings_buffer.count    = 0;
  ____internal_formated_strings_buffer.capacity = 0;
}

#endif // SSAK_IMPLEMENTATION
#endif // SSAK_H_INCLUDE

