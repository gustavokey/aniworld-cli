#ifndef UNITY_H_INCLUDED
#define UNITY_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

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

#define HTTP_IMPLEMENTATION
#define SSAK_IMPLEMENTATION

#include "base64.h"
#include "http.h"
#include "ssak.h"

#include "base64.c"
#include "cmd.c"

#endif // UNITY_H_INCLUDED
