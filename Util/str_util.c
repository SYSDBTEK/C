#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "str_util.h"

int str_isempty(const char *s)
{
  if(s == NULL || s[0]=='\0')
    return 1;
  
  return 0;
}

int str_length(const char *s)
{
  if(str_isempty(s))
    return 0;
  else
    return strlen(s);
}

int str_isnumeric(const char *s)
{
  int j = 0, length = 0;
  
  if(str_isempty(s))
    return 0;

  length = strlen(s);
  while (s[j] && isdigit(s[j])) ++j;

  return (j ==  length);
}

int str_isequal(const char *s1, const char *s2)
{
  if(s1 == NULL || s2 == NULL)
    return 0;
  
  return !strcmp(s1, s2);
}

int str_repeat(char *d, const char c, int len)
{
   if(d == NULL || len <= 0)
    return 0;

   for(int i = 0; i < len; i++) d[i] = c;
   d[len] = '\0';

   return 1;
}

int str_zero_fill(char *d, int len)
{
   if(d == NULL || len <= 0)
    return 0;

   return str_repeat(d, ZERO_CHAR, len);
}

int str_lpad(char *d, const char c, int len)
{
  int d_len = str_length(d);
  int mv_idx = 0;
  int pad_len = 0;
  int s_char;

  pad_len = len - d_len;   
  if(d == NULL || len <= d_len)
    return 0;

  mv_idx = len;
  s_char = d[0];
    
  for(int i = d_len; i >= 0; i--) d[mv_idx--] = d[i];
  
  str_repeat(d, c, pad_len);
  d[pad_len] = s_char;
  
  return 1;
}

int str_rpad(char *d, const char c, int len)
{
  int d_len = str_length(d);
  int pad_len = 0;
   
  if(d == NULL || len <= d_len)
    return 0;
  
  pad_len = len - d_len;   
  str_repeat(d + d_len, c, pad_len);
  
  return 1;
}

int str_reverse(char *s)
{
  int s_len = str_length(s);
  char *t = NULL;
  int r_idx = 0;
   
  if(str_isempty(s))
    return 0;
  
  t = calloc(s_len, sizeof(char));
  strcpy(t, s);

  for(int i=(s_len - 1); i >= 0; i--) s[r_idx++] = t[i];
  
  free(t);
 
  return 1;
}

int str_ltrim(char* s) 
{
    return str_ltrim_char(s, SPACE_CHAR);
}

int str_rtrim(char* s) 
{
    return str_rtrim_char(s, SPACE_CHAR);
}

int str_trim(char* s) 
{
    return str_trim_char(s, SPACE_CHAR);
}

int str_ltrim_char(char* s, const char c) 
{
    int j = 0, length = 0;
    
    if(c == '\0')
      return 0;

    if (str_isempty(s))
      return 0;

    while (s[j] && s[j] == c) ++j;

    if (j > 0) {
        int i = 0;
        length = strlen(s);
        while (j <= length) s[i++] = s[j++];
    }
    return 1;
}

int str_rtrim_char(char* s, const char c) 
{
    int length = 0;

    if(c == '\0')
      return 0;

    if (str_isempty(s))
        return 0;

    length = strlen(s);

    while (length > 0 && s[length - 1] == c) {
        s[--length] = '\0';
    }
    return 1;
}

int str_trim_char(char* s, const char c) 
{
    int retval;
    retval = str_ltrim_char(s,c);
    if(retval) 
      retval = str_rtrim_char(s,c);

    return retval;
}

int str_toupper(char* s) 
{
    int i;
    
    if (str_isempty(s))
      return 0;

    for (i = 0; s[i] != '\0'; ++i)
        s[i] = toupper(s[i]);
    
    return 1;
}

int str_toupper_copy(char *d, const char* s) 
{
    int i;
    
    if(d == NULL)
      return 0;

    if (str_isempty(s))
      return 0;

    for (i = 0; s[i] != '\0'; ++i)
        d[i] = toupper(s[i]);
    
    return 1;
}

int str_tolower(char* s) 
{
    int i;

    if (str_isempty(s))
      return 0;

    for (i = 0; s[i] != '\0'; ++i)
        s[i] = tolower(s[i]);
    
    return 1;
}

int str_tolower_copy(char *d, const char* s) 
{
    int i;
    
    if(d == NULL)
      return 0;

    if (str_isempty(s))
      return 0;

    for (i = 0; s[i] != '\0'; ++i)
        d[i] = tolower(s[i]);
    
    return 1;
}

int str_initcap(char* s)
{
    int i;

    if (str_isempty(s))
      return 0;

    s[0] = toupper(s[0]);
    for (i = 1; s[i] != '\0'; ++i)
        s[i] = tolower(s[i]);
    
    return 1;
}

int str_initcap_copy(char *d, const char* s) 
{
    int i;

    if(d == NULL)
      return 0;

    if (str_isempty(s))
      return 0;

    d[0] = toupper(s[0]);
    for (i = 1; s[i] != '\0'; ++i)
        d[i] = tolower(s[i]);
    
    return 1;
}

