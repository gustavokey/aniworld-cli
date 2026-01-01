#define HTTP_IMPLEMENTATION
#include "http.h"
#include "cmd.h"
#include "fmt.h"

#define URL_SIZE 64
#define HLS_SIZE URL_SIZE*4

extern inline bool string_is_number(const char *s)
{
  const char *ps = s;
  while (*ps)
  {
    if (!(*ps >= '0' && *ps <= '9')) return false;
    ++ps;
  }
  return true;
}

void help(void)
{
  printf("[INFO] Usage:\n");
  printf("[INFO] aniworld_cli [show] [staffel] [episode]\n");
}

int main(int argc, const char **argv)
{
  HttpConnect  con        = {0};
  HttpResponse res        = {0};
  char aniworld[URL_SIZE] = "https://aniworld.to";
  char vidmoly[URL_SIZE]  = {0};
  char hls[HLS_SIZE]      = {0};
  const char *redirect    = NULL;
  const char *sources     = NULL;
  const char *show        = NULL;
  const char *season      = NULL;
  const char *episode     = NULL;

  if (argc < 4)
  {
    help();
    return 0;
  }

  show    = argv[1];
  season  = argv[2];
  episode = argv[3];

  if (!string_is_number(season) || !string_is_number(episode))
  {
    help();
    return 0;
  }

  http_connect(&con, "aniworld.to", .secure = true);
    http_request(&con,
                 format_string("GET /anime/stream/%s/staffel-%s/episode-%s HTTP/1.0\r\n", show, season, episode));
    http_request(&con,
                 "Host: aniworld.to\r\n"
                 "User-Agent: curl/8.17.0\r\n"
                 "Accept: */*\r\n"
                 "\r\n");
    http_send(con);
    http_recv(con, .out = &res);

    redirect = strstr((char*)res.items, "data-link-target=");
    assert(redirect && "Show was not found");

    redirect = strstr(redirect + 1, "data-link-target=");
    assert(redirect && "Could not iterate over providers");
    redirect = strstr(redirect + 1, "data-link-target=");
    assert(redirect && "Missing reliable provider (Vidmoly)");

    redirect = redirect + strlen("data-link-target=") + 1;
    memcpy(aniworld + strlen(aniworld), redirect, strlen("/redirect/......."));
  http_disconnect(&con);

  http_response_reset(&res);

  printf("[INFO] Redirecting to: %s\n", aniworld);

  http_connect(&con, "aniworld.to", .secure = true);
    http_request(&con, "GET ");
    http_request(&con, aniworld + strlen("https://aniworld.to"));
    http_request(&con, " HTTP/1.0\r\n");
    http_request(&con,
                 "Host: aniworld.to\r\n"
                 "User-Agent: curl/8.17.0\r\n"
                 "Accept: */*\r\n"
                 "\r\n");
    http_send(con);
    http_recv(con, .out = &res);
    redirect = (char*)((uintptr_t)strstr((char*)res.items, "Location: ") | (uintptr_t)strstr((char*)res.items, "location: "));
    assert(redirect && "Could not redirect");
    memcpy(vidmoly, redirect + strlen("Location: "), strlen("https://vidmoly.net/embed-.............html"));
  http_disconnect(&con);

  http_response_reset(&res);

  printf("[INFO] Using host: %s\n", vidmoly);

  http_connect(&con, "vidmoly.net", .secure = true);
    http_request(&con, "GET ");
    http_request(&con, vidmoly + strlen("https://vidmoly.net"));
    http_request(&con, " HTTP/1.0\r\n");
    http_request(&con,
                 "Host: vidmoly.net\r\n"
                 "User-Agent: curl/8.17.0\r\n"
                 "Accept: */*\r\n"
                 "\r\n");
    http_send(con);
    http_recv(con, .out = &res);
    sources = strstr((char*)res.items, "player.setup");
    assert(sources && "Player was not found");
    sources = strstr(sources, "file:");
    assert(sources && "File sources is missing");
    sources += 6;
    memcpy(hls, sources, strchr(sources + 1, '"') - sources);
    printf("[INFO] Found hls stream\n");
  http_disconnect(&con);

  http_close(con);

  printf("[INFO] Starting MPV\n");
  return cmd("/usr/bin/mpv", "mpv", hls);
}

#include "cmd.c"
#include "fmt.c"
