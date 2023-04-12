// Copyright Marina Oprea 313CA 2022 - 2023
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "vma.h"

#define COMMAND_SIZE 1000

arena_t *command_alloc_arena(char *line)
{
	char *command = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!command, "calloc failed()\n");

	uint64_t size;
	int rc = sscanf(line, "%s %lu", command, &size);
	if (rc != 2) {
		printf("Invalid command. Please try again.\n");
		free(command);
		return NULL;
	}

	free(command);
	arena_t *arena = alloc_arena(size);
	return arena;
}

void command_alloc_block(arena_t *arena, char *line)
{
	char *command = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!command, "calloc failed()\n");

	uint64_t address, size;
	int rc = sscanf(line, "%s %lu %lu", command, &address, &size);
	if (rc != 3) {
		printf("Invalid command. Please try again.\n");
		free(command);
		return;
	}

	free(command);
	alloc_block(arena, address, size);
}

void command_free_block(arena_t *arena, char *line)
{
	char *command = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!command, "calloc failed()\n");

	uint64_t address;
	int rc = sscanf(line, "%s %lu", command, &address);
	if (rc != 2) {
		printf("Invalid command. Please try again.\n");
		free(command);
		return;
	}

	free(command);
	free_block(arena, address);
}

void command_write(arena_t *arena, char *line)
{
	char *command = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!command, "calloc failed()\n");

	size_t size;
	uint64_t address;

	int rc = sscanf(line, "%s %lu %lu", command, &address, &size);
	if (rc != 3) {
		printf("Invalid command. Please try again.\n");
		free(command);
		return;
	}

	char *p = strchr(line, ' ') + 1;
	while (!(*p >= '0' && *p <= '9'))
		p++;
	while (*p >= '0' && *p <= '9')
		p++;
	while (!(*p >= '0' && *p <= '9'))
		p++;
	while (*p >= '0' && *p <= '9')
		p++;
	p++; // for ' '

	char *new_data = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!new_data, "calloc failed()\n");

	size_t read = 0;
	while (*p != '\n') {
		new_data[read] = *p;
		p++;
		read++;
	}
	new_data[read] = *p;
	p++;
	read++;
	if (read > size) {
		uint64_t i = size;
		while (i < read) {
			if (new_data[i] != ' ' && new_data[i] != '\n') {
				uint64_t j = i;
				while (j < read && new_data[j] != ' ' && new_data[j] != '\n')
					j++;
				printf("Invalid command. Please try again.\n");
				i = j;
				continue;
			}
			i++;
		}
	}

	if (read < size) {
		while (read < size) {
			char c;
			c = fgetc(stdin);
			new_data[read] = c;
			read++;
		}
	}

	new_data = realloc(new_data, read * sizeof(signed char));

	write(arena, address, size, (signed char *)new_data);

	free(command);
	free(new_data);
	new_data = NULL;
}

void command_read(arena_t *arena, char *line)
{
	char *command = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!command, "calloc failed()\n");

	uint64_t address;
	size_t size;
	int rc = sscanf(line, "%s %lu %lu", command, &address, &size);
	if (rc != 3) {
		printf("Invalid command. Please try again.\n");
		free(command);
		return;
	}

	free(command);
	read(arena, address, size);
}

void command_protect(arena_t *arena, char *line)
{
	char *command = calloc(COMMAND_SIZE, sizeof(char));
	DIE(!command, "calloc failed()\n");

	uint64_t address;
	int rc = sscanf(line, "%s %lu", command, &address);
	if (rc != 2) {
		printf("Invalid command. Please try again.\n");
		free(command);
		return;
	}

	int8_t mask = 0;
	if (strstr(line, "PROT_READ"))
		mask |= 4;
	if (strstr(line, "PROT_WRITE"))
		mask |= 2;
	if (strstr(line, "PROT_EXEC"))
		mask |= 1;
	if (strstr(line, "PROT_NONE"))
		mask = 0;

	free(command);
	mprotect(arena, address, &mask);
}
