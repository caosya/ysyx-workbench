#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  // panic("Not implemented");
  size_t len = 0;
  while(s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  // panic("Not implemented");
  size_t i = 0;
  while(src[i] != '\0') {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  // panic("Not implemented");
  size_t i;
  for(i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  for(; i < n; i++) {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  // panic("Not implemented");
  size_t dst_len = strlen(dst);
  size_t i;

  for(i = 0; src[i] != '\0'; i++) {
    dst[dst_len+i] = src[i];
  }
  dst[dst_len+i] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  // panic("Not implemented");
  int i;
  for (i = 0; s1[i] != '\0' && s2[i] != '\0'; i++){
    if (s1[i] != s2[i])
      return (int)(s1[i]) - (int)(s2[i]);
  }
  //case 同时结束：为0
  //case s1先结束，0-x < 0
  //case s2先结束，x-0 > 0
  return (int)(s1[i]) - (int)(s2[i]);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  // panic("Not implemented");
  int i;
  for (i = 0; i < n && s1[i] != '\0' && s2[i] != '\0'; i++){
    if (s1[i] != s2[i])
      return (int)(s1[i]) - (int)(s2[i]);
  }
  //case 同时结束：为0
  //case s1先结束，0-x < 0
  //case s2先结束，x-0 > 0
  //case 到达n，返回0
  if (i == n)
    return 0;
  return (int)(s1[i]) - (int)(s2[i]);
}

void *memset(void *s, int c, size_t n) {
  // panic("Not implemented");
  unsigned char* target = (unsigned char*)s;
  for (int i = 0; i < n; i++){
    target[i] = (unsigned char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  // assert(0);
  char *char_out = (char *)out;
  const char *char_in = (const char *)in;
  for (int i = 0; i < n; ++i){
    char_out[i] = char_in[i];
  }

  return out;
  // panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  // panic("Not implemented");
  unsigned char *char_s1 = (unsigned char *)s1;
  unsigned char *char_s2 = (unsigned char *)s2;

  int i;
  for (i = 0; i < n; ++i){
    if (char_s1[i] != char_s2[i])
      return (int)(char_s1[i]) - (int)(char_s2[i]);
  }
  if (i == n)
    return 0;
  return (int)(char_s1[i]) - (int)(char_s2[i]);
}

#endif
