#include "utils.h"

/**
 * @brief Convert a hexadecimal string to integer
 * 
 * @param s hexadecimal string
 * @param n length of the string
 * @return integer value
 */
int hextoi(const char *s, int n)
{
    int r = 0;
    while (n-- > 0) {
        r = r << 4;
        if (*s >= 'A')
            r += *s++ - 'A' + 10;
        else if (*s >= 0)
            r += *s++ - '0';
    }
    return r;
}

/**
 * @brief Align a number to the nearest multiple of a given number
 * 
 * @param n number
 * @param byte alignment
 * @return aligned number
 */
int align(int n, int byte)
{
    return (n + byte - 1) & ~(byte - 1);
}
