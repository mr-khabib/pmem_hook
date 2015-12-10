#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <unistd.h>

#include "libhook_types.h"
#include "subop_check.h"

#define HOOK_LOG_FILE "/tmp/hook.log"

FILE *logFile = NULL;

/*************************pointers to function********************************/
//pmemobj
_pmemobj_create_t    _pmemobj_create_f   = NULL;
_pmemobj_open_t      _pmemobj_open_f     = NULL;
_pmemobj_close_t     _pmemobj_close_f    = NULL;
_pmemobj_alloc_t     _pmemobj_alloc_f    = NULL;
_pmemobj_realloc_t   _pmemobj_realloc_f  = NULL;
_pmemobj_free_t      _pmemobj_free_f     = NULL;
_pmemobj_alloc_usable_size_t _pmemobj_alloc_usable_size_f = NULL;
//pmem
_pmem_is_pmem_t      _pmem_is_pmem_f = NULL;
_pmem_persist_t      _pmem_persist_f = NULL;
_pmem_msync_t        _pmem_msync_f   = NULL;
_pmem_map_t          _pmem_map_f     = NULL;
_pmem_unmap_t        _pmem_unmap_f   = NULL;

void init_functions_pointers ()
{
    _pmemobj_create_f = (_pmemobj_create_t)     10;
    _pmemobj_open_f = (_pmemobj_open_t)         20;
    _pmemobj_close_f = (_pmemobj_close_t)       30;
    _pmemobj_alloc_f = (_pmemobj_alloc_t)       40;
    _pmemobj_realloc_f = (_pmemobj_realloc_t)   50;
    _pmemobj_free_f = (_pmemobj_free_t)         60;
    _pmemobj_alloc_usable_size_f = (_pmemobj_alloc_usable_size_t)70;
    _pmem_is_pmem_f = (_pmem_is_pmem_t)         80;
    _pmem_persist_f = (_pmem_persist_t)         90;
    _pmem_msync_f = (_pmem_msync_t)             100;
    _pmem_map_f = (_pmem_map_t)                 110;
    _pmem_unmap_f = (_pmem_unmap_t)             120;
}

void reset_structures()
{
    curr_call.command = NULL;
    memset(curr_call.params, 0x00, 255);
    prev_call.command = NULL;
    memset(prev_call.params, 0x00, 255);
}

/*************************TESTS*********************************************/
void POSITIVE_check_for_free_after_alloc()
{
    reset_structures();
    prev_call.command = _pmemobj_alloc_f;
    sprintf(prev_call.params,"%p %p %zu %u %p %p", (void*)0x11, (void*)0x22, (size_t)50, 30, (void *)0x24, (void*)0x27);
    curr_call.command = _pmemobj_free_f;
    sprintf(curr_call.params,"%p", (void*)0x22);
    int result = check_for_free_after_alloc();
    if(result == 0)
        printf("POSITIVE check_for_free_after_alloc: PASS\n");
    else
        printf("POSITIVE check_for_free_after_alloc: FAIL\n");
}

void NEGATIVE_check_for_free_after_alloc()
{
    reset_structures();
    prev_call.command = _pmemobj_alloc_f;
    sprintf(prev_call.params,"%p %p %zu %u %p %p", (void*)0x11, (void*)0x24, (size_t)50, 30, (void *)0x24, (void*)0x27);
    curr_call.command = _pmemobj_free_f;
    sprintf(curr_call.params,"%p", (void*)0x22);
    int result = check_for_free_after_alloc();
    if(result == 0)
        printf("NEGATIVE check_for_free_after_alloc: FAIL\n");
    else
        printf("NEGATIVE check_for_free_after_alloc: PASS\n");
}


int main()
{
    logFile = fopen(HOOK_LOG_FILE, "a+");
    init_functions_pointers();
    reset_structures();

    POSITIVE_check_for_free_after_alloc();
    NEGATIVE_check_for_free_after_alloc();

    fsync(fileno(logFile));
    fclose(logFile);
}
