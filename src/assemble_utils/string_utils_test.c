/** string_utils_test.c
 *
 * Usage of string-manipulation functions.
 */
#include <stdlib.h>
#include <stdio.h>

#include "string_utils.h"

void strEqualNCSTest() {
  printf("String equality (ignore case) test...\n");
  char a[] = "hello";
  char b[] = "hello";
  char c[] = "HELLO";
  char d[] = "world";
  char e[] = "hell";

  printf("%s == %s ? %s\n", a, b, strEqualNCS(a, b) ? "PASS": "FAIL");
  printf("%s == %s ? %s\n", a, c, strEqualNCS(a, c) ? "PASS": "FAIL");
  printf("%s == %s ? %s\n", a, d, !strEqualNCS(a, d) ? "PASS": "FAIL");
  printf("%s == %s ? %s\n", a, e, !strEqualNCS(a, e) ? "PASS": "FAIL");
}

void getTokensTest() {
  printf("Get tokens test...\n");
  char str[] = "r1,r2,r3";
  printf("'%s' -> ", str);
  char **t1 = getTokens(str, 3);
  for (int i = 0; i < 3; i++) {
    printf("'%s' ", t1[i]);
  }
  printf("\n");
  free(t1);

  char str2[] = "r1, r2, r3";
  printf("'%s' -> ", str2);
  char **t2 = getTokens(str2, 3);
  for (int i = 0; i < 3; i++) {
    printf("'%s' ", t2[i]);
  }
  printf("\n");
  free(t2);

  char str3[] = "r0, [r3, r2, lsl #2]";
  printf("'%s' -> ", str3);
  char **t3 = getTokens(str3, 2);
  for (int i = 0; i < 2; i++) {
    printf("'%s' ", t3[i]);
  }
  printf("\n");
  free(t3);

  char str4[] = "r2,r1,#2"; // "abcd"; // "r1, r2, r3, lsl r4";
  printf("'%s' -> ", str4);
  char **t4 = getTokens(str4, 3);
  for (int i = 0; i < 3; i++) {
    printf("'%s' ", t4[i]);
  }
  printf("\n");
  free(t4);

}

void findStringIndexNCSTest() {
  printf("Find string test...\n");
  const char *array[] = {"hello", "world", "test", "mov"};
  printf("'hello': %s\n", findStringIndexNCS("hello", array, 4) == 0 ? "PASS": "FAIL");
  printf("'world': %s\n", findStringIndexNCS("world", array, 4) == 1 ? "PASS": "FAIL");
  printf("'test': %s\n", findStringIndexNCS("test", array, 4) == 2 ? "PASS": "FAIL");
  printf("'mov': %s\n", findStringIndexNCS("mov", array, 4) == 3 ? "PASS": "FAIL");
  printf("'nothere': %s\n", findStringIndexNCS("nothere", array, 4) == -1 ? "PASS": "FAIL");
}

void allTests() {
  strEqualNCSTest();
  getTokensTest();
  findStringIndexNCSTest();
}

