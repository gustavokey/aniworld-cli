#define CFLAGS "-g"
#define BETTER_C_IMPLEMENTATION
#include "betterC.h"

int main(void)
{
  return cbuild("aniworld.c", "-o", "aniworld", "-lssl", "-lcrypto");
}
