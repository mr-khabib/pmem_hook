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
GTree *dirty_alloc_tree = NULL;


void dummy_subotimal_usage_check()
{
    if(curr_call.command == _pmemobj_alloc_f)
    {
        append_dirty_alloc_tree();
    }
    else if(curr_call.command == _pmemobj_free_f)
    {
        check_for_dirty_alloc();
        check_for_free_after_alloc();
        check_for_double_free();
    }
    else if(curr_call.command ==  _pmemobj_realloc_f)
    {
        find_n_mark_alloc_as_ok();
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
        sscanf(prev_call.params, "%p", &prev_free_p);
        sscanf(curr_call.params, "%p", &this_free_p);
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

int check_for_dirty_alloc()
{
    void *this_free_p;
    sscanf(curr_call.params, "%p", &this_free_p);
    gpointer result = g_tree_lookup (dirty_alloc_tree, this_free_p);

    if(result != NULL)
    {
        fprintf(logFile, "**Warning** detected unused allocation oidp %p \n", this_free_p);
        g_tree_remove(dirty_alloc_tree, this_free_p);
        return 0;
    }
    return -1;
}

gint compare_values (gconstpointer val1, gconstpointer val2)
{
    if(val1 < val2)
        return -1;
    else if(val1 > val2)
        return 1;
    else if(val1 == val2)
        return 0;
    return 0;
}

void init_dirty_alloc_tree()
{
    dirty_alloc_tree = g_tree_new(compare_values);
}

void destroy_dirty_alloc_tree()
{
    g_tree_destroy(dirty_alloc_tree);
}

void append_dirty_alloc_tree()
{
    void *alloc_p;
    sscanf(prev_call.params,"%*p %p %*s", &alloc_p);
    g_tree_insert (dirty_alloc_tree, alloc_p, alloc_p);
}

void find_n_mark_alloc_as_ok()
{
    void *realloc_p;
    sscanf(curr_call.params, "%*p %p", &realloc_p);
    g_tree_remove(dirty_alloc_tree, realloc_p);
}
