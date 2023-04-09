#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vma.h"
#include "dll.h"
#include "auxiliars.h"

#define RW 6

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = calloc(1, sizeof(arena_t));
	DIE(!arena, "calloc failed()\n");

	arena->arena_size = size;
	arena->block_list = dll_create(sizeof(block_t));
	DIE(!arena->block_list, "calloc failed()\n");

	return arena;
}

void dealloc_arena(arena_t *arena)
{
	node_t *curr = arena->block_list->head;
	while (curr) {
		block_t *block = (block_t *)curr->info;
		node_t *curr2 = block->miniblock_list->head;
		while (curr2) {
			miniblock_t *miniblock = (miniblock_t *)curr2->info;
			if (miniblock->rw_buffer)
				free(miniblock->rw_buffer);
			free(miniblock);
			node_t *aux2 = (node_t *)curr2->next;
			free(curr2);
			curr2 = aux2;
		}
		free(block->miniblock_list);
		free(block);
		node_t *aux = (node_t *)curr->next;
		free(curr);
		curr = aux;
	}

	free(arena->block_list);
	free(arena);
}

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	node_t *curr = arena->block_list->head;
	long index = 0, last_index = -1;

	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return;
	}
	if (address + size - 1 >= arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return;
	}

	while (curr) {
		block_t *block = (block_t *)curr->info;
		uint64_t x1 = block->start_address;
		uint64_t y1 = x1 + block->size - 1;
		uint64_t x2 = address;
		uint64_t y2 = x2 + size - 1;

		if (intersection(x1, y1, x2, y2)) {
			printf("This zone was already allocated.\n");
			return;
		}
		if (y1 + 1 == x2) {
			miniblock_t *mini = create_miniblock(size, address);
			dll_add_nth_node(block->miniblock_list,
							 block->miniblock_list->size, mini);
			block->size += mini->size;

			node_t *next = (node_t *)curr->next;
			if (!next) {
				free(mini);
				return;
			}

			block_t *next_block = (block_t *)next->info;
			uint64_t x3 = next_block->start_address;
			if (x3 == y2 + 1) {
				block->miniblock_list->tail->next =
				 next_block->miniblock_list->head;
				next_block->miniblock_list->head->prev =
				 block->miniblock_list->tail;

				block->miniblock_list->tail = next_block->miniblock_list->tail;
				block->miniblock_list->size += next_block->miniblock_list->size;
				block->size += next_block->size;
				next = dll_remove_nth_node(arena->block_list, index + 1);
				free(next_block->miniblock_list);
				free(next_block);
				free(next);
				free(mini);
				return;
			}
			free(mini);
			return;
		}
		if (y2 + 1 == x1) {
			miniblock_t *mini = create_miniblock(size, address);
			dll_add_nth_node(block->miniblock_list, 0, mini);
			block->size += mini->size;
			block->start_address = mini->start_address;
			free(mini);
			return;
		}
		if (y1 < x2)
			last_index = index;

		curr = curr->next;
		index++;
	}

	miniblock_t *mini = create_miniblock(size, address); // neadiacent
	block_t *block = create_block(size, address);
	dll_add_nth_node(block->miniblock_list, 0, mini);
	dll_add_nth_node(arena->block_list, last_index + 1, block);
	free(mini);
	free(block);
}

