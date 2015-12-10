/* ---------------------------------------------------------------------------
** subop_check.h
** Dummy check for subotimal usage of pmem lib
**
** Author: ilya.khabibrakhmanov@gmail.com
** -------------------------------------------------------------------------*/

#ifndef	_SUBOP_CHECK_H
#define	_SUBOP_CHECK_H	1

typedef struct {
    void *command;
    char params[255];
} pmem_call_t;

pmem_call_t prev_call, curr_call;

void init_dirty_alloc_tree();
void destroy_dirty_alloc_tree();

void dummy_subotimal_usage_check();

int check_for_free_after_alloc();
int check_for_prev_alloc();
int check_for_double_free();
int check_for_realloc_after_alloc();
int check_for_dirty_alloc();

void append_dirty_alloc_tree();
void find_n_mark_alloc_as_ok();


#endif //subop_check.h
