// Copyright Marina Oprea 313CA 2022 - 2023
#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>

// header contains definitions of used data structures
// header contains solving functions for given commands

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

struct node_t {
	struct node_t *prev;
	struct node_t *next;
	void *info;
};

typedef struct node_t node_t;

typedef struct {
	node_t *head;
	node_t *tail;
	unsigned int data_size;
	int size;
} list_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	list_t *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *block_list;
} arena_t;

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);

void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address,  const uint64_t size,
		   const int8_t *data);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);
