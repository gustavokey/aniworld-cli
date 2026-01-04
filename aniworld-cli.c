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

  if (entry) entry = string_slice("{\n  %.*s\n}", entry, "}", .trim = true);

  return entry;
}

// Source: https://github.com/p4ul17/voe-dl
char *deobfuscate(unsigned char *encoded)
{
  Base64 decoded = {0};
  char*   step1  = NULL;
  char*   step2  = NULL;
  Base64  step3  = {0};
  char*   step4  = NULL;
  char*   step5  = NULL;

  step1   = string_rot13(encoded, strlen(encoded));
  step2   = string_remove(step1, (const char*[8]){"@$", "^^", "~@", "%?", "*~", "!!", "#&", NULL});
  step3   = base64_decode_st((Base64){ .data = step2, strlen(step2) });
  step4   = string_shift(step3.data, -3);
  step5   = string_reverse(step4);
  decoded = base64_decode_st((Base64){ .data = step5, strlen(step5) });

  return (char*)decoded.data;
}

struct request_opts {
  const char *file;
  unsigned char *content;
  unsigned char *content_type;
};
#define request(con, type, host, filename, ...) _request((con), (type), (host), (filename), (struct request_opts){__VA_ARGS__})
HttpResponse _request(HttpConnect *con, const char *type, const char *host, const char *filename, struct request_opts opts)
{
  HttpResponse res = {0};
  size_t allocated = 0;

  allocated = string_get_count();

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
    http_recv(*con, .out = &res, .file = opts.file);
  http_disconnect(con);

  allocated = string_get_count() - allocated;

  string_pop_pro(allocated);

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

    // 1st: Voe       ... data-link-target=/redirect/xxxxxxx ...
    // 2nd: Filemoon  ... data-link-target=/redirect/yyyyyyy ...
    // 3rd: Vidmoly   ... data-link-target=/redirect/zzzzzzz ...

    res = request(&con, "GET", "aniworld.to", string_format("/anime/stream/%s/staffel-%s/episode-%s", show, staffel, episode));
    assert(!strstr((char*)res.items, "Die gewÃ¼nschte Serie wurde nicht gefunden oder ist im Moment deaktiviert."));
    // @Note data-link-target="/redirect/......."
    // @Note Request redirection to provider
    printf("[INFO] Using provider: voe.sx\n");

    redirect = string_jump_over((char*)res.items, "data-link-target=\"");
    assert(redirect && "Couldn't find redirect link");
    aniworld = string_slice("https://aniworld.to%.*s", redirect, "\"", .trim = true);

    // Receive redirect URL
    res = request(&con, "GET", "aniworld.to", string_format("/redirect%s", strrchr(aniworld, '/')));
    redirect = string_jump_over((char*)res.items, "Location: ");
    assert(redirect && "Unexpected header, missing location entry");
    redirect = string_slice("%.*s", redirect, "\r", .trim = true);
    redirect = string_jump_over(redirect, "https://voe.sx");

    // "window.location.href = 'https://crystaltreatmenteast.com/e/jfyxfr84fvot'"
    res = request(&con, "GET", "voe.sx", redirect);
    redirect = string_jump_over((char*)res.items, "window.location.href = '");
    assert(redirect && "'window.location.href' was not found");
    voe = string_slice("%.*s", redirect, "'", .trim = true);
    redirect = string_jump_over(voe, "https://");
    redirect = string_slice("%.*s", redirect, "/", .trim = true);

    // Final step, we need to decrypt the enigma machine.
    res = request(&con, "GET", redirect, voe + strlen("https://") + strlen(redirect));
    encoded = string_jump_over((char*)res.items, "type=\"application/json\">[\"");
    encoded = (unsigned char*)string_slice("%.*s", encoded, "\"", .trim = true);

    printf("[INFO] Deobfuscating JSON containing hls stream\n");
    decoded = deobfuscate(encoded);
    printf("[INFO] Done, enjoy!\n");

    hls = decoded;
    hls = string_jump_over(hls, "\"source\":\"");
    hls = string_slice("%.*s", hls, "\"", .trim = true);
    hls = string_remove(hls, (const char*[2]){"\\", NULL});

    printf("[INFO] Starting MPV now\n");

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
