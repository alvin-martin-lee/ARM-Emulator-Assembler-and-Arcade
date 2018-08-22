/** string_utils.h
 *
 * String manipulation functions.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

int strEqualNCS(const char *a, const char *b);

char **getTokensGeneric(char *string, int count, char delim);

char **getTokens(char *string, int count);

int findStringIndexNCS(char *string, const char **array, unsigned int array_size);

#endif /* end STRING_UTILS_H */
