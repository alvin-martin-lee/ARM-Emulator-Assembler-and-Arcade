/**
 * Linked-list adapted from week 3 lecture code.
 */
#ifndef DOC_FIGHTER_LINKEDLIST_H
#define DOC_FIGHTER_LINKEDLIST_H

typedef struct ListElem * ListIter;
typedef void * ListValue;

typedef struct ListElem {
  ListValue value;
  struct ListElem *prev;
  struct ListElem *next;
} ListElem;

typedef struct List {
  ListElem *header;
  ListElem *footer;
} List;

ListElem* List_alloc_elem(void);
void List_free_elem(ListElem *elem);
void List_insert(List *l, ListIter iter, ListValue value);
ListIter List_iter_next(ListIter iter);
ListIter List_begin(List *l);
ListIter List_end(List *l);
bool List_internal(ListIter iter);
ListValue List_iter_value(ListIter iter);
void List_insert_front(List *l, ListValue value);
void List_insert_back(List *l, ListValue value);
void List_destroy(List *l) ;
void List_init(List *l);
ListIter List_remove(List *l, ListValue value);

#endif //DOC_FIGHTER_LINKEDLIST_H
