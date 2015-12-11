/* ---------------------------------------------------------------------------
** hook_utils.h
** types and pointers to functions
**
** Author: ilya.khabibrakhmanov@gmail.com
** -------------------------------------------------------------------------*/

#ifndef	_HOOK_UTILS_H
#define	_HOOK_UTILS_H	1

#include <libpmemobj.h>
#include <libpmem.h>
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

typedef struct {
    void *alloc_oidp;
    int read_cnt;
} dirty_alloc_data_t;

#endif //hook_utils.h
