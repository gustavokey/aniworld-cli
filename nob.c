#define BETTER_C_IMPLEMENTATION
#include "betterC.h"

int main(void)
{
  int rc = 0;

  rc = cbuild("-g", "aniworld_cli.c", "-o", "aniworld_cli", "-lssl", "-lcrypto");

  if (rc)
    return rc;

  return 0;
}