void free_block(arena_t *arena, const uint64_t address)
{
	node_t *curr = arena->block_list->head;
	node_t *curr2 = NULL;
	block_t *block = NULL;
	miniblock_t *miniblock = NULL;

	int index = 0;
	int index2 = 0;

	int found = get_address(&curr, &block, &curr2, &miniblock,
							&index, &index2, address);
	if (!found) {
		printf("Invalid address for free.\n");
		return;
	}

	if (curr2 == block->miniblock_list->tail) {
		curr2 = dll_remove_nth_node(block->miniblock_list,
									block->miniblock_list->size - 1);
		block->size -= miniblock->size;
		if (miniblock->rw_buffer)
			free(miniblock->rw_buffer);
		free(miniblock);
		free(curr2);
		if (block->miniblock_list->size == 0) {
			curr = dll_remove_nth_node(arena->block_list, index);
			free(block->miniblock_list);
			free(block);
			free(curr);
		}
		return;
	}

	if (curr2 == block->miniblock_list->head) {
		curr2 = dll_remove_nth_node(block->miniblock_list, 0);
		block->size -= miniblock->size;
		block->start_address = miniblock->start_address + miniblock->size;
		block->miniblock_list->head->prev = NULL;
		if (miniblock->rw_buffer)
			free(miniblock->rw_buffer);
		free(miniblock);
		free(curr2);
		if (block->miniblock_list->size == 0) {
			curr = dll_remove_nth_node(arena->block_list, index);
			free(block->miniblock_list);
			free(block);
			free(curr);
		}
		return;
	}

	block_t *new_block = calloc(1, sizeof(block_t));
	DIE(!new_block, "calloc failed\n");

	new_block->miniblock_list = dll_create(sizeof(miniblock_t));
	new_block->miniblock_list->head = curr2->next;
	curr2->next->prev = NULL;
	new_block->miniblock_list->tail = block->miniblock_list->tail;
	new_block->miniblock_list->size = block->miniblock_list->size -
									  (index2 + 1);

	miniblock_t *next_miniblock = (miniblock_t *)(curr2->next->info);
	new_block->start_address = next_miniblock->start_address;
	new_block->size = count_miniblock_bytes(curr2->next);

	curr2->prev->next = NULL;
	block->miniblock_list->tail = curr2->prev;
	block->miniblock_list->tail->next = NULL;
	block->miniblock_list->size = index2;
	block->size = count_miniblock_bytes(block->miniblock_list->head);

	dll_add_nth_node(arena->block_list, index + 1, new_block);
	free(new_block);

	if (miniblock->rw_buffer)
		free(miniblock->rw_buffer);
	free(miniblock);
	free(curr2);
}

size_t get_size(node_t *curr, uint64_t address)
{
	size_t ans = 0;
	miniblock_t *tmp = (miniblock_t *)curr->info;
	while (curr) {
		miniblock_t *miniblock = (miniblock_t *)curr->info;
		ans += miniblock->size;
		curr = curr->next;
	}
	ans = ans - (address - tmp->start_address);
	return ans;
}

