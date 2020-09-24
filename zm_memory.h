#ifndef ZM_MEMORY_H
#define ZM_MEMORY_H

#define MEMORY_POLL_SIZE 						1024*15    //15Kbyte 
#define BLOCK_SIZE								16      //byte
#define BLOCK_NUMBER 							MEMORY_POLL_SIZE/BLOCK_SIZE

/* *
 * memory initialize 
 * return 0 : faild.
 * return 1 : success. 
 */
int zm_memoryInit(void);
/* *
 * refer to malloc.
 */
void *zm_malloc(size_t size);
/* *
 * refer to free.
 */
void zm_free(void *_Block);


#endif
