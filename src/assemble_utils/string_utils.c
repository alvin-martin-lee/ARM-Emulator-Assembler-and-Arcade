/** string_utils.c
 *
 * String manipulation functions.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"

/**
 * Returns a boolean depending if the two strings are equal, ignoring case.
 */
int strEqualNCS(const char *a, const char *b) {
  if (strlen(a) != strlen(b)) {
    return 0;
  }

  for (int i = 0; i < strlen(a); i++) {
    if (tolower(a[i]) != tolower(b[i])) {
      return 0;
    }
  }
  return 1;
}

/** 
 * Get the first `count` tokens of a string delimited by a non-whitespace character.
 * There may be whitespace after the delimiter. e.g. "r2,  r3, r4"
 *
 * IMPORTANT: the original string is modified, so make a copy first if it
 * is still needed later. This function works by replacing delimiters
 * with '\0' characters to distinguish between tokens.
 * 
 * If there are more than `count` delimiter-separated segments,
 * the last token will be whatever is left after the last delimiter character.
 * e.g. getTokens("r1, r2, r3, lsl r4", 3) == {"r1", "r2", "r3, lsl r4"}
 *      instead of {"r1", "r2", "r3", "lsl 4"}
 *
 * If there are less than `count` segments, an error
 * will be reported and the program exits. However, it won't error-check
 * the last token.
 * e.g. getTokens("a,b", 4) throws an error
 *      getTokens("a,b", 3) == {"a", "b", ""} // last one is null string. 
 *      getTokens("abc", 2) == {"abc", ""}
 */
char **getTokensGeneric(char *string, int count, const char delim) {
  if (string == NULL || strlen(string) == 0) {
    fprintf(stderr, "Null or empty string provided.\n");
    exit(EXIT_FAILURE);
  }
  if (count < 1) {
    fprintf(stderr, "Invalid token count provided.\n");
    exit(EXIT_FAILURE);
  }

  // const char delim = ',';
  char **tokens = malloc(sizeof(char *) * count);
  char *chr = string;
  int ntok = 0;
  while (ntok < count - 1) {
    // go to first non-space character
    while (*chr != '\0' && isspace(*chr)) ++chr;

    // reached end
    if (*chr == '\0') break;

    // save the start of token
    tokens[ntok++] = chr;

    // go to end of token
    while (*chr != '\0' && *chr != delim) ++chr;

    // reached end
    if (*chr == '\0') break;

    // overwrite delimiter with null-terminator
    *chr = '\0';

    // start looking for the next token
    ++chr;
  }

  // early termination?
  if (ntok < count - 1) {
    fprintf(stderr, "Warning: getTokens expected %d tokens but got %d\n.",
            count, ntok);
    exit(EXIT_FAILURE);
  }

  // put the rest of the unprocessed string in the array
  // (excluding front whitespace)
  while (*chr != '\0' && isspace(*chr)) ++chr;
  tokens[ntok] = chr;
  return tokens;
}

/**
 * Get the first `count` tokens of a comma-separated string.
 */
char **getTokens(char *string, int count) {
  return getTokensGeneric(string, count, ',');
}

/**
 * Get the index of a string within a string array, -1 if not found.
 */
int findStringIndexNCS(char *string, const char **array, unsigned int array_size) {
  if (string == NULL || strlen(string) == 0) {
    fprintf(stderr, "String is null or empty.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < array_size; i++) {
    if (strEqualNCS(string, array[i])) {
      return i;
    }
  }
  return -1;
}
