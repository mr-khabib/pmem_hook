#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <unistd.h>

#include <libpmemobj.h>
#include <libpmem.h>

#include "subop_check.h"

#define HOOK_LOG_FILE "/home/matt/hook.log"

/*************************types***********************************************/
//pmemobj
typedef PMEMobjpool *(*_pmemobj_create_t)(const char *path, const char *layout,
                                          size_t poolsize, mode_t mode);
typedef PMEMobjpool *(*_pmemobj_open_t)(const char *path, const char *layout);
typedef void         (*_pmemobj_close_t)(PMEMobjpool *pop);
typedef int          (*_pmemobj_alloc_t)(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
                                unsigned int type_num, void (*constructor)(PMEMobjpool *pop,
                                void *ptr, void *arg), void *arg);
typedef int          (*_pmemobj_realloc_t)(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
                                  unsigned int type_num);
typedef void         (*_pmemobj_free_t)(PMEMoid *oidp);
typedef size_t       (*_pmemobj_alloc_usable_size_t)(PMEMoid oid);

//pmem
typedef int     (*_pmem_is_pmem_t)(void *addr, size_t len);
typedef void    (*_pmem_persist_t)(void *addr, size_t len);
typedef int     (*_pmem_msync_t)(void *addr, size_t len);
typedef void   *(*_pmem_map_t)(int fd);
typedef int     (*_pmem_unmap_t)(void *addr, size_t len);

/*************************pointers to function********************************/
//pmemobj
static _pmemobj_create_t    _pmemobj_create_f   = NULL;
static _pmemobj_open_t      _pmemobj_open_f     = NULL;
static _pmemobj_close_t     _pmemobj_close_f    = NULL;
static _pmemobj_alloc_t     _pmemobj_alloc_f    = NULL;
static _pmemobj_realloc_t   _pmemobj_realloc_f  = NULL;
static _pmemobj_free_t      _pmemobj_free_f     = NULL;
static _pmemobj_alloc_usable_size_t _pmemobj_alloc_usable_size_f = NULL;
//pmem
static _pmem_is_pmem_t      _pmem_is_pmem_f = NULL;
static _pmem_persist_t      _pmem_persist_f = NULL;
static _pmem_msync_t        _pmem_msync_f   = NULL;
static _pmem_map_t          _pmem_map_f     = NULL;
static _pmem_unmap_t        _pmem_unmap_f   = NULL;


/************************global variables*************************************/
FILE *logFile = NULL;

__attribute__((constructor)) void _lib_init__ ()
{
    _pmemobj_create_f = (_pmemobj_create_t)dlsym(RTLD_NEXT, "pmemobj_create");
    _pmemobj_open_f = (_pmemobj_open_t)dlsym(RTLD_NEXT, "pmemobj_open");
    _pmemobj_close_f = (_pmemobj_close_t)dlsym(RTLD_NEXT, "pmemobj_close");
    _pmemobj_alloc_f = (_pmemobj_alloc_t)dlsym(RTLD_NEXT, "pmemobj_alloc");
    _pmemobj_realloc_f = (_pmemobj_realloc_t)dlsym(RTLD_NEXT, "pmemobj_realloc");
    _pmemobj_free_f = (_pmemobj_free_t)dlsym(RTLD_NEXT, "pmemobj_free");
    _pmemobj_alloc_usable_size_f = (_pmemobj_alloc_usable_size_t)dlsym(RTLD_NEXT, "pmemobj_alloc_usable_size");

    _pmem_is_pmem_f = (_pmem_is_pmem_t)dlsym(RTLD_NEXT, "pmem_is_pmem");
    _pmem_persist_f = (_pmem_persist_t)dlsym(RTLD_NEXT, "pmem_persist");
    _pmem_msync_f = (_pmem_msync_t)dlsym(RTLD_NEXT, "pmem_msync");
    _pmem_map_f = (_pmem_map_t)dlsym(RTLD_NEXT, "pmem_map");
    _pmem_unmap_f = (_pmem_unmap_t)dlsym(RTLD_NEXT, "pmem_unmap");

    memset(curr_call.command, 0x00, 255);
    memset(curr_call.params, 0x00, 255);
    memset(prev_call.command, 0x00, 255);
    memset(prev_call.params, 0x00, 255);
    logFile = fopen(HOOK_LOG_FILE, "a+");
}

