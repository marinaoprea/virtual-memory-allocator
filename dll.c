#include <stdlib.h>
#include <string.h>
#include "errno.h"
#include "vma.h"

list_t *dll_create(unsigned int data_size)
{
	list_t *list = calloc(1, sizeof(list_t));
	// Die

	list->data_size = data_size;
	list->size = 0;

	return list;
}

node_t  *dll_get_nth_node(list_t *list, unsigned int n)
{
	if (!list || !list->size || !list->head)
		return NULL;
	
	n = n % list->size;
	
	node_t *curr = list->head;
	while (n) {
	   curr = curr->next;
	   n--;
	}
	return curr;
}

void dll_add_nth_node(list_t* list, unsigned int n, const void* data)
{
	if (!list)
		return;
	
	node_t *new_node = malloc(sizeof(node_t));
	DIE(new_node == NULL, "malloc failed()\n");
		
	new_node->info = malloc(list->data_size);
	DIE(new_node->info == NULL, "malloc failed()\n");
		
	memcpy(new_node->info, data, list->data_size);

	if (list->size == 0) {
		list->head = new_node;
		list->tail = new_node;
		new_node->next = NULL;
		new_node->prev = NULL;
	} else {
		node_t *curr, *prev;
			
		if (n >= (unsigned int) list->size) {
			prev = list->tail;
			curr = NULL;
		}
		else {
			curr = dll_get_nth_node(list, n);
			prev = curr->prev;
		}
				
		new_node->prev = prev;
		new_node->next = curr;
		if (prev)
			prev->next = new_node;
		if (curr)
			curr->prev = new_node;
		
		if (n == 0)
			list->head = new_node;
		if (n >= (unsigned) list->size)
			list->tail = new_node;
	}
	
	list->size++;
}

node_t *dll_remove_nth_node(list_t *list, unsigned int n)
{
	if (!list || !list->size)
		return NULL;
		
	if (n >= (unsigned int) list->size)
		n = list->size - 1;
		
	if (list->size == 1) {
		node_t *aux = list->head;
		list->head = NULL;
		list->tail = NULL;
		list->size = 0;
		return aux;
	} else {
		node_t *curr = dll_get_nth_node(list, n);
		
		if (n == 0)
			list->head = list->head->next;
		if (n == (unsigned int) list->size - 1)
			list->tail = list->tail->prev;
		
		if (curr->prev)
			curr->prev->next = curr->next;
		if (curr->next)
			curr->next->prev = curr->prev;
		
		list->size--;
		
		return curr;
	}
}

unsigned int dll_get_size(list_t* list)
{
	return list->size;
}

void dll_free(list_t** pp_list)
{
	if (*pp_list == NULL)
		return;
	
	node_t *curr = (*pp_list)->head;
	while (curr) {
		free(curr->info);
		node_t *aux = curr;
		curr = curr->next;
		free(aux);
	}
	
	free(*pp_list);
	*pp_list = NULL;
}