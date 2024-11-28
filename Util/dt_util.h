#ifndef _DT_UTIL_HDR_
#define _DT_UTIL_HDR_ 1

#include<time.h>

#define DT_SUCCESS 0
#define DT_FAILURE -1

#define DT_TZ_NAME "IST"
#define DT_TIMEZONE "Asia/Kolkata"

#define DT_MONTHS_IN_YEAR 12
#define DT_DAYS_IN_WEEK 7
#define DT_HOURS_IN_DAY 24
#define DT_MINS_IN_HOUR 60
#define DT_SECS_IN_DAY 86400
#define DT_SECS_IN_HOUR 3600
#define DT_SECS_IN_MIN 60

#define DT_MIN_MONTH 1
#define DT_MAX_MONTH 12
#define DT_MIN_DAY 1
#define DT_MAX_DAY 31
#define DT_FEB_MAX_DAY_LEAP_YEAR 29
#define DT_FEB_MAX_DAY 28

#define DT_AM_MAX_HOUR 11
#define DT_MAX_HOUR 23
#define DT_MAX_MIN 59
#define DT_MAX_SEC 59

#define DT_MIN_HOUR 0
#define DT_MIN_MIN 0
#define DT_MIN_SEC 0

#define DT_MIN_YEAR 2000
#define DT_MAX_YEAR 2999

#define ERR_DT_INVALID_YEAR -901
#define ERR_DT_INVALID_YM -902
#define ERR_DT_INVALID_YMD -903
#define ERR_DT_INVALID_HOUR -904
#define ERR_DT_INVALID_MINUTE -905
#define ERR_DT_INVALID_SECOND -906
#define ERR_DT_INVALID_HMS -907
#define ERR_DT_INVALID_DATETIME -908
#define ERR_DT_INVALID_DATE -909
#define ERR_DT_INVALID_EPOCH -910
#define ERR_DT_GET_SYS_RTC -911

#define ERR_DT_NULL_VALUE -912
#define ERR_DT_EMPTY_STR -913
#define ERR_DT_MON_NAME -914
#define ERR_DT_DAY_NAME -915
#define ERR_DT_INVALID_INTERVAL -916
#define ERR_DT_INVALID_LIST_SIZE -917
#define ERR_DT_INVALID_DATE_RANGE -918

enum DTM_WKDY { 
       DTM_WKD_SUN = 1, 
       DTM_WKD_MON = 2, 
       DTM_WKD_TUE = 3, 
       DTM_WKD_WED = 4, 
       DTM_WKD_THU = 5, 
       DTM_WKD_FRI = 6, 
       DTM_WKD_SAT = 7
};
typedef enum DTM_WKDY E_DTM_WKDY;

enum DTM_MONTH { 
       DTM_JAN = 1, 
       DTM_FEB = 2, 
       DTM_MAR = 3, 
       DTM_APR = 4, 
       DTM_MAY = 5, 
       DTM_JUN = 6, 
       DTM_JUL = 7,
       DTM_AUG = 8,
       DTM_SEP = 9,
       DTM_OCT = 10,
       DTM_NOV = 11,
       DTM_DEC = 11
};
typedef enum DTM_MONTH E_DTM_MONTH;

enum DTM_INTRVL { 
       DTM_INTRVL_YEAR = 1,
       DTM_INTRVL_MONTH = 2,
       DTM_INTRVL_DAY = 3, 
       DTM_INTRVL_HOUR = 4,
       DTM_INTRVL_MIN = 5,
       DTM_INTRVL_SEC = 6
};
typedef enum DTM_INTRVL E_DTM_INTRVL;

typedef struct
{
    int yy;
    int mm;
    int dd;
    int doy;
    int wkday;
    int woy;
} utl_date;

typedef struct
{
    int hh;
    int mi;
    int ss;
} utl_time;

typedef struct
{
    int yy;
    int mm;
    int dd;
    int hh;
    int mi;
    int ss;
    int doy;
    int wkday;
    int woy;
} utl_datetime;

typedef struct 
{
    int yy;
    int mm;
} st_ym;

