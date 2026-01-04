#include "unity.h"

#define URL_SIZE 64
#define HLS_SIZE URL_SIZE*4

#define STRING_SLICE(format, string, split) string_format((format), strchr((string), (split)) - (string), (string))

// Source: https://github.com/p4ul17/voe-dl
char *deobfuscate(unsigned char *encoded)
{
  Base64 decoded   = {0};
  char*   step1    = NULL;
  char*   step2    = NULL;
  Base64  step3    = {0};
  char*   step4    = NULL;
  char*   step5    = NULL;

  step1   = string_rot13(encoded, strlen(encoded));
  step2   = string_remove(step1, (const char*[8]){"@$", "^^", "~@", "%?", "*~", "!!", "#&", NULL});
  step3   = base64_decode_st((Base64){ .data = step2, strlen(step2) });
  step4   = string_shift(step3.data, -3);
  step5   = string_reverse(step4);
  decoded = base64_decode_st((Base64){ .data = step5, strlen(step5) });

  return (char*)decoded.data;
}

struct request_opts {
  unsigned char *content;
  unsigned char *content_type;
};
#define request(con, type, host, filename, ...) _request((con), (type), (host), (filename), (struct request_opts){__VA_ARGS__})
HttpResponse _request(HttpConnect *con, const char *type, const char *host, const char *filename, struct request_opts opts)
{
  HttpResponse res = {0};

  http_connect(con, host, .secure = true);
    http_request(con, string_format("%s %s HTTP/1.0\r\n", type, filename));
    http_request(con, string_format("Host: %s\r\n", host));
    http_request(con, "User-Agent: curl/8.17.0\r\n");
    http_request(con, "Accept: */*\r\n");
    if (opts.content) http_request(con, string_format("Content-Length: %ld\r\n", strlen(opts.content)));
    if (opts.content_type) http_request(con, string_format("Content-Type: %s\r\n", opts.content_type));
    http_request(con, "\r\n");
    if (opts.content) http_request(con, opts.content);
    http_send(*con);
    http_recv(*con, .out = &res);
  http_disconnect(con);

  string_pop_pro(4);

  return res;
}

int main(void)
{
  HttpConnect con  = {0};
  HttpResponse res = {0};
  char* aniworld   = NULL;
  char* redirect   = NULL;
  char* voe        = NULL;
  char* encoded    = NULL;
  char* decoded    = NULL;
  char* hls        = NULL;
  int rc           = 0;

  res = request(&con, "GET", "aniworld.to", "/anime/stream/monster/staffel-1/episode-1");
  assert(!strstr((char*)res.items, "Die gewÃ¼nschte Serie wurde nicht gefunden oder ist im Moment deaktiviert."));
  
  redirect = string_jump_over((char*)res.items, "data-link-target=\"");
  assert(redirect);
  aniworld = STRING_SLICE("https://aniworld.to%.*s", redirect, '"');

  res = request(&con, "GET", "aniworld.to", string_format("/redirect%s", strrchr(aniworld, '/')));
  redirect = string_jump_over((char*)res.items, "Location: ");
  assert(redirect);
  redirect = STRING_SLICE("%.*s", redirect, '\r');
  redirect = string_jump_over(redirect, "https://voe.sx");

  res = request(&con, "GET", "voe.sx", redirect);
  redirect = (char*)res.items;

  redirect = string_jump_over((char*)res.items, "window.location.href = '");
  assert(redirect);
  voe = STRING_SLICE("%.*s", redirect, '\'');

  redirect = string_jump_over(voe, "https://");
  redirect = STRING_SLICE("%.*s", redirect, '/');

  res = request(&con, "GET", redirect, voe + strlen("https://") + strlen(redirect));
  encoded = string_jump_over((char*)res.items, "type=\"application/json\">[\"");
  encoded = (unsigned char*)STRING_SLICE("%.*s", encoded, '\"');

  decoded = deobfuscate(encoded);

  hls = decoded;
  hls = string_jump_over(hls, "\"source\":\"");
  hls = STRING_SLICE("%.*s", hls, '"');
  hls = string_remove(hls, (const char*[2]){"\\", NULL});
  printf("%s\n", hls);

  printf("[INFO] Starting MPV\n");


  rc = cmd("/usr/bin/mpv", "mpv", hls);
  if (rc) return rc;

  http_close(con, .received = &res);
  string_free();

  return 0;
}
