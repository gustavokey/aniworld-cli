#define BETTER_C_IMPLEMENTATION
#include "betterC.h"

int main(void)
{
  int rc = 0;

  rc = cbuild("-g", "main.c", "-o", "main", "-lssl", "-lcrypto");

  if (rc)
    return rc;

  return 0;
}
