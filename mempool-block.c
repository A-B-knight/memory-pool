#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MEM_PAGE_SIZE 4096

typedef struct mempool_s {
    int block_size;
    int free_count;

    char *free_ptr;
    char *mem;
} mempool_t;


int mp_init(mempool_t *m, int size) {
    if (!m) return -1;
    if (size < 16) size = 16;

    m->block_size = size;
    
    m->mem = malloc(MEM_PAGE_SIZE);
    if (!m->mem) return -1;
    m->free_ptr = m->mem;
    m->free_count = MEM_PAGE_SIZE / size;

    int i = 0;
    char *ptr = m->free_ptr;
    for (int i = 0; i < m->free_count; i++) {
        *(char **)ptr = ptr + size;
        ptr += size;
    }
    *(char **)ptr = NULL;

    return 0;
}

int mp_destroy(mempool_t *m) {
    if (!m || !m->mem) return -1;
    
    free(m->mem);
    m->mem = NULL;
}

void *mp_alloc(mempool_t *m) {
    if (!m || m->free_count == 0) return NULL; 

    void *ptr = m->free_ptr;

    m->free_ptr = *(char **)ptr;
    m->free_count--;

    return ptr;
}

void mp_free(mempool_t *m, void *ptr) {
    if (!m || !ptr) return;

    *(char **)ptr = m->free_ptr;
    m->free_ptr = ptr;
    m->free_count++;   
}


int main() {
    mempool_t m;
    mp_init(&m, 32);

    void *p1 = mp_alloc(&m);
    printf("p1 = %p\n", p1);

    void *p2 = mp_alloc(&m);
    printf("p2 = %p\n", p2);

    void *p3 = mp_alloc(&m);
    printf("p3 = %p\n", p3);

    void *p4 = mp_alloc(&m);
    printf("p4 = %p\n", p4);

    mp_free(&m, p2);
    mp_free(&m, p4);

    void *p5 = mp_alloc(&m);
    printf("p5 = %p\n", p5);

    void *p6 = mp_alloc(&m);
    printf("p6 = %p\n", p6);

    mp_destroy(&m);

    

    return 0;
}