#ifndef __AUXILIARS__
#define __AUXILIARS__

#include "vma.h"

// header contains auxiliar functions used in command solving

int intersection(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2);

miniblock_t *create_miniblock(size_t size, size_t address);

block_t *create_block(size_t size, size_t address);

size_t count_miniblock_bytes(node_t *curr);

int get_address(node_t **curr, block_t **block, node_t **curr2,	miniblock_t
				**miniblock, int *index, int *index2, size_t address);

size_t minim(size_t a, size_t b);

int check_address_rw(arena_t *arena, node_t **curr, block_t **block,
					 node_t **curr2, miniblock_t **miniblock, uint64_t address);

uint64_t arena_used_size(const arena_t *arena);

int count_miniblocks(const arena_t *arena);

void print_permissions(uint8_t perm);

size_t get_size(node_t *curr, uint64_t address);

int check_permissions(node_t *curr, uint8_t perm, size_t size);

#endif
