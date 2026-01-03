#include "unity.h"

#define URL_SIZE 64
#define HLS_SIZE URL_SIZE*4

#define STRING_SLICE(format, string, split) string_format((format), strchr((string), (split)) - (string), (string))

char *__reverse(unsigned char *data)
{
  size_t size   = 0;
  size_t length = 0;

  size = strlen(data) - 1;
  length = size;

  for (size_t i = 0; i < length/2 + 1; ++i)
  {
    unsigned char c = data[i];
    data[i] = data[size];
    data[size] = c;
    size--;
  }

  return data;
}

char *__shift(char *data, int shift)
{
  for (size_t i = 0; data[i]; ++i)
    data[i] = (unsigned char)(data[i] + shift);

  return data;
}

char *__remove(char *data, const char *pattern[])
{
  char *replaced = NULL;
  char *found = NULL;
  size_t size = 0;

  replaced = string_format("%s", data);
  size = strlen(data);

  for (int i = 0; pattern[i]; ++i)
  {
    do {
        found = strstr(replaced, pattern[i]);

        if (found)
          for (int c = 0; c < strlen(pattern[i]); ++c)
            *(found + c) = '\b';
    } while (found);
  }

  for (size_t i = 0, j = 0; replaced[i]; ++i)
  {
    if (replaced[i] != '\b') data[j++] = replaced[i];
    if (!replaced[i + 1]) memset(data + j, 0, size - j);
  }

  string_pop();

  return data;
}

// Source: https://hea-www.harvard.edu/~fine/Tech/rot13.html
char *__rot13(char *decoded, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    if ((decoded[i] >= 'A' && decoded[i] <= 'Z') || (decoded[i] >= 'a' && decoded[i] <= 'z'))
      decoded[i] = ~(~decoded[i])-1/(~(~decoded[i]|32)/13*2-11)*13;

  return decoded;
}

// Source: https://github.com/p4ul17/voe-dl
char *deobfuscate(unsigned char *encoded)
{
  Base64 decoded   = {0};
  char*   step1    = NULL;
  char*   step2    = NULL;
  Base64  step3    = {0};
  char*   step4    = NULL;
  char*   step5    = NULL;

  step1   = __rot13(encoded, strlen(encoded));
  step2   = __remove(step1, (const char*[8]){"@$", "^^", "~@", "%?", "*~", "!!", "#&", NULL});
  step3   = base64_decode_st((Base64){ .data = step2, strlen(step2) });
  step4   = __shift(step3.data, -3);
  step5   = __reverse(step4);
  decoded = base64_decode_st((Base64){ .data = step5, strlen(step5) });

  return (char*)decoded.data;
}

HttpResponse request(HttpConnect *con, const char *type, const char *host, const char *filename)
{
  HttpResponse res = {0};

  http_connect(con, host, .secure = true);
    http_request(con, string_format("%s %s HTTP/1.0\r\n", type, filename));
    http_request(con, string_format("Host: %s\r\n", host));
    http_request(con, "User-Agent: curl/8.17.0\r\n"
                      "Accept: */*\r\n"
                      "\r\n");
    http_send(*con);
    http_recv(*con, .out = &res);
  http_disconnect(con);

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
  
  redirect = string_goto((char*)res.items, "data-link-target=\"");
  assert(redirect);
  aniworld = STRING_SLICE("https://aniworld.to%.*s", redirect, '"');

  res = request(&con, "GET", "aniworld.to", string_format("/redirect%s", strrchr(aniworld, '/')));
  redirect = string_goto((char*)res.items, "Location: ");
  assert(redirect);
  redirect = STRING_SLICE("%.*s", redirect, '\r');
  redirect = string_goto(redirect, "https://voe.sx");

  res = request(&con, "GET", "voe.sx", redirect);
  redirect = (char*)res.items;

  redirect = string_goto((char*)res.items, "window.location.href = '");
  assert(redirect);
  voe = STRING_SLICE("%.*s", redirect, '\'');

  redirect = string_goto(voe, "https://");
  redirect = STRING_SLICE("%.*s", redirect, '/');

  res = request(&con, "GET", redirect, voe + strlen("https://") + strlen(redirect));
  encoded = string_goto((char*)res.items, "type=\"application/json\">[\"");
  encoded = (unsigned char*)STRING_SLICE("%.*s", encoded, '\"');

  decoded = deobfuscate(encoded);

  hls = decoded;
  hls = string_goto(hls, "\"source\":\"");
  hls = STRING_SLICE("%.*s", hls, '"');
  hls = __remove(hls, (const char*[2]){"\\", NULL});
  printf("%s\n", hls);

  printf("[INFO] Starting MPV\n");


  rc = cmd("/usr/bin/mpv", "mpv", hls);
  if (rc) return rc;

  http_close(con, .received = &res);
  string_free();

  return 0;
}
