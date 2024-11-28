#ifndef _CFG_UTIL_H_
#define _CFG_UTIL_H_ 1

#define CFG_DELIM_CHAR '='
#define CFG_CMNT_CHAR '#'

typedef struct cfg_kv 
{
  char *key;
  char *val;
  struct cfg_kv *next;
} cfg_data;

int cfg_is_loaded();
int cfg_load_file(char *cfg_fn);
int cfg_write_file(char *cfg_fn);
int cfg_log_all();
int cfg_del(char *cfg_key);

int cfg_add_upd_kv(char *cfg_key, char *cfg_val);

int cfg_get_str(char *cfg_key, char *str_val);
int cfg_get_int(char *int_val);
float cfg_get_float(char *float_val);
double cfg_get_double(char *double_val);
#endif