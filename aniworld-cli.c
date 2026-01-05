#include "unity.h"

#define URL_SIZE 64
#define HLS_SIZE URL_SIZE*4
#define MAX_SEARCH_RESULTS 8

void help(void)
{
  printf("Usage Examples\n");
  printf("\n");
  printf("Start new watch:\n");
  printf("  aniworld-cli \"One Punch Man\"");
  printf("\n");
  printf("\n");
  printf("Choose specific staffel & episode\n");
  printf("  aniworld-cli \"Watamote\" 1 5");
  printf("\n");
  printf("\n");
  printf("Watch the movie\n");
  printf("  aniworld-cli \"Watamote\" 0");
  printf("\n");
  printf("\n");
  printf("aniworld-cli watch      show [staffel] [episode]\n");
  printf("aniworld-cli watch-url  show [staffel] [episode]\n");
  printf("aniworld-cli search     show\n");
}

struct request_opts {
  const char *file;
  unsigned char *content;
  unsigned char *content_type;
};
#define request(con, type, host, filename, ...) _request((con), (type), (host), (filename), (struct request_opts){__VA_ARGS__})
HttpResponse _request(HttpConnect *con, const char *type, const char *host, const char *filename, struct request_opts opts);
char *deobfuscate(unsigned char *encoded);
char *json_entry(char *json, int n);
void json_get_values(char **buff, char *json, const char *key);
char *aniworld_search(const char *show);

int main(int argc, char **argv)
{
  HttpConnect  con = {0};
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
  char* json       = NULL;
  int option       = 0;
  int rc           = 0;
  bool watch       = false;
  bool watch_url   = false;
  bool is_movie    = false;
  char* titles[MAX_SEARCH_RESULTS] = {0};
  char* links[MAX_SEARCH_RESULTS]  = {0};

  if (argc < 3)
  {
    help();
    return 1;
  }

  watch     = (strcmp(argv[1], "watch") == 0);
  watch_url = (strcmp(argv[1], "watch-url") == 0);

  if (watch || watch_url)
  {
    if (argc < 3)
    {
      help();
      return 0;
    }

    if (!watch_url)
    {
      json = aniworld_search(argv[2]);
      assert(json && "Nicht gefunden");

      json_get_values(titles, json, "\"title\":\"");
      json_get_values(links,  json, "\"link\":\"");

      printf("Select your choice\n");
      printf("> [0] Quit\n");
      for (int i = 0; titles[i] && i < MAX_SEARCH_RESULTS; ++i)
        printf("> [%d] %s\n", i + 1, titles[i]);

      option = getchar();
      option -= '0';

      if (option > 7 || option < 1)
        goto _defer;

      if (strstr(links[option - 1], "staffel-0")) is_movie = true;

      show    = links[option - 1];
      staffel = is_movie ? "" : string_format("/staffel-%s", argc > 3 ? argv[3] : "1");
      episode = is_movie ? "" : string_format("/episode-%s", argc > 4 ? argv[4] : "1");
    }
    else
    {
      show    = string_format("/anime/stream/%s", argv[2]);
      staffel = string_format("/staffel-%s", argv[3]);
      episode = string_format("/episode-%s", argv[4]);
    }

    printf("[INFO] URL: https://aniworld.to%s%s%s\n", show, staffel, episode);

    // 1st: Voe       ... data-link-target=/redirect/xxxxxxx ...
    // 2nd: Filemoon  ... data-link-target=/redirect/yyyyyyy ...
    // 3rd: Vidmoly   ... data-link-target=/redirect/zzzzzzz ...

    res = request(&con, "GET", "aniworld.to", string_format("%s%s%s", show, staffel, episode));
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
  }
  else if (strcmp(argv[1], "search") == 0)
  {
    json = aniworld_search(argv[2]);

    for (int i = 1; (entry = json_entry(json, i)); ++i)
      printf("%s\n", entry);
  }

_defer:
  http_close(con, .received = &res);
  string_release();

  return 0;
}

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

char *deobfuscate(unsigned char *encoded)
{
  Base64 decoded = {0};
  char*   step1  = NULL;
  char*   step2  = NULL;
  Base64  step3  = {0};
  char*   step4  = NULL;
  char*   step5  = NULL;

  // Source: https://github.com/p4ul17/voe-dl
  step1   = string_rot13(encoded, strlen(encoded));
  step2   = string_remove(step1, (const char*[8]){"@$", "^^", "~@", "%?", "*~", "!!", "#&", NULL});
  step3   = base64_decode_st((Base64){ .data = step2, strlen(step2) });
  step4   = string_shift(step3.data, -3);
  step5   = string_reverse(step4);
  decoded = base64_decode_st((Base64){ .data = step5, strlen(step5) });

  return (char*)decoded.data;
}

char *json_entry(char *json, int n)
{
  char *entry = NULL;

  entry = json;

  do {
    entry = strchr(entry, '{');
    if (entry) ++entry;
    --n;
  } while (entry && n);

  if (entry) entry = string_slice("%.*s", entry, "}", .trim = true);

  return entry;
}

void json_get_values(char **buff, char *json, const char *key)
{
  char *entry = NULL;

  for (int i = 0, j = 0; (entry = json_entry(json, i + 1)) && j < 8; ++i)
  {
    if (strstr(entry, "/anime/stream")) buff[j++] = entry;
    else string_pop();
  }

  assert(*buff && "Nicht gefunden");

  for (int i = 0; buff[i] && i < MAX_SEARCH_RESULTS; ++i)
  {
    buff[i] = string_jump_over(buff[i], key);
    buff[i] = string_slice("%.*s", buff[i], "\"", .trim = true);
  }
}

char *aniworld_search(const char *show)
{
  HttpConnect  con = {0};
  HttpResponse res = {0};
  char *json = NULL;

  res = request(&con,
                "POST", "aniworld.to", "/ajax/search",
                  .content      = string_format("keyword=%s", show),
                  .content_type = "application/x-www-form-urlencoded");

  json = string_remove(res.items, (const char *[8]){"\\", "<em>", "</em>", "u00bb", "u00ba", "u00ab", "u2019", NULL});

  return json;
}
