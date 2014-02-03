#include "mempool.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "util.h"

static mempool_t *global_pools[GLOBAL_MEMPOOL_SIZELIM];

mempool_t *mempool(size_t alloc_size)
{
    uint32_t log2size = 31 - __builtin_clz((uint32_t)alloc_size);
    assert(log2size < GLOBAL_MEMPOOL_SIZELIM);
    if (!global_pools[log2size]) {
        // 32 MB
        size_t pool_size = (1 << 25) / alloc_size;
        global_pools[log2size] = create_mempool(alloc_size, pool_size);
    }
    return global_pools[log2size];
}

void mempool_free_globals()
{
    for (int i = 0; i < GLOBAL_MEMPOOL_SIZELIM; ++i) {
        if (!global_pools[i])
            continue;
        free_mempool(global_pools[i]);
        global_pools[i] = NULL;
    }
}

mempool_t *create_mempool(size_t alloc_size, size_t pool_size) {
    printf("creating mempool alloc size %lu pool size %lu\n", alloc_size, pool_size);
    mempool_t *pool = malloc(sizeof(mempool_t));
    if (!pool)
        err_oom("mempool_create");
    pool->next = NULL;
    pool->alloc_size = alloc_size;
    pool->pool_size = pool_size;
    pool->mem_start = calloc(alloc_size, pool_size);
    if (!pool->mem_start)
        err_oom("mempool_create");
    pool->mem_end = pool->mem_start + alloc_size*pool_size;
    pool->alloc_ptr = pool->mem_start;
    pool->free_spots_size = 0;
    pool->free_spots_res = 128;
    pool->free_spots = calloc(sizeof(void*), pool->free_spots_res);
    if (!pool->free_spots)
        err_oom("mempool_create");
    return pool;
}

void free_mempool(mempool_t *pool)
{
    if (pool->next) {
        free_mempool(pool->next);
    }
    free(pool->mem_start);
    free(pool->free_spots);
    free(pool);
}

void *mempool_alloc(mempool_t *pool)
{
    if (pool->free_spots_size > 0) {
        void *alloc = pool->free_spots[pool->free_spots_size -1];
        pool->free_spots_size--;
        return alloc;
    } else if (pool->alloc_ptr != pool->mem_end) {
        /* N.B. since the size of the memory pool is a multiple
         * of the allocation size, if the alloc_ptr is not equal
         * to mem_end, it must be less, by at least alloc_size.
         */
        assert(pool->alloc_ptr + pool->alloc_size <= pool->mem_end);
        void *alloc = pool->alloc_ptr;
        pool->alloc_ptr += pool->alloc_size;
        return alloc;
    } else if (!pool->next) {
        /* Pool's full, try to make a new one */
        pool->next = create_mempool(pool->alloc_size, pool->pool_size);
    }
    // Try the next pool in the list
    return mempool_alloc(pool->next);
}

void mempool_free(mempool_t *pool, void *ptr) {
    /* Check if the pointer is in this pool */
    if (pool->mem_start <= ptr && ptr <= pool->mem_end) {
        if (pool->free_spots_size == pool->free_spots_res) {
            size_t new_size = 2*sizeof(void*)*pool->free_spots_res;
            void **new_arr = realloc(pool->free_spots, new_size);
            if (!new_arr)
                err_oom("mempool_free");
            pool->free_spots = new_arr;
            pool->free_spots_res *= 2;
        }
        pool->free_spots[pool->free_spots_size] = ptr;
        pool->free_spots_size++;
        return;
    } else if (pool->next) {
        return mempool_free(pool->next, ptr);
    } else {
        fprintf(stderr, "tried to mempool_free a pointer not in the memory pool!\n");
        exit(EXIT_FAILURE);
    }
}
                 

