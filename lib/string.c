#include "string.h"

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && *s1 == *s2) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while ((*d++ = *src++));
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *d = dst;
    while (n && (*d++ = *src++)) n--;
    while (n--) *d++ = '\0';
    return dst;
}

char *strcat(char *dst, const char *src) {
    char *d = dst;
    while (*d) d++;
    while ((*d++ = *src++));
    return dst;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    if (c == '\0') return (char*)s;
    return NULL;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t*)s;
    while (n--) *p++ = (uint8_t)c;
    return s;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dst;
    const uint8_t *s = (const uint8_t*)src;
    while (n--) *d++ = *s++;
    return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *a = (const uint8_t*)s1;
    const uint8_t *b = (const uint8_t*)s2;
    while (n--) {
        if (*a != *b) return *a - *b;
        a++;
        b++;
    }
    return 0;
}

int atoi(const char *s) {
    int result = 0;
    int sign = 1;
    while (*s == ' ') s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return sign * result;
}

void itoa(int64_t val, char *buf, int base) {
    char tmp[65];
    int i = 0;
    bool neg = false;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    if (val < 0 && base == 10) {
        neg = true;
        val = -val;
    }

    while (val > 0) {
        int d = val % base;
        tmp[i++] = d < 10 ? '0' + d : 'A' + d - 10;
        val /= base;
    }

    int j = 0;
    if (neg) buf[j++] = '-';
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void utoa(uint64_t val, char *buf, int base) {
    char tmp[65];
    int i = 0;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (val > 0) {
        int d = val % base;
        tmp[i++] = d < 10 ? '0' + d : 'A' + d - 10;
        val /= base;
    }

    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}
