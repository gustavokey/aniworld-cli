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

#define HTTP_IMPLEMENTATION
#define FMT_IMPLEMENTATION

#include "base64.h"
#include "http.h"
#include "fmt.h"

#include "base64.c"
#include "cmd.c"

#endif // UNITY_H_INCLUDED
