// Copyright Marina Oprea 313CA 2022 - 2023
Virtual Memory Allocator: Homework 1

<h3>Description</h3>
    Current C programm proposes implementation for a virtual memory allocator 
that takes user commands from input and simulates allocation, usage and
deallocation of a memory arena.

<h3>Commands</h3>
<ul>
    <li>
<b><i>ALLOC_ARENA</b></i> - arena is simulated through a doubly linked list of disjunct blocks;
    here we alloc this simulating structure
    </li>

<li>
<b><i>ALLOC_BLOCK</b></i>
    block, continuous memory zone is simulated by a doubly linked list of 
    adjacent miniblocks;<br> 
    blocks are maintained in increasing order based on start address;<br> 
    firstly, we check if the new zone is adjacent to a block to the left; we
    insert a new miniblock in this block's list; then we verify if the next 
    block is also adjacent; in this case we redo the links of first and
    last miniblock nodes so that all are added to the left block and free
    the right block's node;<br>
    in case of total disjunction a new block is formed and inserted in the
    arena block list
</li>

<li>
<b><i>FREE_BLOCK</i></b>
    special cases are freeing the first or last miniblock of a block;
    in this case, we eliminate the node from the miniblock list and free 
    the block if it is now empty;<br>
    in case we free a miniblock in the middle of a block we create a new block 
    node and make its info point to the right part of the split block
</li>

<li>
<b><i>PMAP</i></b>
    parses the arena and prints necessary information
</li>

<li>
<b><i>WRITE</i></b>
    deep copies given data to corresponding rw_buffers
</li>

<li>
<b><i>READ</i></b>
    prints stored information from corresponding rw_buffer
</li>

<li>
<b><i>MPROTECT</i></b>
    updates permissions; permissions are handled in octal form;
    thus we use bit operations for adding 1 on 0th, 1st or 2nd bit
</li>
</ul>

<h3>Comments</h3>
    Search of specific element is linear, despite the list elements
being ordered; doubly linked list structure does not support binary
search, but it optimizes introduction of new elements and redemensioning
the list.<br>
    Homework topic gives a glimpse of data structure use in real life
problems and emphasizes doubly linkd list advantages; also it imposes great
understanding of links and their meaning.
