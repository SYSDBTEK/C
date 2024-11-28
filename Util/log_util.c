#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "comm_defs.h"
#include "log_util.h"
#include "glob_varfn.h"
#include "util.h"
#include "str_util.h"
#include "dt_util.h"

static FILE *__G_LOG_F = NULL;
static FILE *__G_LOG_ERR_F = NULL;

static pthread_mutex_t __G_LOG_MUTEX = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t __G_LOG_ERR_MUTEX = PTHREAD_MUTEX_INITIALIZER;

static void __LOG_LOCK()
{
  pthread_mutex_lock(&__G_LOG_MUTEX); 
}

static void __LOG_UNLOCK()
{
  pthread_mutex_unlock(&__G_LOG_MUTEX);
}

static void __LOG_ERR_LOCK()
{
  pthread_mutex_lock(&__G_LOG_ERR_MUTEX);
}

static void __LOG_ERR_UNLOCK()
{
  pthread_mutex_unlock(&__G_LOG_ERR_MUTEX);
}

void CHECK_RENAME_LOG_FILES()
{
  DIR *d_log = NULL;
  struct dirent *dir_log = NULL;
  char *fname = NULL;
  char abs_name[100] = { 0 };
  char *fn_dt;
  char cur_dt[10] = { 0 };
  struct stat fst_buf = { 0 };
  char dname[100] = { 0 };
  char fn_split[4][100] = { 0 };
  int split_count = 0;
  char fn_new[100] = { 0 };

  dt_format_systm("%Y%m%d", cur_dt, sizeof(cur_dt));
  
  mem_zero(dname, sizeof(dname));
  strcpy(dname, APP_LOG_DIR);
  d_log = opendir(dname);
  if(d_log)
  {
    dname[strlen(dname)] = '/';
    while ((dir_log = readdir(d_log)) != NULL)
    {
 			fname = dir_log->d_name;
			if(strcmp(fname,".") == 0 || strcmp(fname,"..") == 0) continue;

			mem_zero(abs_name, sizeof(abs_name));
      strcpy(abs_name, dname);
      strcat(abs_name, fname);
      
      mem_zero(&fst_buf, sizeof(struct stat));
      lstat(abs_name, &fst_buf);
			if(S_ISREG(fst_buf.st_mode)) // regular file
			{
				if(str_isequal(abs_name, APP_LOG_FILE_NAME))
				{
          mem_zero(fn_new, sizeof(fn_new));
          strcpy(fn_new, abs_name);
          str_append(fn_new, ".");
          str_append(fn_new, cur_dt);
          if(!is_file_exists(fn_new))
          {
            __LOG_LOCK();
              CLOSE_LOG_FILE();
              rename(abs_name, fn_new);
              OPEN_LOG_FILE();
            __LOG_UNLOCK();
          }
				}
        else if(str_isequal(abs_name, APP_ERR_FILE_NAME))
				{
          mem_zero(fn_new, sizeof(fn_new));
          strcpy(fn_new, abs_name);
          str_append(fn_new, ".");
          str_append(fn_new, cur_dt);
          if(!is_file_exists(fn_new))
          {
            __LOG_ERR_LOCK();
              CLOSE_ERR_FILE();
              rename(abs_name, fn_new);
              OPEN_ERR_FILE();
            __LOG_ERR_UNLOCK();
          }
				}
				else if(str_is_starts(abs_name, APP_LOG_FILE_NAME) || str_is_starts(abs_name, APP_ERR_FILE_NAME))
				{
					memset(fn_split, 0x00, sizeof(fn_split));
					split_count = str_split_chr_all(fname, '.', (char *)fn_split, sizeof(fn_split[0]), 1);
          if(split_count > 2 && str_isnumeric(fn_split[split_count - 1]))
          {
            fn_dt = fn_split[split_count - 1];
            if(!str_isequal(cur_dt, fn_dt))
            {
              file_delete(abs_name);
            }
          }
				}
			}
    }
    closedir(d_log);
  }
}

void CLOSE_LOG_FILE()
{
  if(__G_LOG_F != NULL && __G_LOG_F != stdout)
    fclose(__G_LOG_F);
  
  __G_LOG_F = NULL;
}

void OPEN_LOG_FILE()
{
   __G_LOG_F = NULL;
    if(!str_isempty(APP_LOG_FILE_NAME) && 
        (!is_file_exists(APP_LOG_FILE_NAME) ||
        is_file_writable(APP_LOG_FILE_NAME)))
      __G_LOG_F = fopen(APP_LOG_FILE_NAME, "a");

    if(__G_LOG_F == NULL)
      __G_LOG_F = stdout;
}

void CLOSE_ERR_FILE()
{
    if(__G_LOG_ERR_F != NULL && __G_LOG_ERR_F != stderr)
      fclose(__G_LOG_ERR_F);
    
    __G_LOG_ERR_F = NULL;
}

void OPEN_ERR_FILE()
{
   __G_LOG_ERR_F = NULL;
   
    if(!str_isempty(APP_ERR_FILE_NAME) && 
      (!is_file_exists(APP_ERR_FILE_NAME) ||
        is_file_writable(APP_ERR_FILE_NAME)))
      __G_LOG_ERR_F = fopen(APP_ERR_FILE_NAME, "a");

    if(__G_LOG_ERR_F == NULL)
      __G_LOG_ERR_F = stderr;
}

static void GET_TH_NAME(char *th_name)
{
  pthread_t tid = pthread_self();
  if(G_GET_THREAD_NAME(tid, th_name) < 0)
    sprintf(th_name, "%-10ld", tid);
}

void LOG_MSG(const char *func_name, const char *format, ...)
{
    char tm_str[30] = { 0 };
    char th_name[50] = { 0 };
    va_list args;

    GET_TH_NAME(th_name);
    
    __LOG_LOCK();

    if(__G_LOG_F == NULL)
      OPEN_LOG_FILE();

    va_start(args, format);
    get_log_tm_str(tm_str);
    //fprintf(__G_LOG_F, "%s  INFO   %14ld  %-30s  ", tm_str, tid, func_name);
    fprintf(__G_LOG_F, "%s INFO  %-10s %-30s ", tm_str, th_name, func_name);
    vfprintf(__G_LOG_F, format, args);
    fflush(__G_LOG_F);
    va_end(args);
    __LOG_UNLOCK();
}

void LOG_ERR(const char *func_name, const char *format, ...)
{
    char tm_str[30] = { 0 };
    char th_name[50] = { 0 };
    va_list args;

    GET_TH_NAME(th_name);

    __LOG_ERR_LOCK();

    if(__G_LOG_ERR_F == NULL)
      OPEN_ERR_FILE();
    
    if(__G_LOG_ERR_F == stderr)
      G_LOCK_STD_ERR_MUTEX();
    
    va_start(args, format);
    get_log_tm_str(tm_str);
    //fprintf(__G_LOG_ERR_F, "%s  ERROR  %14ld  %-30s  ", tm_str, tid, func_name);
    fprintf(__G_LOG_ERR_F, "%s ERROR %-12s %-30s ", tm_str, th_name, func_name);
    vfprintf(__G_LOG_ERR_F, format, args);
    fflush(__G_LOG_ERR_F);
    va_end(args);
    
    if(__G_LOG_ERR_F == stderr)
      G_UNLOCK_STD_ERR_MUTEX();
    
    __LOG_ERR_UNLOCK();

}