__attribute__((destructor)) void _lib_deinit__ ()
{
    fsync(fileno(logFile));
    fclose(logFile);
}


PMEMobjpool *pmemobj_create(const char *path, const char *layout,
                            size_t poolsize, mode_t mode)
{
    sprintf(curr_call.command, "%s", "pmemobj_create");
    sprintf(curr_call.params,"%s %s %zu %x", path, layout, poolsize, mode);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmemobj_create_f(path, layout, poolsize, mode);
}

PMEMobjpool *pmemobj_open(const char *path, const char *layout)
{
    sprintf(curr_call.command, "%s", "pmemobj_open");
    sprintf(curr_call.params,"%s %s", path, layout);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmemobj_open_f(path, layout);
}

void pmemobj_close(PMEMobjpool *pop)
{
    sprintf(curr_call.command, "%s", "pmemobj_close");
    sprintf(curr_call.params,"%p", pop);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    _pmemobj_close_f(pop);
}

int pmemobj_alloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
                  unsigned int type_num, void (*constructor)(PMEMobjpool *pop,
                                                             void *ptr, void *arg), void *arg)
{
    sprintf(curr_call.command, "%s", "pmemobj_alloc");
    sprintf(curr_call.params,"%p %p %zu %u %p %p", pop, oidp, size, type_num, constructor, arg);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmemobj_alloc_f(pop, oidp, size, type_num, constructor, arg);
}

int pmemobj_realloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
                    unsigned int type_num)
{
    sprintf(curr_call.command, "%s", "pmemobj_realloc");
    sprintf(curr_call.params,"%p %p %zu %u", pop, oidp, size, type_num);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmemobj_realloc_f(pop, oidp, size, type_num);
}

void pmemobj_free(PMEMoid *oidp)
{
    sprintf(curr_call.command, "%s", "pmemobj_free");
    sprintf(curr_call.params,"%p",oidp);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    _pmemobj_free_f(oidp);
}

size_t pmemobj_alloc_usable_size(PMEMoid oid)
{
    sprintf(curr_call.command, "%s", "pmemobj_alloc_usable_size");
    sprintf(curr_call.params,"%zu %zu", oid.pool_uuid_lo, oid.off);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmemobj_alloc_usable_size_f(oid);
}


int pmem_is_pmem(void *addr, size_t len)
{
    sprintf(curr_call.command, "%s", "pmem_is_pmem");
    sprintf(curr_call.params,"%p %zu", addr, len);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmem_is_pmem_f(addr, len);
}

void pmem_persist(void *addr, size_t len)
{
    sprintf(curr_call.command, "%s", "pmem_persist");
    sprintf(curr_call.params,"%p %zu", addr, len);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    _pmem_persist_f(addr, len);
}

int pmem_msync(void *addr, size_t len)
{
    sprintf(curr_call.command, "%s", "pmem_msync");
    sprintf(curr_call.params,"%p %zu", addr, len);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmem_msync_f(addr, len);
}

void *pmem_map(int fd)
{
    sprintf(curr_call.command, "%s", "pmem_map");
    sprintf(curr_call.params,"%i", fd);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmem_map_f(fd);
}

int pmem_unmap(void *addr, size_t len)
{
    sprintf(curr_call.command, "%s", "pmem_unmap");
    sprintf(curr_call.params,"%p %zu", addr, len);
    fprintf(logFile, "%s %s\n", curr_call.command, curr_call.params);
    dummy_subotimal_usage_check();
    return _pmem_unmap_f(addr, len);
}
