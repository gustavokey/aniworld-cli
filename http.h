////////////////////////////////////////
//
// Description: Imediate http library with tls support
// Source: https://codeberg.org/m4ya/http.h
//
//

#ifndef _HTTP_H_INCLUDE
#define _HTTP_H_INCLUDE

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

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

#define __MESSAGE_SIZE 1024

typedef struct {
  const char **items;
  size_t count;
  size_t capacity;
} HttpMessage;

typedef struct {
  int           sockfd;
  in_addr_t     ip;
  HttpMessage   req;
  SSL_CTX*      ctx;
  SSL*          ssl;
} HttpConnect;

typedef struct {
  unsigned char *items;
  size_t count;
  size_t capacity;
} HttpResponse;

struct http_connect_options {
  bool secure;
};

struct http_recv_options {
  HttpResponse *out;
  const char *file;
};

#define http_connect(con, url, ...) _http_connect((con), (url), ((struct http_connect_options){__VA_ARGS__}))
extern inline void _http_connect(HttpConnect *con, const char *url, struct http_connect_options opts);
extern inline void http_resolve(HttpConnect *con, const char *url);
extern inline bool http_ssl(int sockfd, SSL **ssl, SSL_CTX **ctx, const char *url);
extern inline void http_request(HttpConnect *con, const char *req);
extern inline void http_response_reset(HttpResponse *res);
extern inline void http_send(HttpConnect con);
#define http_recv(con, ...) _http_recv((con), ((struct http_recv_options){__VA_ARGS__}))
extern inline void _http_recv(HttpConnect con, struct http_recv_options opts);
extern inline void http_disconnect(HttpConnect *con);
extern inline void http_close(HttpConnect con);

static unsigned char __http__message[__MESSAGE_SIZE] = {0};

#ifdef HTTP_IMPLEMENTATION
extern inline void http_resolve(HttpConnect *con, const char *url)
{
  struct addrinfo hints     = {0};
  struct addrinfo *servinfo = NULL;
  struct addrinfo *p        = NULL;
  struct sockaddr_in *h     = NULL;
  int    rc                 = 0;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  rc = getaddrinfo(url , "https", &hints, &servinfo);

  if (rc) 
  {
    printf("[ERROR] Failed to get address information\n");
    return;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) 
  {
    h = (struct sockaddr_in *) p->ai_addr;
    con->ip = h->sin_addr.s_addr;
  }

  freeaddrinfo(servinfo);
}

extern inline void _http_connect(HttpConnect *con, const char *url, struct http_connect_options opts)
{
  struct sockaddr_in server = {0};
  int sockfd = 0;
  int rc = 0;

  http_resolve(con, url);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = con->ip;
  server.sin_port = opts.secure ? htons(443) : htons(80);

  rc = connect(sockfd, (struct sockaddr *)&server, sizeof(server));

  if (rc)
  {
    printf("[ERROR] Failed to connect\n");
    return;
  }

  con->sockfd = sockfd;

  if (opts.secure)
  {
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    http_ssl(con->sockfd, &con->ssl, &con->ctx, url);
  }
}

extern inline bool http_ssl(int sockfd, SSL **ssl, SSL_CTX **ctx, const char *url)
{
  int rc = 0;

  *ctx = SSL_CTX_new(TLS_client_method());
  if (!*ctx)
  {
    printf("[ERROR] Failed: SSL_CTX_new: %s", ERR_error_string(ERR_get_error(), NULL));
    return false;
  }
  rc = SSL_CTX_load_verify_locations(*ctx, "/etc/ssl/certs/ca-certificates.crt", "/etc/ssl/certs/");
  if (!rc)
  {
    printf("[ERROR] Failed: SSL_CTX_load_verify_locations: %s", ERR_error_string(ERR_get_error(), NULL));
    return false;
  }
  *ssl = SSL_new(*ctx);
  if (!*ssl)
  {
    printf("[ERROR] Failed: SSL_new: %s", ERR_error_string(ERR_get_error(), NULL));
    return false;
  }
  rc = SSL_set_fd(*ssl, sockfd);
  if (!rc)
  {
    printf("[ERROR] Failed: SSL_set_fd: %s", ERR_error_string(ERR_get_error(), NULL));
    return false;
  }
  rc = SSL_set_tlsext_host_name(*ssl, url);
  if (!rc)
  {
    printf("[ERROR] Failed: SSL_set_tlsext_host_name: %s", ERR_error_string(ERR_get_error(), NULL));
    return false;
  }
  rc = SSL_connect(*ssl);
  if (rc != 1)
  {
    printf("[ERROR] Failed: SSL_connect: %s", ERR_error_string(ERR_get_error(), NULL));
    return false;
  }
  return true;
}

extern inline void http_request(HttpConnect *con, const char *req)
{
  da_append(&con->req, req);
}

extern inline void http_response_reset(HttpResponse *res)
{
  res->count = 0;
}

extern inline void http_send(HttpConnect con)
{
  int rc = 0;

  for (size_t i = 0; i < con.req.count; ++i)
  {
    rc = con.ssl && con.ctx ?
         SSL_write(con.ssl, con.req.items[i], strlen(con.req.items[i])) :
         send(con.sockfd, con.req.items[i], strlen(con.req.items[i]), 0);
    if (rc < 0)
    {
      printf("[ERROR] Failed to send message\n");
      return;
    }
  }
}

extern inline void _http_recv(HttpConnect con, struct http_recv_options opts)
{
  FILE*    fd          = NULL;
  long int bytes       = 0;
  int      bytes_recv  = 0;

  if (opts.file)
  {
    fd = fopen(opts.file, "wb");

    if (fd <= 0)
    {
      printf("[ERROR] Invalid file '%s'\n", opts.file);
      return;
    }
  }

  do {
    bytes_recv = con.ssl && con.ctx ?
                      SSL_read(con.ssl, __http__message, __MESSAGE_SIZE) :
                      recv(con.sockfd,  __http__message, __MESSAGE_SIZE, 0);

    if (bytes_recv < 0)
    {
      printf("[ERROR] Failed to read: %s", con.ssl && con.ctx ?
                                           ERR_error_string(ERR_get_error(), NULL) :
                                           "recv() < 0");

      memset(__http__message, 0, __MESSAGE_SIZE);
      return;
    }

    bytes += bytes_recv;

    if (fd)
      fwrite(__http__message, sizeof(unsigned char), bytes_recv, fd);

    if (opts.out)
      for (size_t i = 0; i < bytes_recv; ++i)
        da_append(opts.out, __http__message[i]);

    memset(__http__message, 0, __MESSAGE_SIZE);
  } while (bytes_recv);

  if (fd)
    fclose(fd);
}

extern inline void http_disconnect(HttpConnect *con)
{
  if (con->ssl && con->ctx)
  {
    SSL_set_shutdown(con->ssl, SSL_RECEIVED_SHUTDOWN | SSL_SENT_SHUTDOWN);
    SSL_shutdown(con->ssl);
    SSL_free(con->ssl);
    SSL_CTX_free(con->ctx);
    con->ssl = NULL;
    con->ctx = NULL;
  }

  close(con->sockfd);
  con->req.count = 0;
  memset(__http__message, 0, __MESSAGE_SIZE);
}

extern inline void http_close(HttpConnect con)
{
  free(con.req.items);
}
#endif // HTTP_IMPLEMENTATION
#endif // _HTTP_H_INCLUDE
