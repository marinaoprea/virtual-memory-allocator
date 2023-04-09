CFLAGS=-Wall -Wextra -std=c99

build:
	gcc $(CFLAGS) -g -o vma main.c commands.c vma.c dll.c auxiliars.c

run_vma:
	./vma

clean:
	rm -r vma