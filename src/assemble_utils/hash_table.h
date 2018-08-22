/** hash_table.h
 * 
 * This contains the prototypes for a chaining-based hash table.
 *
 */
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdint.h>

// Size of array (aka bucket size)
#define HASH_TABLE_MAX_SIZE 128

// Key-value data types
typedef char *Key;
typedef uint32_t Value;

typedef struct HashTableEntry {
  Key key;
  Value value;
  // Pointer to the next entry with the same hash (chaining)
  struct HashTableEntry *next;
} HashTableEntry;

typedef struct HashTable {
  // Each element of the array is either NULL or a HashTableEntry linked list.
  HashTableEntry *data[HASH_TABLE_MAX_SIZE];
} HashTable;


HashTable *HashTable_create(void);

void HashTable_destroy(HashTable *hashTable);

Value HashTable_lookup(HashTable *table, Key key);

void HashTable_put(HashTable *table, Key key, Value value);

int HashTable_containsKey(HashTable *table, Key key);

void HashTable_print(HashTable *hashTable);

#endif /* end SYMBOL_TABLE_H */
