#ifndef base64_h_INCLUDED
#define base64_h_INCLUDED

static const char __encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                        '4', '5', '6', '7', '8', '9', '+', '/'};
static char __decoding_table[256] = {0};
static const int __mod_table[] = {0, 2, 1};

typedef struct {
  unsigned char *data;
  size_t count;
} Base64;

extern inline unsigned char *base64_encode(const unsigned char *decoded, size_t size, size_t *count);
extern inline unsigned char *base64_decode(unsigned char *encoded, size_t size, size_t *count);
extern inline Base64 base64_encode_st(unsigned char *decoded);
extern inline Base64 base64_decode_st(Base64 encoded);

#endif // base64_h_INCLUDED