typedef struct 
{
    int yy;
    int mm;
    int dd;
} st_ymd;

int dt_get_wkday_num_ymd(int, int, int);
int dt_get_doy_num_ymd(int, int, int);
time_t dt_get_ymd_to_epoch(int, int, int);
time_t dt_get_dttm_to_epoch(int, int, int, int, int, int);
time_t dt_get_epoch_to_date_epoch(time_t);
int dt_num_days_in_y(int);
int dt_num_days_in_ym(int, int);
int dt_next_month_ym(st_ym *);
int dt_prev_month_ym(st_ym *);
int dt_next_day_ymd(st_ymd *);
int dt_prev_day_ymd(st_ymd *);
int dt_is_valid_y(int);
int dt_is_valid_ym(int, int);
int dt_is_valid_ymd(int, int, int);
int dt_is_valid_hms(int, int, int);
int dt_is_valid(utl_datetime);
int dt_is_qtr_end_m(int);
int dt_is_qtr_start_m(int);
int dt_is_half_yr_end_m(int);
int dt_is_half_yr_start_m(int);

int dt_create_datetime(utl_datetime *u_dt_tm, int yy, int mm, int dd, int hh, int mi, int ss);
int dt_create_date(utl_date *u_dt, int yy, int mm, int dd);
int dt_create_time(utl_time *u_tm, int hh, int mi, int ss);

int dt_get_12hour_dtm(utl_datetime);
int dt_get_12hour_tm(utl_time);

int dt_datetime_to_date(utl_datetime, utl_date *);
int dt_date_to_datetime(utl_date, utl_datetime *);
int dt_time_to_datetime(utl_time, utl_datetime *);

char *dt_get_mon_name(utl_datetime);
char *dt_get_mon_abbr(utl_datetime);
char *dt_get_wkday_name(utl_datetime);
char *dt_get_wkday_abbr(utl_datetime);

char *dt_get_mon_name_initcap(utl_datetime);
char *dt_get_mon_abbr_initcap(utl_datetime);
char *dt_get_wkday_name_initcap(utl_datetime);
char *dt_get_wkday_abbr_initcap(utl_datetime);

int dt_mon_name_to_num(const char *);
char *dt_mon_num_to_name(const int);
char *dt_mon_num_to_abbr(const int);
int dt_wkday_name_to_num(const char *);
char *dt_wkday_num_to_name(const int);
char *dt_wkday_num_to_abbr(const int);
char *dt_get_am_pm_hour(int);
char *dt_get_am_pm(utl_datetime);
char *dt_get_am_pm_time(utl_time);

int dt_is_leap_year_y(int);
int dt_is_leap_year(utl_datetime);
int dt_num_days_in_month(utl_datetime);
int dt_num_days_in_year(utl_datetime);
int dt_rem_days_in_year(utl_datetime);
int dt_rem_days_in_month(utl_datetime);
int dt_get_qtr_num(utl_datetime);
int dt_get_half_yr_num(utl_datetime);

int dt_tm_to_datetime(struct tm, utl_datetime *);
int dt_datetime_to_tm(utl_datetime, struct tm *);
int dt_tm_to_date(struct tm, utl_date *);
int dt_date_to_tm(utl_date, struct tm *);

int dt_epoch_to_datetime(time_t, utl_datetime *);
int dt_epoch_to_date(time_t, utl_date *);
int dt_epoch_to_time(time_t, utl_time *);

time_t dt_datetime_to_epoch(utl_datetime);
time_t dt_date_to_epoch(utl_date);

int dt_systm_to_datetime(utl_datetime *);
time_t dt_systm_to_epoch();
time_t dt_sysdt_to_epoch();

int dt_datetime_to_date(utl_datetime, utl_date *);
int dt_datetime_to_time(utl_datetime, utl_time *);

void dt_trunc_time(utl_datetime *);
int dt_fix_mon_days(utl_datetime *);

