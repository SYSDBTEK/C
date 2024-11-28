#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "cfg_util.h"
#include "str_util.h"
#include "log_util.h"

cfg_data *__CFG_ROOT=NULL, *__CFG_LAST=NULL;

int cfg_is_loaded()
{
  return (__CFG_ROOT)?1:0;
}

static void cfg_free_data(cfg_data *cfg_entry)
{
  free(cfg_entry->key);
  free(cfg_entry->val);
  free(cfg_entry);
}

static int cfg_del_all()
{
  cfg_data *cur, *nxt;
  if(__CFG_ROOT == NULL)
    return -1;

  cur = __CFG_ROOT;
  
  while(cur)
  {
    nxt = cur->next;
    cfg_free_data(cur);
    cur = nxt;
    if(cur)
      nxt = cur->next;
  }

  __CFG_ROOT = NULL;
  __CFG_LAST = NULL;

  return 0;
}

int cfg_del(char *key)
{
  cfg_data *cur, *prv;

  if(key == NULL || key[0] == '\0')
    return -1;
  
  if(__CFG_ROOT == NULL)
    return -1;

  cur = __CFG_ROOT;
  prv = NULL;

  while(cur && strcmp(cur->key,key))
  {
    prv = cur;
    cur = cur->next;
  }

  if(cur == NULL)
    return -1;

  if(cur == __CFG_ROOT) 
  {
    __CFG_ROOT =  __CFG_ROOT->next;
  
    if(__CFG_ROOT == NULL)
        __CFG_LAST = NULL;
  }
  else
  {
    prv->next = cur->next;
    if(cur == __CFG_LAST)
      __CFG_LAST = prv;
  }
  cfg_free_data(cur);

  return 0;
}

int cfg_log_all()
{
  char __FUNC_NAME[] = "cfg_print_all";
  cfg_data *cur;
  if(__CFG_ROOT == NULL)
  {
    LOG_ERR(__FUNC_NAME, "CFG Data is empty\n");
    return -1;
  }
  cur = __CFG_ROOT;
  while(cur)
  {
    LOG_MSG(__FUNC_NAME, "Key: %s, Val: %s\n", cur->key, cur->val);
    cur = cur->next;
  }
  return 0;
}

static cfg_data *cfg_get(char *key)
{
  cfg_data *cur;

  if(key == NULL || key[0] == '\0')
    return NULL;
  
  if(__CFG_ROOT == NULL)
    return NULL;
  
  cur = __CFG_ROOT;
  while (cur && strcmp(cur->key, key))
    cur = cur->next;

  return cur;
}

int cfg_upd_kv(char *key, char *val)
{
  cfg_data *cur =  NULL;
  char *dup_val = NULL;
  
  if(key == NULL || val == NULL)
    return -1;

  str_trim(key);
  str_trim(val);
  
  if(key[0] == '\0' || val[0] == '\0')
    return -1;
     
  cur = cfg_get(key);
  if(cur == NULL)
    return -1;

  dup_val = strdup(val);
  if(dup_val == NULL)
    return -1;

  free(cur->val);
  cur->val = dup_val;
  return 0;
}

int cfg_add_kv(char *key, char *val)
{
  cfg_data *cur =  NULL;
  char *dup_key = NULL, *dup_val = NULL;
  
  if(key == NULL || val == NULL)
    return -1;

  str_trim(key);
  str_trim(val);
  
  if(key[0] == '\0' || val[0] == '\0')
    return -1;
     
  cur = cfg_get(key);
  if(cur)
    return -1;

  dup_key = strdup(key);
  if(dup_key == NULL)
    return -1;

  dup_val = strdup(val);
  if(dup_val == NULL)
    return -1;

  cur = (cfg_data *)calloc(1, sizeof(cfg_data));
  if(cur == NULL)
    return -1;

  cur->key = dup_key;
  cur->val = dup_val;
  cur->next = NULL;

  if(__CFG_ROOT == NULL)
    __CFG_ROOT = cur;
    
  if(__CFG_LAST)
    __CFG_LAST->next = cur;
  
  __CFG_LAST = cur;

  return 0;
}

int cfg_add_upd_kv(char *key, char *val)
{
  int retval;
  retval = cfg_add_kv(key, val);
  if(retval < 0)
    return cfg_upd_kv(key, val);
  
  return retval;
}

int cfg_upd(cfg_data *cfg_entry)
{
  if(cfg_entry == NULL)
    return -1;
  else
    return cfg_upd_kv(cfg_entry->key, cfg_entry->val);
}

