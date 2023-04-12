// Copyright Marina Oprea 313CA 2022 - 2023
#include <stdlib.h>

#include "auxiliars.h"
#include "vma.h"
#include "dll.h"

// function returns 1 whether [x1, y1] intersects [x2, y2] segment and
// 0 otherwise; segments considered on real numbers positive semiaxis
int intersection(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2)
{
	if (x1 >= x2 && x1 <= y2)
		return 1;
	if (y1 >= x2 && y1 <= y2)
		return 1;
	if (x2 >= x1 && x2 <= y1)
		return 1;
	if (y2 >= x1 && y2 <= y1)
		return 1;
	return 0;
}

// function allocates memory for a new miniblock, initializes its fields
// to default values and returns pointer to the structure
miniblock_t *create_miniblock(size_t size, size_t address)
{
	miniblock_t *mini = calloc(1, sizeof(miniblock_t));
	DIE(!mini, "calloc failed()\n");

	mini->start_address = address;
	mini->size = size;
	mini->perm = 6;
	mini->rw_buffer = calloc(size, sizeof(char));

	return mini;
}

// function allocates memory for a new block, initializes its fields
// to default values and returns pointer to the structure
block_t *create_block(size_t size, size_t address)
{
	block_t *block = calloc(1, sizeof(block_t));
	DIE(!block, "calloc failed()\n");
	block->miniblock_list = dll_create(sizeof(miniblock_t));
	block->size = size;
	block->start_address = address;

	return block;
}

// function counts bytes ocuppied by a block's miniblocks starting with
// given node
size_t count_miniblock_bytes(node_t *curr)
{
	size_t ans = 0;
	while (curr) {
		miniblock_t *miniblock = (miniblock_t *)curr->info;
		ans += miniblock->size;
		curr = curr->next;
	}

	return ans;
}

// function returns 1 whether there is a miniblock that starts at given
// address
// function updates curr, block, curr2, miniblock to searched structures
// thus parameters are given by double pointer
int get_address(node_t **curr, block_t **block, node_t **curr2,
				miniblock_t **miniblock, int *index, int *index2,
				size_t address)
{
	int found = 0;
	while (*curr) {
		(*block) = (block_t *)(*curr)->info;

		*curr2 = (*block)->miniblock_list->head;
		*index2 = 0;
		while (*curr2) {
			*miniblock = (miniblock_t *)(*curr2)->info;
			if ((*miniblock)->start_address == address) {
				found = 1;
				break;
			}
			(*index2)++;
			*curr2 = (*curr2)->next;
		}

		if (found)
			break;
		(*index)++;
		(*curr) = (*curr)->next;
	}

	return found;
}

// function returns min(a, b)
size_t minim(size_t a, size_t b)
{
	return (a < b ? a : b);
}

// function checks address for read/write operations
// function updates curr, block, curr2, miniblock to searched structures
int check_address_rw(arena_t *arena, node_t **curr, block_t **block,
					 node_t **curr2, miniblock_t **miniblock, uint64_t address)
{
	int found = 0;
	*curr = arena->block_list->head;
	while (*curr) {
		*block = (block_t *)(*curr)->info;

		if ((*block)->start_address <= address &&
			(*block)->start_address + (*block)->size - 1 >= address) {
			found = 1;
			break;
		}
		(*curr) = (*curr)->next;
	}
	if (!found)
		return 0;
	*curr2  = (*block)->miniblock_list->head;
	while (*curr2) {
		*miniblock = (miniblock_t *)(*curr2)->info;
		if ((*miniblock)->start_address <= address &&
			(*miniblock)->start_address + (*miniblock)->size - 1 >= address) {
			found = 1;
			break;
		}

		(*curr2) = (*curr2)->next;
	}
	return found;
}

// function counts total memory bytes virtually occupied by arena
uint64_t arena_used_size(const arena_t *arena)
{
	uint64_t ans = 0;
	node_t *curr = arena->block_list->head;
	while (curr) {
		block_t *block = (block_t *)curr->info;
		ans += block->size;
		curr = curr->next;
	}

	return ans;
}

// function counts total number of miniblocks of arena
int count_miniblocks(const arena_t *arena)
{
	int ans = 0;

	node_t *curr = arena->block_list->head;
	while (curr) {
		block_t *block = (block_t *)curr->info;
		ans += block->miniblock_list->size;
		curr = curr->next;
	}

	return ans;
}

// function prints permissions for given octal form
void print_permissions(uint8_t perm)
{
	(perm & 4) ? printf("R") : printf("-");
	(perm & 2) ? printf("W") : printf("-");
	(perm & 1) ? printf("X") : printf("-");
}

// function counts bytes occupied from given node to end of block
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

// function checks permissions for read/write operations
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
