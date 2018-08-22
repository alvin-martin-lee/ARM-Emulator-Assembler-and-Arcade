/**
 * Linked-list adapted from week 3 lecture code.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "linkedlist.h"

ListElem* List_alloc_elem(void){
  ListElem *elem = malloc(sizeof(ListElem));
  if(elem == NULL){
    perror("List_alloc_elem");
    exit(EXIT_FAILURE);
  }
  return elem;
}

void List_free_elem(ListElem *elem){
  free(elem);
}

void List_insert(List *l, ListIter iter, ListValue value){
  ListElem *new_elem = List_alloc_elem();
  new_elem->value = value;

  new_elem->next = iter;
  new_elem->prev = iter->prev;

  iter->prev->next = new_elem;
  iter->prev = new_elem;

}

ListIter List_iter_next(ListIter iter){
  return iter->next;
}
ListIter List_begin(List *l){
  return l->header->next;
}

ListIter List_end(List *l){
  return l->footer;
}

bool List_internal(ListIter iter){
  return iter->prev != NULL && iter->next != NULL;
}

ListValue List_iter_value(ListIter iter){
  assert(List_internal(iter));
  return iter->value;
}

void List_insert_front(List *l, ListValue value) {
  List_insert(l, List_begin(l), value);
}

void List_insert_back(List *l, ListValue value) {
  List_insert(l, List_end(l), value);
}


void List_destroy(List *l) {
  ListElem *elem = l->header;
  while (elem != NULL) {
    ListElem *next = elem->next;
    List_free_elem(elem);
    elem = next;
  }
}

void List_init(List *l){
  l->header = List_alloc_elem();
  l->footer = List_alloc_elem();
  l->header->prev = NULL;
  l->footer->next = NULL;
  l->header->next = l->footer;
  l->footer->prev = l->header;
}

/**
 * Removes the first occurence of a list element with the given value.
 *
 * Also returns the previous element of the deleted one, which should be
 * used when removing elements while iterating over the list in a for loop
 *
 * Example:
 * for (ListIter iter = List_begin(&list); iter != List_end(&list); iter = List_iter_next(iter)) {
 *   Item *item = (Item *) List_iter_value(iter);
 *   if (<some condition>) {
 *     iter = List_remove(&game.itemList, item); // iter points back to prev
 *   }
 * }
 *
 * List_remove() frees the memory at iter. At this stage, iter points to an invalid memory location.
 * If iter is not must be assigned to the previous node, List_iter_next(iter) would fail
 */
ListIter List_remove(List *l, ListValue value) {
  for (ListIter it = List_begin(l);
       it != List_end(l);
       it = List_iter_next(it)) {
    if (List_iter_value(it) == value) {
      ListIter prev = it->prev;
      ListIter next = it->next;
      prev->next = it->next;
      next->prev = it->prev;
      List_free_elem(it);
      return prev;
    }
  }
  return NULL;
}
