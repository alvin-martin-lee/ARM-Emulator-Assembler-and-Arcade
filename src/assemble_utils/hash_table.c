/** hash_table.c
 * 
 * This contains the implementation of a chaining-based hash table.
 * Differences from a regular hash table:
 * - key-value mappings are not modifiable once added.
 * - no delete operation (not needed for assembler)
 * 
 * Authors: Ho Long Fung, Christopher Huang
 */
#include "hash_table.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Internal function declarations
static int hashString(char *string, int m);
static HashTableEntry *searchKeyInList(Key key, HashTableEntry *headEntry);
static void insertEntryIntoList(HashTableEntry **headEntry, HashTableEntry *newEntry);
static HashTableEntry *createEntry(Key key, Value value);
static void destroyHashTableChain(HashTableEntry *headEntry);

/**
 * Create a new hash table.
 */
HashTable *HashTable_create(void) {
  HashTable *table = malloc(sizeof(HashTable));
  for (int i = 0; i < HASH_TABLE_MAX_SIZE; i++) {
    table->data[i] = NULL;
  }
  return table;
}

/**
 * Destroy the given hash table (free its memory).
 */
void HashTable_destroy(HashTable *table) {
  if (table == NULL) {
    fprintf(stderr, "Hash table parameter is NULL.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < HASH_TABLE_MAX_SIZE; i++) {
    destroyHashTableChain(table->data[i]);
  }
  free(table);
}

/**
 * Get the value mapped by the given key
 */
Value HashTable_lookup(HashTable *table, Key key) {
  if (table == NULL) {
    fprintf(stderr, "Hash table parameter is NULL.\n");
    exit(EXIT_FAILURE);
  }

  int hash = hashString(key, HASH_TABLE_MAX_SIZE);
  HashTableEntry *entry = searchKeyInList(key, table->data[hash]);
  if (entry == NULL) {
    fprintf(stderr, "Key not found in hash table.\n");
    exit(EXIT_FAILURE);
  }
  return entry->value;
}

/**
 * Add a new key-value pair.
 */
void HashTable_put(HashTable *table, Key key, Value value) {
  if (table == NULL) {
    fprintf(stderr, "Hash table parameter is NULL.\n");
    exit(EXIT_FAILURE);
  }

  int hash = hashString(key, HASH_TABLE_MAX_SIZE);
  HashTableEntry **headEntry = &table->data[hash];
  if (searchKeyInList(key, *headEntry) != NULL) {
    // Don't allow mapping an existing key to a new value
    fprintf(stderr, "Cannot overwrite existing entry with key=%s.\n", key);
    exit(EXIT_FAILURE);
  } else {
    HashTableEntry *newEntry = createEntry(key, value);
    insertEntryIntoList(headEntry, newEntry);
  }
}

/**
 * Check if the key exists in the table
 */
int HashTable_containsKey(HashTable *table, Key key) {
  if (table == NULL) {
    fprintf(stderr, "Hash table parameter is NULL.\n");
    exit(EXIT_FAILURE);
  }

  int hash = hashString(key, HASH_TABLE_MAX_SIZE);
  return searchKeyInList(key, table->data[hash]) != NULL;
}

/**
 * Print out the hash table (for debugging purposes).
 */
void HashTable_print(HashTable *hashTable) {
  if (hashTable == NULL) {
    fprintf(stderr, "Hash table pointer argument is null\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < HASH_TABLE_MAX_SIZE; i++) {
    printf("%d: ", i);
    HashTableEntry *entry = hashTable->data[i];
    while (entry != NULL) {
      printf("(%s, %d) -> ", entry->key, entry->value);
      entry = entry->next;
    }
    printf("NULL\n");
  }
}

/******** Internal Functions ********/

/**
 * Given a string, compute a hash modulo m (size of array).
 *
 * Based on https://algs4.cs.princeton.edu/34hash/
 * Extra m (modulus) value to ensure hash is within array bounds.
 */
static int hashString(char *string, int m) {
  if (string == NULL) {
    fprintf(stderr, "String parameter is NULL.\n");
    exit(EXIT_FAILURE);
  }

  int hash = 0;
  int prime = 31; // small prime recommended
  for (int i = 0; i < strlen(string); i++)
    hash = (prime * hash + (int) string[i]) % m;
  return hash;
}

/**
 * Return a pointer to the entry with the specified key, null if not found.
 */
static HashTableEntry *searchKeyInList(Key key, HashTableEntry *headEntry) {
  // note: headEntry could be null (indicating empty bucket in the array)
  HashTableEntry *entry = headEntry;
  while (entry != NULL && strcmp(entry->key, key) != 0) {
    entry = entry->next;
  }
  return entry;
}

/**
 * Insert a key-value entry into the given entry linked-list.
 *
 * Note: a pointer to a pointer to the head node is required
 */
static void insertEntryIntoList(HashTableEntry **headEntry, HashTableEntry *newEntry) {
  if (newEntry == NULL || headEntry == NULL) {
    fprintf(stderr, "One or more NULL pointer arguments provided\n");
    exit(EXIT_FAILURE);
  }
  newEntry->next = *headEntry;
  *headEntry = newEntry;
}

/**
 * Create a new hash table entry with next pointer set to NULL.
 */
static HashTableEntry *createEntry(Key key, Value value) {
  if (key == NULL) {
    fprintf(stderr, "Key string is NULL\n");
    exit(EXIT_FAILURE);
  }

  HashTableEntry *newEntry = malloc(sizeof(HashTableEntry));
  // Need to make a copy of the string
  newEntry->key = malloc(strlen(key) + 1);
  newEntry->key = strncpy(newEntry->key, key, strlen(key) + 1);
  newEntry->value = value;
  newEntry->next = NULL;
  return newEntry;
}

/**
 * Destroy an entry linked-list.
 */
static void destroyHashTableChain(HashTableEntry *headEntry) {
  HashTableEntry *current = headEntry;
  while (current != NULL) {
    HashTableEntry *next = current->next;
    // the key's memory was malloc'd
    free(current->key);
    free(current);
    current = next;
  }
}
