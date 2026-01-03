#ifndef FMT_H_INCLUDE
#define FMT_H_INCLUDE

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

extern inline char *vstring_format(const char *format, va_list args);
extern inline char *string_format(const char *format, ...);
extern inline char *string_jump_over(char *string, const char *go);
extern inline char *string_scratch(size_t size);
extern inline void string_reset(void);
extern inline void string_pop(void);
extern inline void string_free(void);

#ifdef FMT_IMPLEMENTATION
extern inline char *vstring_format(const char *format, va_list args)
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
  result = vstring_format(format, args);
  va_end(args);

  return result;
}

extern inline char *string_jump_over(char *string, const char *go)
{
  char *to = NULL;

  to = strstr(string, go);

  if (!to) return NULL;

  return to + strlen(go);
}

extern inline char *string_scratch(size_t size)
{
  char *string = NULL;

  string = (char*)malloc(size*sizeof(char));
  memset(string, 0, size + 1);
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

extern inline void string_free(void)
{
  for (size_t i = 0; i < ____internal_formated_strings_buffer.count; ++i)
    free(____internal_formated_strings_buffer.items[i]);

  ____internal_formated_strings_buffer.items    = NULL;
  ____internal_formated_strings_buffer.count    = 0;
  ____internal_formated_strings_buffer.capacity = 0;
}

#endif // FMT_IMPLEMENTATION
#endif // FMT_H_INCLUDE

