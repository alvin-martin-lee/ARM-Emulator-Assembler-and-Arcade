/* hash_table_test.c
 *
 * Example usage of the hash table.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"

#define MAX_LINE_LENGTH 128

/* simple test with manual insertion */
void simpleTest() {
  // constructor
  HashTable *ht = HashTable_create();

  // Add some test data
  HashTable_put(ht, "loop", 123);
  HashTable_put(ht, "goto", 456543);
  // "hello" and "index" have the same hash value
  HashTable_put(ht, "hello", 456543);
  HashTable_put(ht, "index", 356543);

  // try printing
  HashTable_print(ht);

  // Try modifying the value of an existing entry
  // HashTable_put(ht, "hello", 999);
  // printHashTable(ht);

  // Look up key
  printf("%d\n", HashTable_lookup(ht, "hello"));
  printf("%d\n", HashTable_lookup(ht, "index"));
  printf("%d\n", HashTable_lookup(ht, "loop"));
  printf("%d\n", HashTable_lookup(ht, "goto"));

  // Key existence check
  printf("%s\n", HashTable_containsKey(ht, "loop") ? "Yes" : "No");
  printf("%s\n", HashTable_containsKey(ht, "hello") ? "Yes" : "No");
  printf("%s\n", HashTable_containsKey(ht, "index") ? "Yes" : "No");
  printf("%s\n", HashTable_containsKey(ht, "randomthing") ? "Yes" : "No");
  // "airplane" has the same hash as "hello" and "index"
  printf("%s\n", HashTable_containsKey(ht, "airplane") ? "Yes" : "No");
  // "accommodates" has the same hash as "loop"
  printf("%s\n", HashTable_containsKey(ht, "accommodates") ? "Yes" : "No");

  // clean up
  HashTable_destroy(ht);
}

/* inserting with dictionary file */
void dictionaryTest(char *pathname) {

  srand(time(NULL));

  FILE *fp;
  if ((fp = fopen(pathname, "r")) == NULL) {
    fprintf(stderr, "Error opening dictionary file.\n");
    exit(EXIT_FAILURE);
  }
  HashTable *ht = HashTable_create();
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), fp) != NULL) {
    line[strcspn(line, "\n")] = 0;
    if (!HashTable_containsKey(ht, line)) {
      HashTable_put(ht, line, rand() % 1024);
    }
  }
  fclose(fp);
  HashTable_print(ht);
  HashTable_destroy(ht);
}

