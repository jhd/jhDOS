#ifndef KMALLOC_H
#define KMALLOC_H
    
    uint32_t* initialiseKmalloc();
    void* kmalloc(uint32_t bytes);
    uint32_t kfree(void* memory);

#endif
