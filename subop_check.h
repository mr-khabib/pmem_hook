/* ---------------------------------------------------------------------------
** subop_check.h
** Dummy check for subotimal usage of pmem lib
**
** Author: ilya.khabibrakhmanov@gmail.com
** -------------------------------------------------------------------------*/

#ifndef	_SUBOP_CHECK_H
#define	_SUBOP_CHECK_H	1

typedef struct {
    char command[255];
    char params[255];
} pmem_call_t;

pmem_call_t prev_call, curr_call;

void dummy_subotimal_usage_check();


#endif //subop_check.h
