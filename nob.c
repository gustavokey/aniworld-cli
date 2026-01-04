#define CFLAGS "-g"
#define BETTER_C_IMPLEMENTATION
#include "betterC.h"

int main(void)
{
  int rc = 0;
  rc = cbuild("-g", "-std=c99", "-D_POSIX_C_SOURCE=200112L", "aniworld.c", "-o", "aniworld", "-lssl", "-lcrypto");
  if (rc) return rc;
  cbuild("-g", "-std=c99", "-D_POSIX_C_SOURCE=200112L", "aniworld-cli.c", "-o", "aniworld-cli", "-lssl", "-lcrypto");
  if (rc) return rc;
  return 0;
}