int str_find_chr(char* s, const char c) 
{
    if (str_isempty(s))
      return -1;

    char* loc = strchr(s, c);
    if (loc == NULL)
        return -1;
    
    return (loc - s);
}

int str_find_str(char* s, const char* sub) 
{
    int s_len, sub_len;

    if(str_isempty(s) || str_isempty(sub))
      return -1;

    s_len = strlen(s);
    sub_len = strlen(sub);

    if(sub_len >= s_len)
       return -1;

    char* loc = strstr(s, sub);
    if (loc == NULL)
        return -1;

    return (loc - s);
}

int str_split_chr_first(char* s, const char c, char *split_lst, int split_nchars, int do_trim) 
{
    int count = 0;
    char *split_loc = NULL;
    char *dst = NULL;
    char *src_str =  NULL;
    int split_max = split_nchars - 1;
    int nc = 0;
    int s_len = 0;
    int split_flag = 0;

    if(c == '\0')
      return 0;

    if (str_isempty(s) || strlen(s) == 1)
       return 0;

    src_str = s;
    while(*src_str && *src_str == c)
    {
      split_flag = 1;
      src_str++;
    }
    
    s_len = strlen(src_str);
    if(s_len <= 1)
      return 0;

    split_loc = strchr(src_str, c);
    if(split_loc == NULL && split_flag == 0)
      return count;
    
    if(split_loc)
    {    
      split_flag = 1;
      nc = split_loc - s;
      if(nc > split_max)
        return -1;

      dst = (char *)(split_lst + (count * split_nchars));
      strncpy(dst, s, nc);
      dst[nc] = '\0';
      if(do_trim)
        str_trim(dst);
      count++;

      src_str = split_loc + 1;

      while(*src_str && *src_str == c)
        src_str++;
    }

    if(split_flag == 0)
      return count;

    nc = strlen(src_str);
    if(nc == 0)
      return count;
      
    if(nc > split_max)
      return -1;

    dst = (char *)(split_lst + (count * split_nchars));
    strcpy(dst, src_str);
    dst[nc] = '\0';

    if(do_trim)
      str_trim(dst);

    count++;

    return count;
}    

int str_split_chr_all(char* s, const char c, char *split_lst, int split_nchars, int do_trim) 
{
    int count = 0;
    char *split_loc = NULL; 
    char *src_str =  NULL;
    char *dst = NULL;
    int split_max = split_nchars - 1;
    int nc = 0;
    int s_len = 0;
    int split_flag = 0;

    if(c == '\0')
      return 0;

    if (str_isempty(s) || strlen(s) == 1)
       return 0;

    src_str = s;
    while(*src_str && *src_str == c)
    {
      split_flag = 1;
      src_str++;
    }

    s_len = strlen(src_str);
    if(s_len == 0)
      return count;

    split_loc = strchr(s, c);
    if(split_loc == NULL && split_flag == 0)
      return count;
    
    while(split_loc)
    {
      split_flag = 1;
      nc = split_loc - src_str;
      if(nc > split_max)
         return -1;
      
      dst = (char *)(split_lst + (count * split_nchars));
      strncpy(dst, src_str, nc);
      dst[nc] = '\0';
      if(do_trim)
        str_trim(dst);
      count++;
      
      src_str = split_loc + 1;
      while(*src_str && *src_str == c)
        src_str++;

  	  s_len = strlen(src_str);
      if(s_len == 0)
        break;

      split_loc = strchr(src_str, c);
    }
    
    if(split_flag == 0 || s_len == 0)
      return count;
      
    if(s_len > split_max)
      return -1;
     
    dst = (char *)(split_lst + (count * split_nchars));
    strcpy(dst, src_str);
    dst[s_len] = '\0';

    if(do_trim)
      str_trim(dst);
    
    count++;

    return count;
}    

int str_split_str_first(char* s, const char* sub, char *split_lst, int split_nchars, int do_trim) 
{
    int count = 0, s_len, sub_len;
    char *split_loc = NULL;
    char *dst = NULL;
    char *src_str =  NULL;
    int split_max = split_nchars - 1;
    int nc = 0;
    int split_flag = 0;

    if (str_isempty(s) || str_isempty(sub) || strlen(s) == 1)
       return 0;

    s_len = strlen(s);
    sub_len = strlen(sub);

    if(sub_len >= s_len)
       return count;
       
    src_str = s;
    while(*src_str && strlen(src_str) >= sub_len && !strncmp(src_str, sub, sub_len))
    {
	    split_flag = 1;
      src_str += sub_len;
    }
    
    s_len = strlen(src_str);
    if(sub_len >= s_len)
      return count;
      
    split_loc = strstr(src_str, sub);
    if(split_loc == NULL && split_flag == 0 )
      return count;

    if(split_loc)
    {    
      split_flag = 1;
      nc = split_loc - src_str;
      if(nc > split_max)
        return -1;

      dst = (char *)(split_lst + (count * split_nchars));
      strncpy(dst, src_str, nc);
      dst[nc] = '\0';

      if(do_trim)
        str_trim(dst);
      
      count++;

      src_str = split_loc + sub_len;
      while(*src_str && strlen(src_str) >= sub_len && !strncmp(src_str, sub, sub_len))
        src_str += sub_len;
    }
	  
    if(split_flag == 0)
      return count;
    
    nc = strlen(src_str);
    if (nc == 0)
      return count;
      
    if(nc > split_max)
      return -1;

    dst = (char *)(split_lst + (count * split_nchars));
    strcpy(dst, src_str);
    dst[nc] = '\0';
    
    if(do_trim)
      str_trim(dst);
    
    count++;

    return count;
}