int check_permissions(node_t *curr, uint8_t perm, size_t size)
{
	size_t checked = 0;
	while (curr) {
		miniblock_t *miniblock = (miniblock_t *)curr->info;
		if (!(miniblock->perm & perm))
			return 0;
		checked += miniblock->size;
		if (checked >= size)
			return 1;
		curr = curr->next;
	}
	return 1;
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	node_t *curr = NULL;
	block_t *block = NULL;
	node_t *curr2 = NULL;
	miniblock_t *miniblock = NULL;
	int found = check_address_rw(arena, &curr, &block, &curr2, &miniblock,
								 address);

	if (!found) {
		printf("Invalid address for read.\n");
		return;
	}

	if (!check_permissions(curr2, 4, size)) {
		printf("Invalid permissions for read.\n");
		return;
	}

	size_t available = get_size(curr2, address);
	if (size > available) {
		printf("Warning: size was bigger than the block size. ");
		printf("Reading %lu characters.\n", available);
	} else {
		available = size;
	}

	if (!miniblock->rw_buffer) {
		printf("\n");
		return;
	}

	size_t read = 0;
	size_t aux = minim(miniblock->size -
					  (address - miniblock->start_address), available);
	signed char *tmp = calloc(aux + 1, sizeof(signed char));
	DIE(!tmp, "calloc failed()\n");
	memcpy(tmp, (signed char *)miniblock->rw_buffer +
							 address - miniblock->start_address, aux);
	tmp[aux] = '\0';
	printf("%s", tmp);
	free(tmp);
	curr2 = curr2->next;
	read = aux;

	while (read < available && curr2) {
		miniblock_t *miniblock = (miniblock_t *)curr2->info;
		if (read + miniblock->size <= available) {
			char *tmp = calloc(miniblock->size + 1, sizeof(char));
			if (miniblock->rw_buffer)
				memcpy(tmp, miniblock->rw_buffer, miniblock->size);
			tmp[miniblock->size] = '\0';
			printf("%s", tmp);
			free(tmp);
			read += miniblock->size;
		} else {
			signed char *tmp = calloc(available - read + 1,
									  sizeof(signed char));
			DIE(!tmp, "calloc failed()\n");
			if (miniblock->rw_buffer)
				memcpy(tmp, miniblock->rw_buffer, available - read);
			tmp[available - read + 1] = '\0';
			printf("%s", tmp);
			free(tmp);
			read += available - read;
		}
		curr2 = curr2->next;
	}

	printf("\n");
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   const int8_t *data)
{
	node_t *curr = NULL;
	block_t *block = NULL;
	node_t *curr2 = NULL;
	miniblock_t *miniblock = NULL;
	int found = check_address_rw(arena, &curr, &block, &curr2, &miniblock,
								 address);
	if (!found) {
		printf("Invalid address for write.\n");
		return;
	}

	if (!check_permissions(curr2, 2, size)) {
		printf("Invalid permissions for write.\n");
		return;
	}

	size_t available = get_size(curr2, address);
	if (size > available) {
		printf("Warning: size was bigger than the block size. ");
		printf("Writing %lu characters.\n", available);
	} else {
		available = size;
	}

	size_t copied = 0;
	size_t aux = minim(miniblock->size - (miniblock->start_address - address),
					   available);
	miniblock->rw_buffer = calloc(miniblock->size, sizeof(char));
	DIE(!miniblock->rw_buffer, "calloc failed()\n");
	memcpy(miniblock->rw_buffer +
		  (address - miniblock->start_address), data, aux);
	copied = aux;
	curr2 = curr2->next;

	while (copied < available && curr2) {
		miniblock_t *miniblock = (miniblock_t *)curr2->info;
		if (copied + miniblock->size <= available) {
			signed char *new_info = calloc(miniblock->size,
										   sizeof(signed char));
			DIE(!new_info, "calloc failed()\n");
			memcpy(new_info, data + copied, miniblock->size);
			miniblock->rw_buffer = new_info;
			copied += miniblock->size;
		} else {
			signed char *new_info = calloc(miniblock->size,
										   sizeof(signed char));
			DIE(!new_info, "calloc failed()\n");
			memcpy(new_info, data + copied, available - copied);
			miniblock->rw_buffer = new_info;
			copied += available - copied;
		}

		curr2 = curr2->next;
	}
}

void pmap(const arena_t *arena)
{
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);

	uint64_t sum = arena_used_size(arena);
	printf("Free memory: 0x%lX bytes\n", arena->arena_size - sum);
	printf("Number of allocated blocks: %d\n", arena->block_list->size);

	int no_miniblocks = count_miniblocks(arena);
	printf("Number of allocated miniblocks: %d\n", no_miniblocks);

	if (arena->block_list->size)
		printf("\n");

	node_t *curr = arena->block_list->head;
	int index = 1;
	while (curr) {
		block_t *block = (block_t *)curr->info;
		printf("Block %d begin\n", index);
		printf("Zone: 0x%lX - 0x%lX\n", block->start_address,
			   block->start_address + block->size);

		node_t *curr2 = block->miniblock_list->head;
		int index2 = 1;
		while (curr2) {
			miniblock_t *miniblock = (miniblock_t *)curr2->info;
			printf("Miniblock %d:\t\t0x%lX\t\t-\t\t0x%lX\t\t| ",
				   index2, miniblock->start_address,
				   miniblock->start_address + miniblock->size);
			print_permissions(miniblock->perm);
			printf("\n");
			curr2 = curr2->next;
			index2++;
		}

		printf("Block %d end\n", index);
		if (curr->next)
			printf("\n");

		curr = curr->next;
		index++;
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return;
	}

	node_t *curr = arena->block_list->head;
	while (curr) {
		block_t *block = (block_t *)curr->info;
		node_t *curr2 = block->miniblock_list->head;
		while (curr2) {
			miniblock_t *miniblock = (miniblock_t *)curr2->info;
			if (miniblock->start_address == address) {
				miniblock->perm = *permission;
				return;
			}
			curr2 = curr2->next;
		}
		curr = curr->next;
	}

	printf("Invalid address for mprotect.\n");
}
