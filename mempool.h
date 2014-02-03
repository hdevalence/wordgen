/*
 * Implementation of memory pool allocator for many small allocs
 * of a fixed size.
 */
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stddef.h>

struct mempool_t {
    struct mempool_t *next;
    void *mem_start;
    void *mem_end;
    void *alloc_ptr;
    void **free_spots;
    size_t alloc_size;
    size_t pool_size;
    size_t free_spots_size;
    size_t free_spots_res;
};
typedef struct mempool_t mempool_t;

mempool_t *create_mempool(size_t alloc_size, size_t pool_size);
void free_mempool(mempool_t *pool);

void *mempool_alloc(mempool_t *pool);
void mempool_free(mempool_t *pool, void *ptr);

#define GLOBAL_MEMPOOL_SIZELIM 129

/*
 * Get a memory pool for the given allocation size from
 * a global set of allocation pools.
 *
 * Note that alloc_size must be a power of two
 * with log2(alloc_size) < GLOBAL_MEMPOOL_SIZELIM.
 */
mempool_t *mempool(size_t alloc_size);
void mempool_free_globals();

#endif