int str_split_str_all(char* s, const char* sub, char *split_lst, int split_nchars, int do_trim) 
{
    int count = 0, s_len, sub_len;
    char *split_loc = NULL;
    char *src_str =  NULL;
    int split_max = split_nchars - 1;
    char *dst = NULL;
    int nc = 0;
    int split_flag = 0;

    if (str_isempty(s) || str_isempty(sub) || strlen(s) == 1)
       return 0;

    s_len = strlen(s);
    sub_len = strlen(sub);

    if(sub_len >= s_len)
       return count;
      
    src_str = s;
    while(*src_str && !strncmp(src_str, sub, sub_len))
    {
	    split_flag = 1;
      src_str += sub_len;
    }
	 
	  s_len = strlen(src_str);
    if(sub_len >= s_len)
      return count;
    
    split_loc = strstr(src_str, sub);
    if(split_loc == NULL && split_flag == 0)
      return count;
    
    while(split_loc)
    {
      split_flag = 1;
      nc = split_loc - src_str;
      if(nc > split_max)
        return -1;

      dst = (char *)(split_lst + (count * split_nchars));
      strncpy(dst, src_str, nc);
      dst[nc] = '\0';
      if(do_trim)
        str_trim(dst);
      count++;

      src_str = split_loc + sub_len;
      while(*src_str && strlen(src_str) >= sub_len && !strncmp(src_str, sub, sub_len))
		    src_str += sub_len;

	    s_len = strlen(src_str);
      if(sub_len >= s_len)
        break;
       
      split_loc = strstr(src_str, sub);
    }

    if(split_flag == 0 || s_len == 0 || sub_len > s_len)
      return count;
      
    if(sub_len == s_len && !strncmp(src_str, sub, sub_len))
      return count;

    if(s_len > split_max)
      return -1;
     
    dst = (char *)(split_lst + (count * split_nchars));
    strcpy(dst, src_str);
    dst[s_len] = '\0';

    if(do_trim)
      str_trim(dst);

    count++;

    return count;
}    

void str_remove_crlf(char *s)
{
   int length = 0;
   if (str_isempty(s))
     return;

   length = strlen(s);

   if(s[length - 1] == LF_CHAR)
      s[--length] = '\0';
    
   if(length == 0)
     return;
     
   if(s[length - 1] == CR_CHAR)
      s[--length] = '\0';
}

int str_copy(char *dest, const char *src, int dsize)
{
  int src_len, cp_len;

  if(dsize <= 0)
    return 0;
  
  if (dest == NULL || str_isempty(src))
    return 0;
  
  src_len = strlen(src);
  cp_len = (src_len>(dsize-1))?(dsize-1):src_len;
  
  memset(dest, 0x00, dsize);
  strncpy(dest, src, cp_len);
  return cp_len;
}

int str_sub_str(char *dest, char *src, int dsize, int spos, int len)
{
  int src_len, sub_len;
  char *st_ptr;

  if(len <= 0)
    return 0;

  if(dsize <= 0)
    return 0;

  if (dest == NULL || str_isempty(src))
    return 0;
  
  src_len = strlen(src);
  if(spos >= (src_len - 1)) 
    return 0;

  st_ptr = src + spos;
  sub_len = strlen(st_ptr);
  if(sub_len == 0 || len > sub_len)
    return 0;
  
  sub_len = len;

  if(sub_len > (dsize - 1))
    return 0;
  
  memset(dest, 0x00, dsize);
  strncpy(dest, st_ptr, sub_len);
  return sub_len;
}

int str_append_char(char *dest, const char ch)
{
  int d_len = 0;
    
  d_len = strlen(dest);
  dest[d_len] = ch;
  *(dest + d_len + 1) = '\0';
  return 0;
}

int str_append(char *dest, const char *src)
{
  int d_len = 0;
  int s_len = 0;

  if(str_isempty(src))
    return 0;
  
  s_len = strlen(src);
  d_len = strlen(dest);
 
  strcpy((dest + d_len), src);
  *(dest + d_len + s_len) = '\0';
  return 1;
}

int str_is_starts(char *str,const char *st_str)
{
  int s_len = 0, st_len = 0;
  int cmp_len = 0;
  
  if(str_isempty(str) || str_isempty(st_str))
    return 0;
  
  s_len = strlen(str);
  st_len = strlen(st_str);
  if(st_len > s_len)
    return 0;
  
  if(s_len == st_len)
    cmp_len = s_len;
  else
    cmp_len = st_len;

  // for(int i = 0; i < cmp_len; i++)
  // {
  //   if(str[i] != st_str[i])
  //   {
  //     retval = 0;
  //     break;
  //   }
  // }
  return !(strncmp(str, st_str, cmp_len));
}