time_t dt_incr_year_epoch(time_t, int);
time_t dt_decr_year_epoch(time_t, int);
time_t dt_incr_month_epoch(time_t, int);
time_t dt_decr_month_epoch(time_t, int);
time_t dt_incr_day_epoch(time_t, int);
time_t dt_decr_day_epoch(time_t, int);
time_t dt_incr_hour_epoch(time_t, int);
time_t dt_decr_hour_epoch(time_t, int);
time_t dt_incr_min_epoch(time_t, int);
time_t dt_decr_min_epoch(time_t, int);

int dt_incr_year(utl_datetime *, int);
int dt_decr_year(utl_datetime *, int);
int dt_incr_month(utl_datetime *, int);
int dt_decr_month(utl_datetime *, int);
int dt_incr_day(utl_datetime *, int);
int dt_decr_day(utl_datetime *, int);
int dt_incr_hour(utl_datetime *, int);
int dt_decr_hour(utl_datetime *, int);
int dt_incr_min(utl_datetime *, int);
int dt_decr_min(utl_datetime *, int);
int dt_incr_sec(utl_datetime *, int);
int dt_decr_sec(utl_datetime *, int);

int dt_incr_intrvl(utl_datetime *, E_DTM_INTRVL, int);

int dt_incr_nxt_wkday(utl_datetime *, E_DTM_WKDY);
int dt_decr_prv_wkday(utl_datetime *, E_DTM_WKDY);

time_t dt_sec_frm_mn(utl_datetime);
int dt_sec_frm_mn_hms(int, int, int);
time_t dt_sec_frm_mn_time(utl_time);

int dt_diff(utl_datetime, utl_datetime);
int dt_diff_date(utl_date, utl_date);
int dt_diff_time(utl_time, utl_time);

int dt_datetime_dup(utl_datetime *, utl_datetime *);
int dt_date_dup(utl_date *, utl_date *);
int dt_time_dup(utl_time *, utl_time *);

int dt_is_month_end(utl_datetime);
int dt_is_year_end(utl_datetime);
int dt_is_year_start(utl_datetime);
int dt_is_qtr_end(utl_datetime);
int dt_is_qtr_start(utl_datetime);
int dt_is_half_yr_end(utl_datetime);
int dt_is_half_yr_start(utl_datetime);

int dt_datetime_set_year(utl_datetime *, int);
int dt_datetime_set_month(utl_datetime *, int);
int dt_datetime_set_day(utl_datetime *, int);
int dt_datetime_set_ymd(utl_datetime *, int, int, int);
int dt_datetime_set_hms(utl_datetime *, int, int, int);
int dt_datetime_set_dttm(utl_datetime *, int, int, int, int, int, int);

int dt_date_set_year(utl_date *, int);
int dt_date_set_month(utl_date *, int);
int dt_date_set_day(utl_date *, int);
int dt_date_set_ymd(utl_date *, int, int, int);

int dt_time_set_hms(utl_time *, int, int, int);

int dt_epoch_to_str(time_t, char *);
int dt_epoch_to_date_str(time_t, char *);
int dt_epoch_to_time_str(time_t, char *);
int dt_epoch_to_linux_str(time_t, char *);
int dt_epoch_to_unix_str(time_t, char *);

int dt_datetime_to_str(utl_datetime, char *);
int dt_datetime_to_date_str(utl_datetime, char *);
int dt_datetime_to_time_str(utl_datetime, char *);
int dt_datetime_to_linux_str(utl_datetime , char *);
int dt_datetime_to_unix_str(utl_datetime , char *);

int dt_date_to_str(utl_date, char *);
int dt_date_to_linux_str(utl_date, char *);
int dt_date_to_unix_str(utl_date, char *);

int dt_time_to_str(utl_time, char *);

int dt_format_systm(char *, char *, int);
int dt_format_datetime(utl_datetime, char *, char *, int);
int dt_format_epoch(time_t, char *, char *, int);

int dt_gen_dttm_lst_range(utl_datetime *, E_DTM_INTRVL, utl_datetime, utl_datetime, int);
int dt_gen_dttm_lst_count(utl_datetime *, E_DTM_INTRVL, utl_datetime, int);
#endif