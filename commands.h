#ifndef __COMMANDS__
#define __COMMANDS__

#include "vma.h"

// header implements input interpretation and function call
// for solving given commands

arena_t *command_alloc_arena(char *line);

void command_alloc_block(arena_t *arena, char *line);

void command_free_block(arena_t *arena, char *line);

void command_write(arena_t *arena, char *line);

void command_read(arena_t *arena, char *line);

void command_protect(arena_t *arena, char *line);

#endif
