#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vma.h"
#include "commands.h"

#define LINE_SIZE 1000

int main(void)
{
	arena_t *arena = NULL;

	char *line = calloc(LINE_SIZE, sizeof(char));

	while (fgets(line, LINE_SIZE - 1, stdin)) {
		if (!strncmp(line, "ALLOC_ARENA", 11) && line[11] == ' ') {
			arena = command_alloc_arena(line);
			continue;
		}

		if (!strncmp(line, "ALLOC_BLOCK", 11) && line[11] == ' ') {
			command_alloc_block(arena, line);
			continue;
		}

		if (!strncmp(line, "FREE_BLOCK", 10) && line[10] == ' ') {
			command_free_block(arena, line);
			continue;
		}

		if (!strncmp(line, "PMAP", 4) && line[4] == '\n') {
			pmap(arena);
			continue;
		}

		if (!strncmp(line, "WRITE", 5) && line[5] == ' ') {
			command_write(arena, line);
			continue;
		}

		if (!strncmp(line, "READ", 4) && line[4] == ' ') {
			command_read(arena, line);
			continue;
		}

		if (!strncmp(line, "MPROTECT", 8) && line[8] == ' ') {
			command_protect(arena, line);
			continue;
		}

		if (!strncmp(line, "DEALLOC_ARENA", 13)) {
			dealloc_arena(arena);
			break;
		}
		
		while (strchr(line, ' ')) {
			printf("Invalid command. Please try again.\n");
			char *p = strchr(line, ' ');
			*p = '\n';
		}
		if (*line == '\n')
			continue;

		printf("Invalid command. Please try again.\n");
	}

	free(line);
	return 0;
}