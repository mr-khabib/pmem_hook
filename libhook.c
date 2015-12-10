#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <unistd.h>

#include "libhook_types.h"
#include "subop_check.h"

#define HOOK_LOG_FILE "/tmp/hook.log"
#define HOOK_LOG_N_CHECK(COMMAND, PARAMS) ( {fprintf(logFile, "%s %s\n",COMMAND, PARAMS);\
                                                dummy_subotimal_usage_check();})

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

    curr_call.command = NULL;
    memset(curr_call.params, 0x00, 255);
    prev_call.command = NULL;
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
    curr_call.command = _pmemobj_create_f;
    sprintf(curr_call.params,"%s %s %zu %x", path, layout, poolsize, mode);
    HOOK_LOG_N_CHECK("pmemobj_create", curr_call.params);
    return _pmemobj_create_f(path, layout, poolsize, mode);
}

PMEMobjpool *pmemobj_open(const char *path, const char *layout)
{
    curr_call.command = _pmemobj_open_f;
    sprintf(curr_call.params,"%s %s", path, layout);
    HOOK_LOG_N_CHECK("pmemobj_open", curr_call.params);
    return _pmemobj_open_f(path, layout);
}

void pmemobj_close(PMEMobjpool *pop)
{
    curr_call.command = _pmemobj_close_f;
    sprintf(curr_call.params,"%p", pop);
    HOOK_LOG_N_CHECK("pmemobj_close", curr_call.params);
    _pmemobj_close_f(pop);
}

int pmemobj_alloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
                  unsigned int type_num, void (*constructor)(PMEMobjpool *pop,
                                                             void *ptr, void *arg), void *arg)
{
    int result = _pmemobj_alloc_f(pop, oidp, size, type_num, constructor, arg);
    curr_call.command = _pmemobj_alloc_f;
    sprintf(curr_call.params,"%p %p %zu %u %p %p", pop, oidp, size, type_num, constructor, arg);
    HOOK_LOG_N_CHECK("pmemobj_alloc", curr_call.params);
    return result;
}

int pmemobj_realloc(PMEMobjpool *pop, PMEMoid *oidp, size_t size,
                    unsigned int type_num)
{
    curr_call.command = _pmemobj_realloc_f;
    sprintf(curr_call.params,"%p %p %zu %u", pop, oidp, size, type_num);
    HOOK_LOG_N_CHECK("pmemobj_realloc", curr_call.params);
    return _pmemobj_realloc_f(pop, oidp, size, type_num);
}

void pmemobj_free(PMEMoid *oidp)
{
    curr_call.command = _pmemobj_free_f;
    sprintf(curr_call.params,"%p",oidp);
    HOOK_LOG_N_CHECK("pmemobj_free", curr_call.params);
    _pmemobj_free_f(oidp);
}

size_t pmemobj_alloc_usable_size(PMEMoid oid)
{
    curr_call.command = _pmemobj_alloc_usable_size_f;
    sprintf(curr_call.params,"%zu %zu", oid.pool_uuid_lo, oid.off);
    HOOK_LOG_N_CHECK("pmemobj_alloc_usable_size", curr_call.params);
    return _pmemobj_alloc_usable_size_f(oid);
}


int pmem_is_pmem(void *addr, size_t len)
{
    curr_call.command = _pmem_is_pmem_f;
    sprintf(curr_call.params,"%p %zu", addr, len);
    HOOK_LOG_N_CHECK("pmem_is_pmem", curr_call.params);
    return _pmem_is_pmem_f(addr, len);
}

void pmem_persist(void *addr, size_t len)
{
    curr_call.command = _pmem_persist_f;
    sprintf(curr_call.params,"%p %zu", addr, len);
    HOOK_LOG_N_CHECK("pmem_persist", curr_call.params);
    _pmem_persist_f(addr, len);
}

int pmem_msync(void *addr, size_t len)
{
    curr_call.command = _pmem_msync_f;
    sprintf(curr_call.params,"%p %zu", addr, len);
    HOOK_LOG_N_CHECK("pmem_msync", curr_call.params);
    return _pmem_msync_f(addr, len);
}

void *pmem_map(int fd)
{
    curr_call.command = _pmem_map_f;
    sprintf(curr_call.params,"%i", fd);
    HOOK_LOG_N_CHECK("pmem_map", curr_call.params);
    return _pmem_map_f(fd);
}

int pmem_unmap(void *addr, size_t len)
{
    curr_call.command = _pmem_unmap_f;
    sprintf(curr_call.params,"%p %zu", addr, len);
    HOOK_LOG_N_CHECK("pmem_unmap", curr_call.params);
    return _pmem_unmap_f(addr, len);
}
