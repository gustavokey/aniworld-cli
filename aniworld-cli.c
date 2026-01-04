#include "unity.h"

#define URL_SIZE 64
#define HLS_SIZE URL_SIZE*4

char *retrieve_entry(char *json, int n)
{
  char *entry = NULL;

  entry = json;

  do {
    entry = strchr(entry, '{');
    if (entry) ++entry;
    --n;
  } while (entry && n);

  if (entry) entry = string_slice("{\n  %.*s\n}", entry, '}');

  return entry;
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

void help(void)
{
  printf("[INFO] aniworld-cli watch  show staffel episode\n");
  printf("[INFO] aniworld-cli search show\n");
}

int main(int argc, char **argv)
{
  HttpConnect con  = {0};
  HttpResponse res = {0};
  char* show       = NULL;
  char* staffel    = NULL;
  char* episode    = NULL;
  char* aniworld   = NULL;
  char* redirect   = NULL;
  char* voe        = NULL;
  char* encoded    = NULL;
  char* decoded    = NULL;
  char* hls        = NULL;
  char* entry      = NULL;
  int rc           = 0;

  // ./aniworld-cli watch  show staffel episode
  // ./aniworld-cli search show
  if (argc < 3)
  {
    printf("[INFO] Not enough args\n");
    help();
    return 1;
  }

  if (strcmp(argv[1], "watch") == 0)
  {
    if (argc < 5)
    {
      help();
      return 0;
    }

    show    = argv[2];
    staffel = argv[3];
    episode = argv[4];

    res = request(&con, "GET", "aniworld.to", string_format("/anime/stream/%s/%s/%s", show, staffel, episode));
    assert(!strstr((char*)res.items, "Die gewÃ¼nschte Serie wurde nicht gefunden oder ist im Moment deaktiviert."));
    
    redirect = string_jump_over((char*)res.items, "data-link-target=\"");
    assert(redirect);
    aniworld = string_slice("https://aniworld.to%.*s", redirect, '"');

    res = request(&con, "GET", "aniworld.to", string_format("/redirect%s", strrchr(aniworld, '/')));
    redirect = string_jump_over((char*)res.items, "Location: ");
    assert(redirect);
    redirect = string_slice("%.*s", redirect, '\r');
    redirect = string_jump_over(redirect, "https://voe.sx");

    res = request(&con, "GET", "voe.sx", redirect);
    redirect = (char*)res.items;

    redirect = string_jump_over((char*)res.items, "window.location.href = '");
    assert(redirect);
    voe = string_slice("%.*s", redirect, '\'');

    redirect = string_jump_over(voe, "https://");
    redirect = string_slice("%.*s", redirect, '/');

    res = request(&con, "GET", redirect, voe + strlen("https://") + strlen(redirect));
    encoded = string_jump_over((char*)res.items, "type=\"application/json\">[\"");
    encoded = (unsigned char*)string_slice("%.*s", encoded, '\"');

    decoded = deobfuscate(encoded);

    hls = decoded;
    hls = string_jump_over(hls, "\"source\":\"");
    hls = string_slice("%.*s", hls, '"');
    hls = string_remove(hls, (const char*[2]){"\\", NULL});
    printf("%s\n", hls);

    printf("[INFO] Starting MPV\n");

    rc = cmd("/usr/bin/mpv", "mpv", hls);
    if (rc) return rc;

    http_close(con, .received = &res);
    string_free();
  }
  else if (strcmp(argv[1], "search") == 0)
  {
    res = request(&con,
                  "POST", "aniworld.to", "/ajax/search",
                    .content      = string_format("keyword=%s", argv[2]),
                    .content_type = "application/x-www-form-urlencoded");
    char *json = string_remove(res.items, (const char *[4]){"\\", "<em>", "</em>", NULL});

    for (int i = 1; (entry = retrieve_entry(json, i)); ++i)
      printf("%s\n", entry);

    http_close(con, .received = &res);
    string_free();
  }


  return 0;
}
