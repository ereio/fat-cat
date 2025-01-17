/* References
 *
 * http://en.cppreference.com/w/cpp/language/sizeof
 * http://www.cplusplus.com/reference/cstdlib/malloc/
 * http://stackoverflow.com/questions/3642010/comparing-int-with-size-t
 * http://www.gnu.org/software/libc/manual/html_node/Memory-Allocation-and-C.html
 * http://www.cplusplus.com/reference/cstring/memcpy
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "../global.h"
#include "memmy.h"

/*
 * Address Storage
 * By having an address struct, we can construct an array of searchable
 * addresses by name. We can then create array of alloc and freeable memory
 * using strings. Using double in case 64 bit machines become an issue. Should be compiled
 * in 32 anyway
 * */
struct address* ADDRS = NULL;

						/* Internal Management */
/***************************************************************************/

/* Clears address of all values*/
int _clear(struct address * addr){
	if(addr == NULL) return -1;
	if(addr->loc != NULL) free(addr->loc);

	addr->loc = NULL;
	addr->name[0] = '\0';
	addr->type = sizeof(void *);

	return 0;
}

/* Initializes an address with the specified elements*/
int internal_init(struct address * addr, const char* name, size_t type, int* loc){
	if(addr == NULL) return -1;

	addr->loc = loc;
	// strcpy(addr->name, name);
	memcpy(addr->name, name, strlen(name)+1);
	addr->type = type;

	return 0;
}

/* Returns next empty address for heap allocation */
struct address * _next_addr(){

	int i = 0;

	for(i = 0; i < MAX_HEAP_SIZE; i++){
		if(ADDRS[i].loc == NULL){
			return &ADDRS[i];
		}
	}
	return NULL;
}


/***************************************************************************/

/* Initializes the memory map to null values but creates the space that will
 * house heap information
 * */
int init_memmy(){

	int i = 0;

	if(ADDRS != NULL) return 0;

	ADDRS = (struct address *) malloc(sizeof(struct address) * MAX_HEAP_SIZE);

	if(ADDRS == NULL) return 0;

	for(i = 0; i < MAX_HEAP_SIZE; i++){
		ADDRS[i].loc = NULL;
		_clear(&ADDRS[i]);
	}

	fatcat.curDir = malloc(sizeof(struct directory));
	fatcat.curDir->cluster = malloc(sizeof(struct cluster));

	return 1;
}

/* Erases all allocated memory, very safe to use with initialization
 * testing. Careful about data loss, you will lose everything!
 * */
int purge_memmy(){

	int i = 0;
	for(i = 0; i < MAX_HEAP_SIZE; i++){
		if(_clear(&ADDRS[i]) == -1)
			return -1;
	}

	free(fatcat.curDir->cluster);
	free(fatcat.curDir);

	return 0;
}
/* fetch_heap
 * fetch data in heap (by name)
 * searches heap for address by a specific name
 * */
void* fetch_heap(const char* name){
	int i = 0;

	for(i = 0; i < MAX_HEAP_SIZE; i++){
		if(strcmp(name, ADDRS[i].name) == 0){
			return ADDRS[i].loc;
		}
	}

	return NULL;
}

/* Will free the heap location with a specific name
 * Name of reference could not be found it will return a
 * zero
 * */
int free_heap(const char* name){

	int i = 0;
	for(i = 0; i < MAX_HEAP_SIZE; i++){
		if(strcmp(name, ADDRS[i].name) == 0){
			free(ADDRS[i].loc);
			_clear(&ADDRS[i]);
			return 1;
		}
	}

	return 0;
}

/* Resizes heap based on the addr struct and size passed in
int resize_heap(struct address * addr, size_t size){
	addr->loc = realloc(addr->loc, size);
	return 0;
}
*/

void print_memmy(){

	int x = 0;

	FILE* f = fopen("log.txt", "a+");
	fprintf(f, "%s", "********* LOGGING ADDRESS ARRAY **********\n\n\n");

	for(x = 0; x < MAX_HEAP_SIZE; x++){
			if(ADDRS[x].loc == NULL){
				fprintf(f,"location: %p \n", (void *) ADDRS[x].loc);
				fprintf(f,"type: %zu \n", ADDRS[x].type);
				fprintf(f,"name: %s \n\n", ADDRS[x].name);
			}
		}
}

/* creates a simple char* (string) with a name
 * good for quick manipulation of a string
 * Based only on the size of the char* array, easy if allocated for a string
 * */
char* set_stringn(const char* name, int size){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, name, sizeof(char*), malloc(size * sizeof(char*)));

	return (char*) addr->loc;
}

/* creates a simple char* (string)
 * good for quick manipulation of a string
 * Based only on the size of the char* array, easy if allocated for a string
 * */
char* set_string(int size){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, "array", sizeof(char*), malloc(size * sizeof(char*)));

	return (char*) addr->loc;
}

/* Quickly create a simple string array
 * good for quick manipulation of arguments
 * */
char** set_array(int rows, int cols){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, "array", sizeof(char*), calloc(rows, cols * sizeof(char*)));

	return (char**) addr->loc;
}

/* Set array in Heap based on name
 * Allocates memory internally for you based on type size and default
 * */
void* set_arrayn(char* name,  size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, name, type, calloc(DEFAULT_ALLOC, type));

	return addr->loc;
}

/* Set array in heap based on name
 * Allocates memory based on a specified size passed in
 * */
void* set_arraynr(char* name, int rows, size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, name, type, calloc(rows, type));

	return addr->loc;
}

/* Set array in Heap based on name
 * Allocates memory based on a specified size passed in
 * */
void* set_arraynrc(char* name, int rows, int cols, size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, name, type, malloc(rows * cols * type));

	return addr->loc;
}

/* Set Heap Based on specific size
 * Allocates memory internally for you based on type size and default
 * */
void* set_heapt(size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, "Default", type,  malloc(DEFAULT_ALLOC * type));

	return addr->loc;
}

/* Set Heap Based on specific size
 * Allocates memory based on a specified size passed in
 * */
void* set_heapst(size_t size, size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr,"Default", type, malloc(size * type));

	return addr->loc;
}


/* Set Heap Based on specific size
 * Allocates memory based on a specified size passed in
 * */
void* set_heaprct(int rows, int cols, size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr,"Default", type, malloc(rows * cols * type));

	return addr->loc;
}

/* Set Heap Memory (name, type, size)
 * Allocates memory based on a specified size passed in
 * */
void* set_file(char* name, size_t size, size_t type){

	struct address * addr = _next_addr();

	if(addr == NULL) return NULL;

	internal_init(addr, name, type, malloc(size * type));

	return addr->loc;
}


