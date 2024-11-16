#include "string.h"

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
        s1++, s2++;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int memcmp(const void *s1, const void *s2, int n)
{
    const unsigned char *a = s1, *b = s2;
    while (n-- > 0) {
        if (*a != *b)
            return *a - *b;
        a++;
        b++;
    }
    return 0;
}

char *strncpy(char *dst, const char *src, int n)
{
    while (n-- && (*dst++ = *src++))
        ;
    return dst;
}

void *memset(void *s, int c, int n)
{
    unsigned char *p = s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

int strlen(const char *s)
{
    int len = 0;
    while (*s++ != '\0')
        len++;
    return len;
}

char *strchr(const char *s, int c)
{
    char ch = (char)c;
    while (*s) {
        if (*s == ch)
            return (char *)s;
        s++;
    }
    if (ch == '\0')
        return (char *)s;
    return 0;
}

char *strtok(char *s, const char *delim)
{
    static char *next = 0;
    if (s)
        next = s;
    if (!next)
        return 0;
    char *start = next;
    while (*start && strchr(delim, *start))
        start++;
    if (*start == '\0') {
        next = 0;
        return 0;
    }
    char *end = start;
    while (*end && !strchr(delim, *end))
        end++;
    if (*end) {
        *end = '\0';
        next = end + 1;
    } else {
        next = 0;
    }
    return start;
}
