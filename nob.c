#define BETTER_C_IMPLEMENTATION
#include "betterC.h"

int main(void)
{
  int rc = 0;
  rc = cbuild("-g", "aniworld.c", "-o", "aniworld", "-lssl", "-lcrypto");
  if (rc) return rc;
  rc = cbuild("-g", "aniworld-cli.c", "-o", "aniworld-cli", "-lssl", "-lcrypto");
  if (rc) return rc;

  BETTER_CLOG("main", "Compiled succesfully\n");

  BETTER_CLOG("main", "Usage:");
  BETTER_CLOG("main", "./aniworld-cli watch  show staffel episode");
  BETTER_CLOG("main", "./aniworld-cli search show\n");

  return 0;
}
