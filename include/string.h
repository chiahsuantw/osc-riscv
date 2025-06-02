#pragma once

int strcmp(const char *s1, const char *s2);
int memcmp(const void *s1, const void *s2, int n);
char *strncpy(char *dst, const char *src, int n);
void *memset(void *s, int c, int n);
int strlen(const char *s);
char *strchr(const char *s, int c);
char *strtok(char *s, const char *delim);
void *memcpy(void *dst, const void *src, int n);
char *strcat(char *dst, const char *src);
