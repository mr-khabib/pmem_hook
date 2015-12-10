#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "subop_check.h"
#include "libhook_types.h"

/*************************pointers to function********************************/
//pmemobj
extern _pmemobj_create_t    _pmemobj_create_f;
extern _pmemobj_open_t      _pmemobj_open_f;
extern _pmemobj_close_t     _pmemobj_close_f;
extern _pmemobj_alloc_t     _pmemobj_alloc_f;
extern _pmemobj_realloc_t   _pmemobj_realloc_f;
extern _pmemobj_free_t      _pmemobj_free_f;
extern _pmemobj_alloc_usable_size_t _pmemobj_alloc_usable_size_f;
//pmem
extern _pmem_is_pmem_t      _pmem_is_pmem_f;
extern _pmem_persist_t      _pmem_persist_f;
extern _pmem_msync_t        _pmem_msync_f;
extern _pmem_map_t          _pmem_map_f;
extern _pmem_unmap_t        _pmem_unmap_f;

extern FILE *logFile;

void dummy_subotimal_usage_check()
{
    if(curr_call.command == _pmemobj_free_f)
    {
        check_for_free_after_alloc();
        check_for_double_free();
    }
    else if(curr_call.command == _pmemobj_realloc_f)
    {
        check_for_realloc_after_alloc();
    }

    prev_call.command = curr_call.command;
    strcpy(prev_call.params, curr_call.params);
}

int check_for_free_after_alloc()
{
    void *alloc_p;
    void *free_p;
    if(prev_call.command == _pmemobj_alloc_f)
    {
        sscanf(prev_call.params,"%*p %p %*s", &alloc_p);
        sscanf(curr_call.params, "%p", &free_p);
        if(alloc_p == free_p)
        {
            fprintf(logFile, "**Warning** detected free after alloc with the same oidp %p = %p \n", alloc_p, free_p);
            return 0;
        }
    }
    return -1;
}


int check_for_double_free()
{
    void *this_free_p;
    void *prev_free_p;
    if(prev_call.command == _pmemobj_free_f)
    {
        sscanf(prev_call.params, "%p", &this_free_p);
        sscanf(curr_call.params, "%p", &prev_free_p);
        if(this_free_p == prev_free_p)
        {
            fprintf(logFile, "**Warning** detected double free the same oidp %p = %p \n", this_free_p, prev_free_p);
            return 0;
        }
    }
    return -1;
}

int check_for_realloc_after_alloc()
{
    void *alloc_p;
    void *realloc_p;
    if(prev_call.command == _pmemobj_alloc_f)
    {
        sscanf(prev_call.params,"%*p %p %*s", &alloc_p);
        sscanf(curr_call.params, "%*p %p", &realloc_p);
        if(alloc_p == realloc_p)
        {
            fprintf(logFile, "**Warning** detected realloc after alloc with the same oidp %p = %p \n", alloc_p, realloc_p);
            return 0;
        }
    }
    return -1;
}
