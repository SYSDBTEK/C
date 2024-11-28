#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>

#include "debug_varfn.h"

#include "comm_defs.h"
#include "glob_varfn.h"
#include "log_util.h"
#include "util.h"
#include "str_util.h"

static int __G_DEBUG = FALSE;
static FILE *__G_DBG_F = NULL;

static pthread_mutex_t __G_DEBUG_MUTEX = PTHREAD_MUTEX_INITIALIZER;

static void __DBG_LOCK()
{
  pthread_mutex_lock(&__G_DEBUG_MUTEX);
}

static void __DBG_UNLOCK()
{
  pthread_mutex_unlock(&__G_DEBUG_MUTEX);
}


void SET_DEBUG(int dbg)
{
    __DBG_LOCK();
    __G_DEBUG = ((dbg)?TRUE:FALSE);
    
    if(__G_DEBUG && __G_DBG_F == NULL)
      OPEN_DEBUG_FILE();
    else if(!__G_DEBUG && __G_DBG_F != stderr)
      CLOSE_DEBUG_FILE();
    
    __DBG_UNLOCK();
}

// void SET_DEBUG_STREAM(FILE *dbg_f)
// {
//     __DBG_LOCK();
//     __G_DBG_F = ((dbg_f==NULL)?stderr:dbg_f);
//     __DBG_UNLOCK();
// }

int IS_DEBUG()
{
    int retval;
    
    __DBG_LOCK();
    retval = __G_DEBUG;
    __DBG_UNLOCK();
    return retval;
}

void CLOSE_DEBUG_FILE()
{
    if(__G_DBG_F != NULL && __G_DBG_F != stderr)
      fclose(__G_DBG_F);
    
    __G_DBG_F = stderr;
}

void OPEN_DEBUG_FILE()
{
    __G_DBG_F = NULL;
    
    if(__G_DEBUG)
    {
        if(!str_isempty(APP_DEBUG_FILE_NAME) && 
          (!is_file_exists(APP_DEBUG_FILE_NAME) ||
           is_file_writable(APP_DEBUG_FILE_NAME)))

          __G_DBG_F = fopen(APP_DEBUG_FILE_NAME, "w");
    }

    if(__G_DBG_F == NULL)
      __G_DBG_F = stderr;
}

void DEBUG_PRINT(const char *func_name, const char *format, ...)
{
    char tm_str[30] = { 0 };
    pthread_t tid = pthread_self();
    char th_name[50] = { 0 };
    int retval = 0;
    va_list args;

    retval = G_GET_THREAD_NAME(tid, th_name);
    if(retval < 0)
      sprintf(th_name, "%-10ld", tid);

    __DBG_LOCK();

    if(__G_DBG_F == NULL)
      OPEN_DEBUG_FILE();
  
    if(__G_DBG_F == stderr)
      G_LOCK_STD_ERR_MUTEX();

    va_start(args, format);
    get_log_tm_str(tm_str);
    //fprintf(__G_DBG_F, "%s  DEBUG  %14ld  %-30s  ", tm_str, tid, func_name);
    fprintf(__G_DBG_F, "%s  DEBUG  %-10s  %-30s  ", tm_str, th_name, func_name);
    vfprintf(__G_DBG_F, format, args);
    fflush(__G_DBG_F);
    va_end(args);

    if(__G_DBG_F == stderr)
      G_UNLOCK_STD_ERR_MUTEX();
    
    __DBG_UNLOCK();
}
