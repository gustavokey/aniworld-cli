#define BETTER_C_IMPLEMENTATION
#include "betterC.h"

int main(void)
{
  int rc = 0;
  rc = cbuild("aniworld.c", "-o", "aniworld", "-lssl", "-lcrypto");
  if (rc) return rc;
  cbuild("aniworld-cli.c", "-o", "aniworld-cli", "-lssl", "-lcrypto");
  if (rc) return rc;
  return 0;
}
