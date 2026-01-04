// Source: https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
extern inline unsigned char *base64_encode(const unsigned char *decoded, size_t size, size_t *count)
{
  uint32_t octet_a = 0;
  uint32_t octet_b = 0;
  uint32_t octet_c = 0;
  unsigned char *encoded = NULL;

  *count = 4 * ((size + 2) / 3);

  encoded = (unsigned char*)string_scratch(*count);

  for (size_t i = 0, j = 0; i < size;)
  {
    octet_a = i < size ? (unsigned char)decoded[i++] : 0;
    octet_b = i < size ? (unsigned char)decoded[i++] : 0;
    octet_c = i < size ? (unsigned char)decoded[i++] : 0;

    uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

    encoded[j++] = __encoding_table[(triple >> 3 * 6) & 0x3F];
    encoded[j++] = __encoding_table[(triple >> 2 * 6) & 0x3F];
    encoded[j++] = __encoding_table[(triple >> 1 * 6) & 0x3F];
    encoded[j++] = __encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (int i = 0; i < __mod_table[size % 3]; i++)
    encoded[*count - 1 - i] = '=';

  return encoded;
}


extern inline unsigned char *base64_decode(unsigned char *encoded, size_t size, size_t *count)
{
  uint32_t sextet_a = 0;
  uint32_t sextet_b = 0;
  uint32_t sextet_c = 0;
  uint32_t sextet_d = 0;
  uint32_t triple   = 0;
  unsigned char *decoded = NULL;

  if (size % 4 != 0)
    return NULL;

  *count = size / 4 * 3;
  if (encoded[size - 1] == '=') (*count)--;
  if (encoded[size - 2] == '=') (*count)--;

  decoded = (unsigned char*)string_scratch(*count);

  for (int i = 0; i < 64; i++)
    __decoding_table[(unsigned char) __encoding_table[i]] = i;

  for (size_t i = 0, j = 0; i < size;)
  {
    sextet_a = encoded[i] == '=' ? 0 & i++ : (uint32_t)__decoding_table[(uint32_t)encoded[i++]];
    sextet_b = encoded[i] == '=' ? 0 & i++ : (uint32_t)__decoding_table[(uint32_t)encoded[i++]];
    sextet_c = encoded[i] == '=' ? 0 & i++ : (uint32_t)__decoding_table[(uint32_t)encoded[i++]];
    sextet_d = encoded[i] == '=' ? 0 & i++ : (uint32_t)__decoding_table[(uint32_t)encoded[i++]];

    triple = (sextet_a << 3 * 6)
    + (sextet_b << 2 * 6)
    + (sextet_c << 1 * 6)
    + (sextet_d << 0 * 6);

    if (j < *count) decoded[j++] = (triple >> 2 * 8) & 0xFF;
    if (j < *count) decoded[j++] = (triple >> 1 * 8) & 0xFF;
    if (j < *count) decoded[j++] = (triple >> 0 * 8) & 0xFF;
  }

  return decoded;
}

extern inline Base64 base64_encode_st(unsigned char *decoded)
{
  Base64 encoded = {0};

  encoded.data = base64_encode(decoded, strlen((char*)decoded), &encoded.count);

  return encoded;
}

extern inline Base64 base64_decode_st(Base64 encoded)
{
  Base64 decoded = {0};

  decoded.data = base64_decode(encoded.data, encoded.count, &decoded.count);

  return decoded;
}
