#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MEM_PAGE_SIZE 256

typedef struct mp_node_s {
    char *free_ptr;
    char *end;
    struct mp_node_s *next;
} mp_node_t;


typedef struct mp_pool_s {
    struct mp_node_s *first;
    struct mp_node_s *current;
    int max; // page size
} mp_pool_t;

int mp_init(mp_pool_t *m, int size) {
    if (!m) return -1;
    
    void *addr = malloc(size);
    mp_node_t *node = (mp_node_t *)addr;

    node->free_ptr = (char *)addr + sizeof(mp_node_t);
    node->end = (char *)addr + size;
    node->next = NULL;

    m->first = node;
    m->current = node;
    m->max = size;

    return 0;
}

int mp_destroy(mp_pool_t *m) {
    if (!m) return -1;
    
    while (m->first) {
        void *addr = m->first;
        mp_node_t *node = (mp_node_t *)addr;

        m->first = node->next;
        free(addr);
    }
    return 0;
}

void *mp_alloc(mp_pool_t *m, int size) {
    void *addr = m->current;
    mp_node_t *node = (mp_node_t *)addr;

    do {
        if (size <= (node->end - node->free_ptr)) { 
            char *ptr = node->free_ptr;
            node->free_ptr += size;
            return ptr;
        }
        node = node->next;
    } while (node);

#if 1
    // 分配新的内存页
    addr = malloc(m->max);
    node = (mp_node_t *)addr;

    node->free_ptr = (char *)addr + sizeof(mp_node_t);
    node->end = (char *)addr + m->max;
#else
    size = size > m->max ? size : m->max;
    addr = mp_init(&m, size);
    node = (mp_node_t *)addr;
#endif

    char *ptr = node->free_ptr;
    node->free_ptr += size;

    node->next = m->current;
    m->current = node;

    return ptr;
}

void mp_free(mp_pool_t *m, void *p) {

}


int main() {
    mp_pool_t m;

    mp_init(&m, MEM_PAGE_SIZE);


    void *p1 = mp_alloc(&m, 10);
    printf("p1 = %p\n", p1);

    void *p2 = mp_alloc(&m, 32);
    printf("p2 = %p\n", p2);

    void *p3 = mp_alloc(&m, 64);
    printf("p3 = %p\n", p3);

    void *p4 = mp_alloc(&m, 128);
    printf("p4 = %p\n", p4);

    void *p5 = mp_alloc(&m, 64);
    printf("p5 = %p\n", p5);

    void *p6 = mp_alloc(&m, 2);
    printf("p6 = %p\n", p6);

    mp_destroy(&m);

    return 0;
}