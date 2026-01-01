#define HTTP_IMPLEMENTATION
#include "http.h"
#include "cmd.h"

#define URL_SIZE 64
#define HLS_SIZE URL_SIZE*4

int main(void)
{
  HttpConnect  con        = {0};
  HttpResponse res        = {0};
  char aniworld[URL_SIZE] = "https://aniworld.to";
  char vidmoly[URL_SIZE]  = {0};
  char hls[HLS_SIZE]      = {0};
  char *redirect          = NULL;
  char *sources           = NULL;

  http_connect(&con, "aniworld.to", .secure = true);
    http_request(&con,
                 "GET /anime/stream/monster/staffel-1/episode-32 HTTP/1.0\r\n"
                 "Host: aniworld.to\r\n"
                 "User-Agent: curl/8.17.0\r\n"
                 "Accept: */*\r\n"
                 "\r\n");
    http_send(con);
    http_recv(con, .out = &res);

    redirect = strstr((char*)res.items, "data-link-target=");
    assert(redirect && "[ERROR] Did not found redirect in html (1)");
    redirect = strstr(redirect + 1, "data-link-target=");
    assert(redirect && "[ERROR] Did not found redirect in html (2)");
    redirect = strstr(redirect + 1, "data-link-target=");
    assert(redirect && "[ERROR] Did not found redirect in html (3)");

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
    assert(redirect && "[ERROR] Failed to redirect");
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
    assert(sources && "[ERROR] Failed to find player");
    sources = strstr(sources, "file:");
    assert(sources && "[ERROR] Failed to find file sources");
    sources += 6;
    memcpy(hls, sources, strchr(sources + 1, '"') - sources);
    printf("[INFO] Found hls stream\n");
  http_disconnect(&con);

  http_close(con);

  printf("[INFO] Starting MPV\n");
  return cmd("/usr/bin/mpv", "mpv", hls);
}

#include "cmd.c"
