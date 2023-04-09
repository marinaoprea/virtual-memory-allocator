#ifndef __DLL__
#define __DLL__

#include "vma.h"

// header implements doubly linked list operations

list_t *dll_create(unsigned int data_size);

node_t *dll_get_nth_node(list_t *list, unsigned int n);

void dll_add_nth_node(list_t *list, unsigned int n, const void *new_data);

node_t *dll_remove_nth_node(list_t *list, unsigned int n);

unsigned int dll_get_size(list_t *list);

void dll_free(list_t **pp_list);

#endif
