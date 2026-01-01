extern inline char *format_string(char *text, ...)
{
  static char buffers[8][256] = { 0 };
  static int index = 0;
  va_list args     = {0};
  char *current    = NULL;
  char *truncate   = NULL;
  int bytes        = 0;

  current = buffers[index];
  memset(current, 0, 128);

  va_start(args, text);
  bytes = vsnprintf(current, 128, text, args);
  va_end(args);

  if (bytes >= 128)
  {
    printf("[WARNING] Format buffer overwrite\n");
    truncate = buffers[index] + 124;
    sprintf(truncate, "...");
  }

  index += 1;
  if (index >= 16)
  {
    printf("[WARNING] Format buffers overwrite\n");
    index = 0;
  }

  return current;
}