int cfg_add(cfg_data *cfg_entry)
{
  if(cfg_entry == NULL)
    return -1;
  else
    return cfg_add_kv(cfg_entry->key, cfg_entry->val);
}

int cfg_add_upd(cfg_data *cfg_entry)
{
  if(cfg_entry == NULL)
    return -1;
  else
    return cfg_add_upd_kv(cfg_entry->key, cfg_entry->val);
}

int cfg_load_file(char *f_name)
{
  char __FUNC_NAME[] = "cfg_load_file";
  FILE *fp_cfg;
  char *line_buf = NULL;
  size_t line_buf_size = 0;
  ssize_t line_size;
  char cfg_kv[2][256] = { 0 };
  int retval = 0;

  struct stat sb;

  if(str_isempty(f_name))
  {
    LOG_ERR(__FUNC_NAME, "Config File name is empty\n");
    return -1;
  }

  if(access(f_name, F_OK | R_OK))
  {
    LOG_ERR(__FUNC_NAME, "Config File %s is not accessible\n", f_name);
    return -1;
  }
  
  if(lstat(f_name, &sb))
  {
    LOG_ERR(__FUNC_NAME, "Unable to get the stat for the Config File %s\n", f_name);
      return -1;
  }

  if(sb.st_size == 0)
  {
    LOG_ERR(__FUNC_NAME, "Config File %s is empty\n", f_name);
    return -1;
  }

  fp_cfg = fopen(f_name, "r");
  if(fp_cfg == NULL)
    return -1;
  
  cfg_del_all();
  retval = 0;
  while((line_size = getline(&line_buf, &line_buf_size, fp_cfg)) >= 0)
  {
    if(line_size <= 1)
      continue;

    str_ltrim(line_buf);
    if(line_buf[0] == '\0' || line_buf[0] == CFG_CMNT_CHAR)
      continue;
    
    str_remove_crlf(line_buf);
    if(str_split_chr_first(line_buf, CFG_DELIM_CHAR, (char *)cfg_kv, sizeof(cfg_kv[0]), 1) < 0)
    {
      LOG_ERR(__FUNC_NAME, "Invalid config entry[%s]\n", line_buf);
      retval = -1;
      break;
    }

    if(str_isempty(cfg_kv[0]) || str_isempty(cfg_kv[1]))
    {
      LOG_ERR(__FUNC_NAME, "Invalid config entry[%s]\n", line_buf);
      retval = -1;
      break;
    }
    
    if(cfg_add_upd_kv(cfg_kv[0], cfg_kv[1]) < 0)
    {
      LOG_ERR(__FUNC_NAME, "Config entry add failed, [%s]\n", line_buf);
        retval = -1;
        break;
    }
  }

  free(line_buf);
  line_buf =  NULL;
  fclose(fp_cfg);
  
  return retval;
}

int cfg_write_file(char *f_name)
{
  FILE *fp_w;
  cfg_data *cfg = __CFG_ROOT;

  if(cfg == NULL)
    return -1;

  if(f_name == NULL || f_name[0] == '\0')
    return -1;

  fp_w = fopen(f_name, "w");
  if(fp_w == NULL)
    return -1;

  while(cfg)
  {
    fprintf(fp_w, "%s = %s\n", cfg->key, cfg->val);
    cfg = cfg->next;
  }
  fclose(fp_w);

  return 0;
}

int cfg_get_str(char *key, char *str)
{
  cfg_data *cfg_entry = NULL;

  if(key == NULL || key[0] == '\0')
    return -1;
  
  if(str == NULL)
    return -1;

  str[0] = '\0';  
  cfg_entry = cfg_get(key);
  if(cfg_entry == NULL)
    return -1;

  strcpy(str, cfg_entry->val);
  return 0;
}

int cfg_get_int(char *key)
{
  cfg_data *cfg_entry = NULL;

  if(key == NULL || key[0] == '\0')
    return -1;

  cfg_entry = cfg_get(key);
  if(cfg_entry == NULL)
    return -1;

  return strtol(cfg_entry->val, NULL, 10);
}

float cfg_get_float(char *key)
{
  cfg_data *cfg_entry = NULL;

  if(key == NULL || key[0] == '\0')
    return (float)-1;

  cfg_entry = cfg_get(key);
  if(cfg_entry == NULL)
    return (float)-1;

  return strtof(cfg_entry->val, NULL);
}

double cfg_get_double(char *key)
{
  cfg_data *cfg_entry = NULL;

  if(key == NULL || key[0] == '\0')
    return (double)-1;

  cfg_entry = cfg_get(key);
  if(cfg_entry == NULL)
    return (double)-1;

  return strtod(cfg_entry->val, NULL);
}