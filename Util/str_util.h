#ifndef _STR_UTIL_H_ 
#define _STR_UTIL_H_ 1

#define SPACE_CHAR ' '
#define LF_CHAR '\n'
#define CR_CHAR '\r'
#define ZERO_CHAR '0'

int str_isempty(const char*);
int str_length(const char*);
int str_isnumeric(const char *);
int str_isequal(const char *, const char *);
int str_repeat(char *, const char, int);
int str_zero_fill(char *, int);
int str_lpad(char *, const char, int);
int str_rpad(char *, const char, int);
int str_reverse(char *);
int str_trim(char*);
int str_rtrim(char*);
int str_ltrim(char*);
int str_trim_char(char*, const char);
int str_rtrim_char(char*, const char);
int str_ltrim_char(char*, const char);
int str_toupper(char*);
int str_tolower(char*);
int str_initcap(char*);
int str_toupper_copy(char *, const char*);
int str_tolower_copy(char *, const char*);
int str_initcap_copy(char *, const char*);
int str_find_chr(char*, const char);
int str_find_str(char*, const char*);
int str_split_chr_first(char*, const char, char *, const int, const int);
int str_split_chr_all(char*, const char, char *, const int, const int);
int str_split_str_first(char*, const char*, char *, const int, const int);
int str_split_str_all(char*, const char*, char *, const int, const int);
void str_remove_crlf(char *);
int str_copy(char *, const char *, int);
int str_sub_str(char *, char *, int, int, int);
int str_append_char(char *, const char);
int str_append(char *, const char *);
int str_is_starts(char *,const char *);
#endif
