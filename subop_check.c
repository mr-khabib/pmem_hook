#include <string.h>
#include <stdio.h>

#include "subop_check.h"

void check_for_free_after_alloc();
void check_for_prev_alloc();
void check_for_double_free();
void check_for_realloc_after_alloc();

extern FILE *logFile;

void dummy_subotimal_usage_check()
{
    if(0 == strcmp(curr_call.command, "pmemobj_free"))
    {
        check_for_free_after_alloc();
        check_for_double_free();
    }
    else if(0 == strcmp(curr_call.command, "pmemobj_realloc"))
    {
        check_for_realloc_after_alloc();
    }

    strcpy(prev_call.command, curr_call.command);
    strcpy(prev_call.params, curr_call.params);
}

void check_for_free_after_alloc()
{
    void *alloc_p;
    void *free_p;
    if(0 == strcmp(prev_call.command, "pmemobj_alloc"))
    {
        fprintf(logFile, "inside check_for_free_after_alloc 1 if \n");
        sscanf(prev_call.params,"%*p %p %*s", &alloc_p);
        sscanf(curr_call.params, "%p", &free_p);
        if(alloc_p == free_p)
        {
            fprintf(logFile, "inside check_for_free_after_alloc 2 if \n");
            fprintf(logFile, "**Warning** detected free after alloc with the same oidp %p = %p \n", alloc_p, free_p);
        }
    }
}


void check_for_double_free()
{
       fprintf(logFile, "inside check_for_double_free \n");
       void *this_free_p;
       void *prev_free_p;
       if(0 == strcmp(prev_call.command, "pmemobj_free"))
       {
           fprintf(logFile, "inside check_for_double_free 1 if \n");
           sscanf(prev_call.params, "%p", &this_free_p);
           sscanf(curr_call.params, "%p", &prev_free_p);
           if(this_free_p == prev_free_p)
           {
               fprintf(logFile, "inside check_for_double_free 2 if \n");
               fprintf(logFile, "**Warning** detected double free the same oidp %p = %p \n", this_free_p, prev_free_p);
           }
       }
}

void check_for_realloc_after_alloc()
{
    void *alloc_p;
    void *realloc_p;
    if(0 == strcmp(prev_call.command, "pmemobj_alloc"))
    {
        sscanf(prev_call.params,"%*p %p %*s", &alloc_p);
        sscanf(curr_call.params, "%*p %p", &realloc_p);
        if(alloc_p == realloc_p)
        {
            fprintf(logFile, "**Warning** detected realloc after alloc with the same oidp %p = %p \n", alloc_p, realloc_p);
        }
    }
}
