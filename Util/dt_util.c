#define _XOPEN_SOURCE 600
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>

#include "dt_util.h"
#include "str_util.h"

static const int ydays_tab[2] = {365, 366};

static const int day_tab[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static const int doy_tab[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
};

static const int hr12_tab[24] = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

static const int qtr_tab[12] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4};

static const int half_yr_tab[12] = {1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2};

static const char wdy_fn_tab[7][10] = {
    "SUNDAY", 
    "MONDAY", 
    "TUESDAY", 
    "WEDNESDAY", 
    "THURSDAY", 
    "FRIDAY", 
    "SATURDAY"
};

static const char wdy_fn_tab_ic[7][10] = {
    "Sunday", 
    "Monday", 
    "Tuesday", 
    "Wednesday", 
    "Thursday", 
    "Friday", 
    "Saturday"
};

static const char wdy_sn_tab[7][4] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

static const char wdy_sn_tab_ic[7][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char mn_fn_tab[12][10] = {
    "JANUARY",
    "FEBRUARY",
    "MARCH",
    "APRIL",
    "MAY",
    "JUNE",
    "JULY",
    "AUGUST",
    "SEPTEMBER",
    "OCTOBER",
    "NOVEMBER",
    "DECEMBER"
};

static const char mn_fn_tab_ic[12][10] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

static const char mn_sn_tab[12][4] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

static const char mn_sn_tab_ic[12][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char am_pm_tab[2][3]={"AM", "PM"};
static const char am_pm_tab_lc[2][3]={"am", "pm"};

int dt_is_leap_year_y(int yy)
{
  return ((yy % 400 == 0) || (yy % 100 != 0 && yy % 4 == 0));
}

int dt_is_leap_year(utl_datetime dt_tm)
{
  if(dt_is_valid(dt_tm))
    return ((dt_tm.yy % 400 == 0) || (dt_tm.yy % 100 != 0 && dt_tm.yy % 4 == 0));
  else
    return 0;
}

int dt_is_valid_y(int y)
{
    if(y < DT_MIN_YEAR || y > DT_MAX_YEAR) 
      return 0;
    
    return 1;
}

int dt_is_valid_ym(int y, int m)
{
    if(!dt_is_valid_y(y))
      return 0;

    if(m < 1 || m > 12) 
      return 0;

    return 1;
}

int dt_is_valid_ymd(int y, int m, int d)
{
    if(!dt_is_valid_ym(y, m))
      return 0;

    if((d < DT_MIN_DAY || d > DT_MAX_DAY) || (d > DT_FEB_MAX_DAY_LEAP_YEAR && m == DTM_FEB))
      return 0;

    if(m == DTM_FEB && d == DT_FEB_MAX_DAY_LEAP_YEAR && dt_is_leap_year_y(y))
      return 1;
    
    if(m == DTM_FEB && d > DT_FEB_MAX_DAY)
      return 0;

    if(d == DT_MAX_DAY && (m == DTM_APR || m == DTM_JUN || m == DTM_SEP || m == DTM_NOV))
      return 0;
    
    return 1;
}

int dt_is_valid_hms(int h, int m, int s)
{
  return ((h >= DT_MIN_HOUR && h <= DT_MAX_HOUR) && 
          (m >= DT_MIN_MIN && m <= DT_MAX_MIN) && 
          (s >= DT_MIN_SEC && s <= DT_MAX_SEC));
}

int dt_is_valid_time(utl_time u_tm)
{
  return dt_is_valid_hms(u_tm.hh, u_tm.mi, u_tm.ss);
}

int dt_is_valid(utl_datetime u_dt_tm)
{
  if(!dt_is_valid_ymd(u_dt_tm.yy, u_dt_tm.mm, u_dt_tm.dd))
    return 0;

  if(!dt_is_valid_hms(u_dt_tm.hh, u_dt_tm.mi, u_dt_tm.ss))
    return 0;
  
  return 1;
}

int dt_is_valid_date(utl_date u_dt)
{
  if(!dt_is_valid_ymd(u_dt.yy, u_dt.mm, u_dt.dd))
    return 0;

  return 1;
}

int dt_is_qtr_end_m(int m)
{
  return (m == DTM_MAR || m == DTM_JUN || m == DTM_SEP || m == DTM_DEC);
}

int dt_is_qtr_start_m(int m)
{
  return (m == DTM_JAN || m == DTM_APR || m == DTM_JUL || m == DTM_OCT);
}

int dt_is_half_yr_end_m(int m)
{
  return (m == DTM_JUN || m == DTM_DEC);
}

int dt_is_half_yr_start_m(int m)
{
  return (m == DT_MIN_DAY || m == DT_DAYS_IN_WEEK);
}

int dt_get_wkday_num_ymd(int y, int m, int d)
{
  if(!dt_is_valid_ymd(y, m, d))
    return ERR_DT_INVALID_YMD;

  return ((d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7) + 1;
}

int dt_get_doy_num_ymd(int y, int m, int d)
{
  if(!dt_is_valid_ymd(y, m, d))
    return ERR_DT_INVALID_YMD;
    
  return doy_tab[dt_is_leap_year_y(y)][m] + d;
}

time_t dt_get_dttm_to_epoch(int yy, int mm, int dd, int hh, int mi, int ss)
{
  utl_datetime u_dtm = { 0 };
  if(!dt_is_valid_ymd(yy, mm, dd))
    return ERR_DT_INVALID_YMD;
  
  if(!dt_is_valid_hms(hh, mi, ss))
    return ERR_DT_INVALID_HMS;
    
  dt_create_datetime(&u_dtm, yy, mm, dd, hh, mi, ss);
  return dt_datetime_to_epoch(u_dtm);
}

time_t dt_get_ymd_to_epoch(int y, int m, int d)
{
  return dt_get_dttm_to_epoch(y, m, d, 0, 0, 0);
}

time_t dt_get_epoch_to_date_epoch(time_t dtm_epoch)
{
  utl_date u_dt;
  dt_epoch_to_date(dtm_epoch, &u_dt);
  return dt_date_to_epoch(u_dt);
}

int dt_num_days_in_y(int y)
{
  if(!dt_is_valid_y(y))
    return ERR_DT_INVALID_YEAR;
  
  return ydays_tab[dt_is_leap_year_y(y)];
}

int dt_num_days_in_ym(int y, int m)
{
  if(!dt_is_valid_ym(y, m))
    return ERR_DT_INVALID_YM;
  
  return day_tab[dt_is_leap_year_y(y)][(m - 1)];
}

int dt_next_month_ym(st_ym *ym)
{
  if(!dt_is_valid_ym(ym->yy, ym->mm))
    return ERR_DT_INVALID_YM;
  
  ym->mm++;
  if(ym->mm > DT_MONTHS_IN_YEAR)
  {
    ym->mm = 1;
    ym->yy++;
  }
  return 0;
}

int dt_prev_month_ym(st_ym *ym)
{
  if(!dt_is_valid_ym(ym->yy, ym->mm))
    return ERR_DT_INVALID_YM;
  
  ym->mm--;
  if(ym->mm < 1)
  {
    ym->mm = DT_MONTHS_IN_YEAR;
    ym->yy--;
  }
  return 0;
}

int dt_next_day_ymd(st_ymd *ymd)
{
  st_ym ym;
  if(!dt_is_valid_ymd(ymd->yy, ymd->mm, ymd->dd))
    return ERR_DT_INVALID_YMD;
  
  ymd->dd++;
  if(ymd->dd > dt_num_days_in_ym(ymd->yy, ymd->mm))
  {
    ym.yy = ymd->yy;
    ym.mm = ymd->mm;
    dt_next_month_ym(&ym);
    ymd->yy = ym.yy;
    ymd->mm = ym.mm;
    ymd->dd = 1;
  }
  
  return 0;
}

int dt_prev_day_ymd(st_ymd *ymd)
{
  st_ym ym;
  if(!dt_is_valid_ymd(ymd->yy, ymd->mm, ymd->dd))
    return ERR_DT_INVALID_YMD;
  
  ymd->dd--;
  if(ymd->dd < 0)
  {
    ym.yy = ymd->yy;
    ym.mm = ymd->mm;
    dt_prev_month_ym(&ym);
    ymd->yy = ym.yy;
    ymd->mm = ym.mm;
    ymd->dd = dt_num_days_in_ym(ymd->yy, ymd->mm);
  }

  return 0;
}

static void __set_wkd_doy(utl_datetime *u_dt_tm)
{
   //struct tm tm = { 0 };
   if(u_dt_tm == NULL)
     return;

   if(!dt_is_valid(*u_dt_tm))
     return;

   u_dt_tm->doy = dt_get_doy_num_ymd(u_dt_tm->yy, u_dt_tm->mm, u_dt_tm->dd);
   u_dt_tm->wkday = dt_get_wkday_num_ymd(u_dt_tm->yy, u_dt_tm->mm, u_dt_tm->dd);
   u_dt_tm->woy = (u_dt_tm->doy / DT_DAYS_IN_WEEK);
}

static void __set_wkd_doy_date(utl_date *u_dt)
{
   //struct tm tm = { 0 };
   if(u_dt == NULL)
     return;

   if(!dt_is_valid_ymd(u_dt->yy, u_dt->mm, u_dt->dd))
     return;

   u_dt->doy = dt_get_doy_num_ymd(u_dt->yy, u_dt->mm, u_dt->dd);
   u_dt->wkday = dt_get_wkday_num_ymd(u_dt->yy, u_dt->mm, u_dt->dd);
   u_dt->woy = (u_dt->doy / DT_DAYS_IN_WEEK);
}

int dt_create_datetime(utl_datetime *u_dt_tm, int yy, int mm, int dd, int hh, int mi, int ss)
{
   if(u_dt_tm == NULL)
     return ERR_DT_NULL_VALUE;

   if(!dt_is_valid_ymd(yy, mm, dd))
     return ERR_DT_INVALID_YMD;

   if(!dt_is_valid_hms(hh, mi, ss))
     return ERR_DT_INVALID_HMS;
    
   memset(u_dt_tm, 0x00, sizeof(utl_datetime));
   u_dt_tm->yy = yy;
   u_dt_tm->mm = mm;
   u_dt_tm->dd = dd;
   u_dt_tm->hh = hh;
   u_dt_tm->mi = mi;
   u_dt_tm->ss = ss;

   __set_wkd_doy(u_dt_tm);
   return 0;
}

int dt_create_date(utl_date *u_dt, int yy, int mm, int dd)
{
    if(u_dt == NULL)
     return ERR_DT_NULL_VALUE;

   if(!dt_is_valid_ymd(yy, mm, dd))
     return ERR_DT_INVALID_YMD;

   u_dt->yy = yy;
   u_dt->mm = mm;
   u_dt->dd = dd;

   __set_wkd_doy_date(u_dt);

   return 0;
}

int dt_create_time(utl_time *u_tm, int hh, int mi, int ss)
{
   if(u_tm == NULL)
     return ERR_DT_NULL_VALUE;

   if(!dt_is_valid_hms(hh, mi, ss))
     return ERR_DT_INVALID_HMS;

   u_tm->hh = hh;
   u_tm->mi = mi;
   u_tm->ss = ss;
   
   return 0;
}

int dt_tm_to_datetime(struct tm tm, utl_datetime *dt_tm)
{
    if(dt_tm == NULL)
      return ERR_DT_NULL_VALUE;
     
    if(!dt_is_valid_ymd(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday))
      return ERR_DT_INVALID_YMD;

    if(!dt_is_valid_hms(tm.tm_hour, tm.tm_min, tm.tm_sec))
      return ERR_DT_INVALID_HMS;

    dt_tm->yy = tm.tm_year + 1900;
    dt_tm->mm = tm.tm_mon + 1;
    dt_tm->dd = tm.tm_mday;
    dt_tm->hh = tm.tm_hour;
    dt_tm->mi = tm.tm_min;
    dt_tm->ss = tm.tm_sec;

    __set_wkd_doy(dt_tm);
    
    return 0;
}

int dt_tm_to_date(struct tm tm, utl_date *dt)
{
  if(dt == NULL)
      return ERR_DT_NULL_VALUE;
     
    if(!dt_is_valid_ymd(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday))
       return ERR_DT_INVALID_YMD;

    dt->yy = tm.tm_year + 1900;
    dt->mm = tm.tm_mon + 1;
    dt->dd = tm.tm_mday;
  
    __set_wkd_doy_date(dt);
    
    return 0;
}

int dt_datetime_to_tm(utl_datetime dt_tm, struct tm *tm)
{
    int retval = 0;

    if(tm ==  NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(dt_tm)) 
      return ERR_DT_INVALID_DATETIME; 

    retval = dt_is_valid_hms(dt_tm.hh, dt_tm.mi, dt_tm.ss);
    if(retval < 0)
      return retval; 

    tm->tm_isdst = 0;
    tm->tm_year = dt_tm.yy - 1900;
    tm->tm_mon = dt_tm.mm - 1;
    tm->tm_mday = dt_tm.dd;
    tm->tm_hour = dt_tm.hh;
    tm->tm_min = dt_tm.mi;
    tm->tm_sec = dt_tm.ss;
    tm->tm_wday = dt_tm.wkday - 1;
    tm->tm_yday = dt_tm.doy - 1;

    return 0;
}

int dt_date_to_tm(utl_date dt, struct tm *tm)
{
    if(tm ==  NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid_date(dt))
      return ERR_DT_INVALID_DATE;

    tm->tm_isdst = 0;
    tm->tm_year = dt.yy - 1900;
    tm->tm_mon = dt.mm - 1;
    tm->tm_mday = dt.dd;
    tm->tm_hour = 0;
    tm->tm_min = 0;
    tm->tm_sec = 0;
    tm->tm_wday = dt.wkday - 1;
    tm->tm_yday = dt.doy - 1;

    return 0;
}

int dt_epoch_to_datetime(time_t epoch, utl_datetime *dt_tm)
{
   struct  tm *tm;
   if(dt_tm == NULL)
     return ERR_DT_NULL_VALUE;
   
   setenv("TZ", DT_TIMEZONE, 1);
   tm = localtime(&epoch);
   if(tm == NULL)
    return ERR_DT_GET_SYS_RTC;


   return dt_tm_to_datetime(*tm, dt_tm);
}

time_t dt_datetime_to_epoch(utl_datetime dt_tm)
{
   struct tm tm = { 0 };
   int retval = 0;
   
   retval = dt_datetime_to_tm(dt_tm, &tm); 
   if(retval < 0)
     return retval;
  
   setenv("TZ", DT_TIMEZONE, 1);
   return mktime(&tm);
}

int dt_epoch_to_date(time_t epoch, utl_date *dt)
{
   struct  tm *tm;
   if(dt == NULL)
     return ERR_DT_NULL_VALUE;
   
   setenv("TZ", DT_TIMEZONE, 1);
   tm = localtime(&epoch);
   if(tm == NULL)
    return ERR_DT_GET_SYS_RTC;
   
   return dt_tm_to_date(*tm, dt);
}

time_t dt_date_to_epoch(utl_date dt)
{
   struct tm tm = { 0 };
   int retval = 0;

   retval = dt_date_to_tm(dt, &tm);
   if(retval < 0)
     return retval;
  
   setenv("TZ", DT_TIMEZONE, 1);
   return mktime(&tm);
}

int dt_epoch_to_time(time_t tm_epoch, utl_time *u_tm)
{
  int hh = 0, mi = 0, ss = 0;
  utl_datetime u_dtm = { 0 };
  if(u_tm == NULL)
    return ERR_DT_NULL_VALUE;

  if(tm_epoch >= DT_SECS_IN_DAY)
  {
     dt_epoch_to_datetime(tm_epoch, &u_dtm);
     dt_trunc_time(&u_dtm);
     tm_epoch -= dt_datetime_to_epoch(u_dtm);
  }

  if(tm_epoch > 0)
  {
    hh = (tm_epoch / DT_SECS_IN_HOUR);
    mi = ((tm_epoch - (hh * DT_SECS_IN_HOUR)) / DT_MINS_IN_HOUR);
    ss = (tm_epoch - (hh * DT_SECS_IN_HOUR) - (mi * DT_SECS_IN_MIN));
  
    return dt_create_time(u_tm, hh, mi, ss);
  }
  else
    return ERR_DT_INVALID_EPOCH;
  
  return 0;
}

int dt_systm_to_datetime(utl_datetime *sys_dt_tm)
{
    struct timespec tm_spec;
    time_t tm_epoch;
    struct tm *tm;
    
    if(sys_dt_tm == NULL)
      return ERR_DT_NULL_VALUE;
    
    setenv("TZ", DT_TIMEZONE, 1);
    if(clock_gettime(CLOCK_REALTIME, &tm_spec) < 0)
       return ERR_DT_GET_SYS_RTC;
    
    tm_epoch = tm_spec.tv_sec;
    
    //tm_epoch = time(NULL);
    tm = localtime(&tm_epoch);
    if(tm == NULL)
      return ERR_DT_GET_SYS_RTC;

    memset(sys_dt_tm, 0x00, sizeof(utl_datetime));
    sys_dt_tm->yy = tm->tm_year + 1900;
    sys_dt_tm->mm = tm->tm_mon + 1;
    sys_dt_tm->dd = tm->tm_mday;
    sys_dt_tm->hh = tm->tm_hour;
    sys_dt_tm->mi = tm->tm_min;
    sys_dt_tm->ss = tm->tm_sec;

    __set_wkd_doy(sys_dt_tm);

    return 0;
}

time_t dt_systm_to_epoch()
{
  utl_datetime u_dtm = { 0 };
  dt_systm_to_datetime(&u_dtm);
  return dt_datetime_to_epoch(u_dtm);
}

time_t dt_sysdt_to_epoch()
{
  utl_datetime u_dtm = { 0 };
  dt_systm_to_datetime(&u_dtm);
  dt_trunc_time(&u_dtm);
  return dt_datetime_to_epoch(u_dtm);
}

int dt_datetime_dup(utl_datetime *d_dtm, utl_datetime *s_dtm)
{
  if(d_dtm == NULL || s_dtm == NULL)
    return ERR_DT_NULL_VALUE;

  if(!dt_is_valid(*s_dtm))
    return ERR_DT_INVALID_DATETIME;
  
  memcpy(d_dtm, s_dtm, sizeof(utl_datetime));
  return 0;
}

int dt_date_dup(utl_date *d_dt, utl_date *s_dt)
{
  if(d_dt == NULL || s_dt == NULL)
    return ERR_DT_NULL_VALUE;
  
  if(!dt_is_valid_date(*s_dt)) 
    return ERR_DT_INVALID_DATE;
  
  memcpy(d_dt, s_dt, sizeof(utl_date));
  return 0;
}

int dt_time_dup(utl_time *d_tm, utl_time *s_tm)
{
  if(d_tm == NULL || s_tm == NULL)
    return ERR_DT_NULL_VALUE;
  
  if(!dt_is_valid_hms(s_tm->hh, s_tm->mi, s_tm->ss))
    return ERR_DT_INVALID_HMS;
  
  memcpy(d_tm, s_tm, sizeof(utl_time));
  return 0;
}

void dt_trunc_time(utl_datetime *u_dt_tm)
{
  u_dt_tm->hh = 0;
  u_dt_tm->mi = 0;
  u_dt_tm->ss = 0;
}

int dt_datetime_to_date(utl_datetime dt_tm, utl_date *dt)
{
    if(dt == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(dt_tm))
      return ERR_DT_INVALID_DATETIME;
    
    dt->yy = dt_tm.yy;
    dt->mm = dt_tm.mm;
    dt->dd = dt_tm.dd;

    __set_wkd_doy_date(dt);   

    return 0;
}

int dt_datetime_to_time(utl_datetime dt_tm, utl_time *tm)
{
    if(tm == NULL)
      return ERR_DT_NULL_VALUE;
    
    if(!dt_is_valid(dt_tm))
      return ERR_DT_INVALID_DATETIME;

    tm->hh = dt_tm.hh;
    tm->mi = dt_tm.mi;
    tm->ss = dt_tm.ss;

    return 0;
}

int dt_date_to_datetime(utl_date dt, utl_datetime *dt_tm)
{
    if(dt_tm == NULL)
      return ERR_DT_NULL_VALUE;
    
    
    if(!dt_is_valid_date(dt))
      return ERR_DT_INVALID_DATE;
    
    dt_tm->yy = dt.yy;
    dt_tm->mm = dt.mm;
    dt_tm->dd = dt.dd;
    dt_tm->hh = 0;
    dt_tm->mi = 0;
    dt_tm->ss = 0;

    __set_wkd_doy(dt_tm);

    return 0;
}

int dt_time_to_datetime(utl_time tm, utl_datetime *dt_tm)
{
    if(dt_tm == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid_hms(tm.hh, tm.mi, tm.ss))
      return ERR_DT_INVALID_HMS; 

    dt_tm->yy = DT_MIN_YEAR;
    dt_tm->mm = 1;
    dt_tm->dd = 1;
    dt_tm->hh = tm.hh;
    dt_tm->mi = tm.mi;
    dt_tm->ss = tm.ss;

    __set_wkd_doy(dt_tm);

    return 0;
}


int dt_get_12hour_dtm(utl_datetime u_dtm)
{
  return hr12_tab[u_dtm.hh];
}

int dt_get_12hour_tm(utl_time u_tm)
{
  return hr12_tab[u_tm.hh];
}

char *dt_get_mon_name(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm))
      return NULL;
    
    return (char *)mn_fn_tab[(u_dtm.mm - 1)];
}

char *dt_get_mon_abbr(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm))
      return NULL;

    return (char *)mn_sn_tab[(u_dtm.mm - 1)];
}

char *dt_get_wkday_name(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm) || (u_dtm.wkday < 1 || u_dtm.wkday > DT_DAYS_IN_WEEK))
      return NULL;
    
    return (char *)wdy_fn_tab[u_dtm.wkday - 1];
}

char *dt_get_wkday_abbr(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm)  || (u_dtm.wkday < 1 || u_dtm.wkday > DT_DAYS_IN_WEEK))
      return NULL;

    return (char *)wdy_sn_tab[u_dtm.wkday - 1];
}

char *dt_get_mon_name_initcap(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm))
      return NULL;
    
   return (char *)mn_fn_tab_ic[(u_dtm.mm - 1)];
}

char *dt_get_mon_abbr_initcap(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm))
      return NULL;

    return (char *)mn_sn_tab_ic[(u_dtm.mm - 1)];
}

char *dt_get_wkday_name_initcap(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm) || (u_dtm.wkday < 1 || u_dtm.wkday > DT_DAYS_IN_WEEK))
      return NULL;
    
    return (char *)wdy_fn_tab_ic[u_dtm.wkday - 1];
}

char *dt_get_wkday_abbr_initcap(utl_datetime u_dtm)
{
    if(!dt_is_valid(u_dtm)  || (u_dtm.wkday < 1 || u_dtm.wkday > DT_DAYS_IN_WEEK))
      return NULL;

    return (char *)wdy_sn_tab_ic[u_dtm.wkday - 1];
}

char *dt_get_am_pm_hour(int h)
{
    if(h < DT_MIN_HOUR || h > DT_MAX_HOUR)
      return NULL;

    return (char *)((h < 12)?am_pm_tab[0]:am_pm_tab[1]);
}

char *dt_get_am_pm_hour_lcase(int h)
{
    if(h < DT_MIN_HOUR || h > DT_MAX_HOUR)
      return NULL;

    return (char *)((h < 12)?am_pm_tab_lc[0]:am_pm_tab_lc[1]);
}

char *dt_get_am_pm(utl_datetime u_dtm)
{
    return dt_get_am_pm_hour(u_dtm.hh);
}

char *dt_get_am_pm_time(utl_time u_tm)
{
    return dt_get_am_pm_hour(u_tm.hh);
}

int dt_mon_name_to_num(const char *name)
{
  int sn_mode = 0;
  int name_len = 0;
  
  if(str_isempty(name))
    return ERR_DT_EMPTY_STR;
  
  name_len = strlen(name);
  
  if(name_len < 3 || name_len > 10)
    return ERR_DT_MON_NAME;

  if(name_len == 3)
    sn_mode = 1;

  for(int i=0; i < DT_MONTHS_IN_YEAR; i++)  
  {
    if(sn_mode && (!strcasecmp((char *)mn_sn_tab[i], name)))
      return i + 1;
    else if(!sn_mode && (!strcasecmp((char *)mn_fn_tab[i], name)))
      return i + 1;
  }
  
  return ERR_DT_MON_NAME;
}

char *dt_mon_num_to_name(const int mnum)
{
  if(mnum < 1 || mnum > DT_MONTHS_IN_YEAR)
    return NULL;

  return (char *)mn_fn_tab[(mnum - 1)];
}

char *dt_mon_num_to_abbr(const int mnum)
{
  if(mnum < 1 || mnum > DT_MONTHS_IN_YEAR)
    return NULL;
  
  return (char *)mn_sn_tab[(mnum - 1)];
}

int dt_wkday_name_to_num(const char *name)
{
  int sn_mode = 0;
  int name_len = 0;
  
  if(str_isempty(name))
    return ERR_DT_EMPTY_STR;
  
  name_len = strlen(name);
  
  if(name_len < 3 || name_len > 10)
    return ERR_DT_DAY_NAME;

  if(name_len == 3)
    sn_mode = 1;
  
  for(int i=0; i < DT_DAYS_IN_WEEK; i++)  
  {
    if(sn_mode && (!strcasecmp((char *)wdy_sn_tab[i], name)))
      return i + 1;
    else if(!sn_mode && (!strcasecmp((char *)wdy_fn_tab[i], name)))
      return i + 1;
  }

  return ERR_DT_DAY_NAME;
}

char *dt_wkday_num_to_name(const int wkday)
{
  if(wkday < 1 || wkday > DT_DAYS_IN_WEEK)
    return NULL;

  return (char *)wdy_fn_tab[wkday - 1];
}

char *dt_wkday_num_to_abbr(const int wkday)
{
  if(wkday < 1 || wkday > DT_DAYS_IN_WEEK)
    return NULL;

  return (char *)wdy_sn_tab[wkday - 1];
}

int dt_num_days_in_month(utl_datetime dt_tm)
{
  return day_tab[dt_is_leap_year(dt_tm)][(dt_tm.mm - 1)];
}

int dt_num_days_in_year(utl_datetime dt_tm)
{
   return dt_num_days_in_y(dt_tm.yy);
}

int dt_rem_days_in_year(utl_datetime dt_tm)
{
   return (dt_num_days_in_y(dt_tm.yy) - dt_tm.doy);
}

int dt_rem_days_in_month(utl_datetime dt_tm)
{
  return (dt_num_days_in_month(dt_tm) - dt_tm.dd);
}

int dt_get_qtr_num(utl_datetime dt_tm)
{
  return qtr_tab[(dt_tm.mm - 1)];
}

int dt_get_half_yr_num(utl_datetime dt_tm)
{
  return half_yr_tab[(dt_tm.mm - 1)];
}

int dt_fix_mon_days(utl_datetime *u_dt_tm)
{
  int mn_days = dt_num_days_in_month(*u_dt_tm);
  
  if(!dt_is_valid(*u_dt_tm))
      return ERR_DT_INVALID_DATETIME;
  
  if(u_dt_tm->dd > mn_days) 
  {
    u_dt_tm->dd = mn_days;
    __set_wkd_doy(u_dt_tm);
  }

  return 0;
}

time_t dt_incr_year_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_incr_year(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_decr_year_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_decr_year(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_incr_month_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_incr_month(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_decr_month_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_decr_month(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_incr_day_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_incr_day(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_decr_day_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_decr_day(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_incr_hour_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_incr_hour(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_decr_hour_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_decr_hour(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_incr_min_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_incr_min(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

time_t dt_decr_min_epoch(time_t dtm_epoch, int ntimes)
{
  utl_datetime u_dt_tm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dt_tm);
  dt_decr_min(&u_dt_tm, ntimes);
  return dt_datetime_to_epoch(u_dt_tm);
}

int dt_incr_year(utl_datetime *u_dt_tm, int ntimes)
{
    time_t dtm_epoch;
    int year, day;
    int yrdays, mdays;
    utl_datetime u_dt_tm2 = { 0 };

    if(u_dt_tm == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(*u_dt_tm))
      return ERR_DT_INVALID_DATETIME;
    
    __set_wkd_doy(u_dt_tm);
    
    dtm_epoch = dt_datetime_to_epoch(*u_dt_tm);
    yrdays = dt_rem_days_in_year(*u_dt_tm) - 1;
    dtm_epoch += (yrdays * DT_SECS_IN_DAY);
    
    year = u_dt_tm->yy;
    for(int i = 1; i < ntimes; i++)
      dtm_epoch += (dt_num_days_in_y(++year) * DT_SECS_IN_DAY);
    
    year++;
    day = u_dt_tm->dd;
    mdays = dt_num_days_in_month(*u_dt_tm);
    if(day > mdays)
      day = mdays;
    
    dt_create_datetime(&u_dt_tm2, year, u_dt_tm->mm, day, u_dt_tm->hh, u_dt_tm->mi, u_dt_tm->ss);

    dtm_epoch += u_dt_tm2.doy * DT_SECS_IN_DAY;
    dt_epoch_to_datetime(dtm_epoch, u_dt_tm);

    return 0;   
}

int dt_decr_year(utl_datetime *u_dt_tm, int ntimes)
{
    time_t dtm_epoch;
    int year, day;
    int yrdays, mdays;
    utl_datetime u_dt_tm2 = { 0 };

    if(u_dt_tm == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(*u_dt_tm))
      return ERR_DT_INVALID_DATETIME;
    
    __set_wkd_doy(u_dt_tm);

    dtm_epoch = dt_datetime_to_epoch(*u_dt_tm);
    dtm_epoch -= (u_dt_tm->doy * DT_SECS_IN_DAY);
    
    year = u_dt_tm->yy;
    for(int i = 1; i < ntimes; i++)
      dtm_epoch -= (dt_num_days_in_y(--year) * DT_SECS_IN_DAY);

    year--;
    day = u_dt_tm->dd;
    mdays = dt_num_days_in_month(*u_dt_tm);
    if(day > mdays)
      day = mdays;
    
    dt_create_datetime(&u_dt_tm2, year, u_dt_tm->mm, day, u_dt_tm->hh, u_dt_tm->mi, u_dt_tm->ss);

    yrdays = dt_rem_days_in_year(u_dt_tm2);
    dtm_epoch -= yrdays * DT_SECS_IN_DAY;
    
    dt_epoch_to_datetime(dtm_epoch, u_dt_tm);

    return 0;   
}

int dt_incr_month(utl_datetime *u_dt_tm, int ntimes)
{
    time_t dtm_epoch;
    int day, mrdays, mdays;
    st_ym ym;

    if(u_dt_tm == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(*u_dt_tm))
      return ERR_DT_INVALID_DATETIME;
    
    __set_wkd_doy(u_dt_tm);

    dtm_epoch = dt_datetime_to_epoch(*u_dt_tm);
    mrdays = dt_rem_days_in_month(*u_dt_tm);
    dtm_epoch += (mrdays * DT_SECS_IN_DAY);
    
    ym.yy = u_dt_tm->yy; 
    ym.mm = u_dt_tm->mm;

    for(int i=1; i < ntimes; i++)
    {
       dt_next_month_ym(&ym);
       dtm_epoch += (dt_num_days_in_ym(ym.yy, ym.mm) * DT_SECS_IN_DAY);   
    }
    
    dt_next_month_ym(&ym);
    day = u_dt_tm->dd;
    mdays = dt_num_days_in_ym(ym.yy, ym.mm);
    if(day > mdays)
      day = mdays;
    
    dtm_epoch += day * DT_SECS_IN_DAY;
    dt_epoch_to_datetime(dtm_epoch, u_dt_tm);
    
    return 0;
}

int dt_decr_month(utl_datetime *u_dt_tm, int ntimes)
{
    time_t dtm_epoch;
    int day, mdays;
    utl_datetime u_dt_tm2 = { 0 };
    st_ym ym;

    if(u_dt_tm == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(*u_dt_tm))
      return ERR_DT_INVALID_DATETIME;
    
    __set_wkd_doy(u_dt_tm);

    dtm_epoch = dt_datetime_to_epoch(*u_dt_tm);
    dtm_epoch -= u_dt_tm->dd * DT_SECS_IN_DAY;
    ym.yy = u_dt_tm->yy; 
    ym.mm = u_dt_tm->mm;

    for(int i=1; i < ntimes; i++)
    {
       dt_prev_month_ym(&ym);
       dtm_epoch -= (dt_num_days_in_ym(ym.yy, ym.mm) * DT_SECS_IN_DAY);   
    }
    
    dt_prev_month_ym(&ym);
    day = u_dt_tm->dd;
    mdays = dt_num_days_in_ym(ym.yy, ym.mm);
    if(day > mdays)
      day = mdays;
    
    dt_create_datetime(&u_dt_tm2, ym.yy, ym.mm, day, u_dt_tm->hh, u_dt_tm->mi, u_dt_tm->ss);
    dtm_epoch -= dt_rem_days_in_month(u_dt_tm2) * DT_SECS_IN_DAY;
    dt_epoch_to_datetime(dtm_epoch, u_dt_tm);
    
    return 0;
}

int dt_incr_day(utl_datetime *u_dt_tm, int ntimes)
{
  return dt_incr_sec(u_dt_tm, (ntimes * DT_SECS_IN_DAY));
}

int dt_decr_day(utl_datetime *u_dt_tm, int ntimes)
{
  return dt_incr_sec(u_dt_tm, (-1 * ntimes * DT_SECS_IN_DAY));
}

int dt_incr_hour(utl_datetime *u_dt_tm, int ntimes)
{
  return dt_incr_sec(u_dt_tm, (ntimes * DT_SECS_IN_HOUR));
}

int dt_decr_hour(utl_datetime *u_dt_tm, int ntimes)
{
  return dt_incr_sec(u_dt_tm, (-1 * ntimes * DT_SECS_IN_HOUR));
}

int dt_incr_min(utl_datetime *u_dt_tm, int ntimes)
{
  return dt_incr_sec(u_dt_tm, (ntimes * DT_SECS_IN_MIN));
}

int dt_decr_min(utl_datetime *u_dt_tm, int ntimes)
{
  return dt_incr_sec(u_dt_tm, (-1 * ntimes * DT_SECS_IN_MIN));
}

int dt_incr_sec(utl_datetime *u_dt_tm, int ntimes)
{
    time_t dtm_epoch;

    if(u_dt_tm == NULL)
      return ERR_DT_NULL_VALUE;

    if(!dt_is_valid(*u_dt_tm))
      return ERR_DT_INVALID_DATETIME;
    
    __set_wkd_doy(u_dt_tm);

    dtm_epoch = dt_datetime_to_epoch(*u_dt_tm);
    dtm_epoch += ntimes;
    
    dt_epoch_to_datetime(dtm_epoch, u_dt_tm);
    
    return 0;
}

int dt_decr_sec(utl_datetime *u_dt_tm, int ntimes)
{
    return dt_incr_sec(u_dt_tm, (-1 * ntimes));
}

int dt_incr_intrvl(utl_datetime *u_dt_tm, E_DTM_INTRVL intrvl, int ntimes)
{
  if(intrvl == DTM_INTRVL_YEAR)
    return dt_incr_year(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_MONTH)
    return dt_incr_month(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_DAY)
    return dt_incr_day(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_HOUR)
    return dt_incr_hour(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_MIN)
    return dt_incr_min(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_SEC)
    return dt_incr_sec(u_dt_tm, ntimes);
  
  return ERR_DT_INVALID_INTERVAL;
}

int dt_decr_intrvl(utl_datetime *u_dt_tm, E_DTM_INTRVL intrvl, int ntimes)
{
  if(intrvl == DTM_INTRVL_YEAR)
    return dt_decr_year(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_MONTH)
    return dt_decr_month(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_DAY)
    return dt_decr_day(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_HOUR)
    return dt_decr_hour(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_MIN)
    return dt_decr_min(u_dt_tm, ntimes);
  if(intrvl == DTM_INTRVL_SEC)
    return dt_decr_sec(u_dt_tm, ntimes);
  
  return ERR_DT_INVALID_INTERVAL;
}

int dt_sec_frm_mn_hms(int hh, int mi, int ss)
{
    int tm_epoch = 0;

    if(!dt_is_valid_hms(hh, mi, ss))
      return ERR_DT_INVALID_HMS;

    tm_epoch += (hh * DT_SECS_IN_HOUR);
    tm_epoch += (mi * DT_SECS_IN_MIN); 
    tm_epoch += ss;
    
    return tm_epoch;
}

time_t dt_sec_frm_mn(utl_datetime u_dtm)
{
  return dt_sec_frm_mn_hms(u_dtm.hh, u_dtm.mi, u_dtm.ss); 
}

time_t dt_sec_frm_mn_time(utl_time u_tm)
{
    return dt_sec_frm_mn_hms(u_tm.hh, u_tm.mi, u_tm.ss); 
}

int dt_diff(utl_datetime u_dtm1, utl_datetime u_dtm2)
{
   return dt_datetime_to_epoch(u_dtm1) - dt_datetime_to_epoch(u_dtm2);
}

int dt_diff_date(utl_date u_dt1, utl_date u_dt2)
{
   return dt_date_to_epoch(u_dt1) - dt_date_to_epoch(u_dt2);
}

int dt_diff_time(utl_time u_tm1, utl_time u_tm2)
{
   return dt_sec_frm_mn_time(u_tm1) - dt_sec_frm_mn_time(u_tm2);
}

int dt_incr_nxt_wkday(utl_datetime *u_dtm, E_DTM_WKDY wd)
{
  int c_day = u_dtm->wkday;
  int t_day = (int)wd;
  int incr_days = 0;

  if(u_dtm ==  NULL)
    return ERR_DT_NULL_VALUE;

  if(t_day <= c_day)
    t_day += DT_DAYS_IN_WEEK;
  
  incr_days = (t_day - c_day);
  return dt_incr_day(u_dtm, incr_days);
}

int dt_decr_prv_wkday(utl_datetime *u_dtm, E_DTM_WKDY wd)
{
  int c_day = u_dtm->wkday;
  int t_day = (int)wd;
  int decr_days = 0;

  if(u_dtm ==  NULL)
    return ERR_DT_NULL_VALUE;

  if(c_day <= t_day)
    t_day -= DT_DAYS_IN_WEEK;
  
  decr_days = (c_day - t_day);
  return dt_decr_day(u_dtm, decr_days);
}

int dt_is_month_end(utl_datetime u_dtm)
{
  return (u_dtm.dd == dt_num_days_in_month(u_dtm));
}

int dt_is_year_end(utl_datetime u_dtm)
{
  return (u_dtm.mm == DT_MONTHS_IN_YEAR && u_dtm.dd == day_tab[0][11]);
}

int dt_is_year_start(utl_datetime u_dtm)
{
  return (u_dtm.mm == 1 && u_dtm.dd == 1);
}

int dt_is_qtr_end(utl_datetime u_dtm)
{
  return (dt_is_qtr_end_m(u_dtm.mm) && u_dtm.dd == day_tab[0][(u_dtm.mm - 1)]);
}

int dt_is_qtr_start(utl_datetime u_dtm)
{
  return (dt_is_qtr_start_m(u_dtm.mm) && u_dtm.dd == 1);
}

int dt_is_half_yr_end(utl_datetime u_dtm)
{
  return (dt_is_half_yr_end_m(u_dtm.mm) && u_dtm.dd == day_tab[0][(u_dtm.mm - 1)]);
}

int dt_is_half_yr_start(utl_datetime u_dtm)
{
  return (dt_is_half_yr_end_m(u_dtm.mm) && u_dtm.dd == 1);
}

int dt_datetime_set_year(utl_datetime *u_dtm, int yy)
{
  return dt_datetime_set_ymd(u_dtm, yy, u_dtm->mm, u_dtm->dd);
}

int dt_datetime_set_month(utl_datetime *u_dtm, int mm)
{
  return dt_datetime_set_ymd(u_dtm, u_dtm->yy, mm, u_dtm->dd);
}

int dt_datetime_set_day(utl_datetime *u_dtm, int dd)
{
  return dt_datetime_set_ymd(u_dtm, u_dtm->yy, u_dtm->mm, dd);
}

int dt_datetime_set_ymd(utl_datetime *u_dtm, int yy, int mm, int dd)
{
  if((yy < DT_MIN_YEAR || yy > DT_MAX_YEAR) ||
      (mm < DT_MIN_MONTH || mm > DT_MONTHS_IN_YEAR) ||
      (dd < DT_MIN_DAY || dd > DT_MAX_DAY))
    return ERR_DT_INVALID_YMD;
  
  u_dtm->yy = yy;
  u_dtm->mm = mm;
  u_dtm->dd = dd;

  dt_fix_mon_days(u_dtm);
  
  __set_wkd_doy(u_dtm);
  
  return 0;
}

int dt_datetime_set_hms(utl_datetime *u_dtm, int hh, int mi, int ss)
{
  if(!dt_is_valid_hms(hh, mi, ss))
    return ERR_DT_INVALID_HMS;
  
  u_dtm->hh = hh;
  u_dtm->mi = mi;
  u_dtm->ss = ss;
  
  return 0;
}

int dt_datetime_set_dttm(utl_datetime *u_dtm, int yy, int mm, int dd, int hh, int mi, int ss)
{
  if(!dt_is_valid_ymd(yy, mm, dd))
    return ERR_DT_INVALID_YMD;
  
  if(!dt_is_valid_hms(hh, mi, ss))
    return ERR_DT_INVALID_HMS;
  
  u_dtm->yy = yy;
  u_dtm->mm = mm;
  u_dtm->dd = dd;
  u_dtm->hh = hh;
  u_dtm->mi = mi;
  u_dtm->ss = ss;
  
  __set_wkd_doy(u_dtm);
  
  return 0;
}

int dt_date_set_year(utl_date *u_dt, int yy)
{
  return dt_date_set_ymd(u_dt, yy, u_dt->mm, u_dt->dd);
}

int dt_date_set_month(utl_date *u_dt,  int mm)
{
  return dt_date_set_ymd(u_dt, u_dt->yy, mm, u_dt->dd);
}

int dt_date_set_day(utl_date *u_dt,  int dd)
{
  return dt_date_set_ymd(u_dt, u_dt->yy, u_dt->mm, dd);
}

int dt_date_set_ymd(utl_date *u_dt, int yy, int mm, int dd)
{
  utl_datetime u_dtm = { 0 };
  dt_date_to_datetime(*u_dt, &u_dtm);
  int retval = dt_datetime_set_ymd(&u_dtm, yy, mm, dd);
  
  if( retval < 0)
    return retval;
  
  u_dt->yy = u_dtm.yy;
  u_dt->mm = u_dtm.mm;
  u_dt->dd = u_dtm.dd;
  
  __set_wkd_doy_date(u_dt);
  
  return 0;
}

int dt_time_set_hms(utl_time *u_tm, int hh, int mi, int ss)
{
  if(!dt_is_valid_hms(hh, mi, ss))
    return ERR_DT_INVALID_HMS;
  
  u_tm->hh = hh;
  u_tm->mi = mi;
  u_tm->ss = ss;
  
  return 0;
}

int dt_epoch_to_str(time_t dtm_epoch, char *dtm_str)
{
  utl_datetime u_dtm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dtm);
  return dt_datetime_to_str(u_dtm, dtm_str);
}

int dt_epoch_to_date_str(time_t dtm_epoch, char *dtm_str)
{
  utl_datetime u_dtm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dtm);
  return dt_datetime_to_date_str(u_dtm, dtm_str);
}

int dt_epoch_to_time_str(time_t dtm_epoch, char *dtm_str)
{
  utl_datetime u_dtm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dtm);
  return dt_datetime_to_time_str(u_dtm, dtm_str);
}

int dt_epoch_to_linux_str(time_t dtm_epoch, char *dtm_str)
{
  utl_datetime u_dtm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dtm);
  return dt_datetime_to_linux_str(u_dtm, dtm_str);
}

int dt_epoch_to_unix_str(time_t dtm_epoch, char *dtm_str)
{
  utl_datetime u_dtm = { 0 };
  dt_epoch_to_datetime(dtm_epoch, &u_dtm);
  return dt_datetime_to_unix_str(u_dtm, dtm_str);
}

int dt_datetime_to_str(utl_datetime u_dtm, char *dtm_str)
{
  if(dtm_str == NULL)
    return ERR_DT_NULL_VALUE;

  if(!dt_is_valid(u_dtm))
    return ERR_DT_INVALID_DATETIME;
  
  sprintf(dtm_str, "%4d-%02d-%02dT%02d:%02d:%02d",
                  u_dtm.yy, u_dtm.mm, u_dtm.dd, 
                  u_dtm.hh, u_dtm.mi, u_dtm.ss);
  return 0;
}

int dt_datetime_to_date_str(utl_datetime u_dtm, char *dtm_str)
{
  if(dtm_str == NULL)
    return ERR_DT_NULL_VALUE;

  if(!dt_is_valid(u_dtm))
    return ERR_DT_INVALID_DATETIME;
  
  sprintf(dtm_str, "%4d-%02d-%02d", u_dtm.yy, u_dtm.mm, u_dtm.dd);
  return 0;
}

int dt_datetime_to_time_str(utl_datetime u_dtm, char *dtm_str)
{
  if(dtm_str == NULL)
    return ERR_DT_NULL_VALUE;

  if(!dt_is_valid(u_dtm))
    return ERR_DT_INVALID_DATETIME;

  sprintf(dtm_str, "%02d:%02d:%02d", u_dtm.hh, u_dtm.mi, u_dtm.ss);
  return 0;
}

int dt_datetime_to_linux_str(utl_datetime u_dtm, char *unx_str)
{
  int hour;
  
  if(unx_str == NULL)
    return ERR_DT_NULL_VALUE;
  
  if(!dt_is_valid(u_dtm))
    return ERR_DT_INVALID_DATETIME;

  if(u_dtm.hh > 12)
    hour = (u_dtm.hh - 12);
  else
    hour = u_dtm.hh;

  sprintf(unx_str, "%s %02d %s %04d %02d:%02d:%02d %s %s",
                  dt_get_wkday_name_initcap(u_dtm), u_dtm.dd, dt_get_mon_name_initcap(u_dtm), u_dtm.yy,
                  hour, u_dtm.mi, u_dtm.ss, dt_get_am_pm_hour(u_dtm.hh), DT_TZ_NAME);
  return 0;
}

int dt_datetime_to_unix_str(utl_datetime u_dtm, char *unx_str)
{

  if(unx_str == NULL)
    return ERR_DT_NULL_VALUE;
  
  sprintf(unx_str, "%s %02d %s %02d:%02d:%02d %s %04d",
                  dt_get_wkday_abbr_initcap(u_dtm), u_dtm.dd, dt_get_mon_abbr_initcap(u_dtm), u_dtm.hh, u_dtm.mi, u_dtm.ss,
                  DT_TZ_NAME, u_dtm.yy);
  return 0;
}

int dt_date_to_str(utl_date u_dt, char *dt_str)
{
  utl_datetime u_dtm = { 0 };
  int retval = 0;
  retval = dt_date_to_datetime(u_dt, &u_dtm);
  if( retval < 0)
    return retval;

  dt_datetime_to_date_str(u_dtm, dt_str);
  return 0;
}

int dt_date_to_linux_str(utl_date u_dt, char *dt_str)
{
  utl_datetime u_dtm = { 0 };
  int retval = 0;
  retval = dt_date_to_datetime(u_dt, &u_dtm); 
  if(retval < 0)
    return retval;

  dt_datetime_to_linux_str(u_dtm, dt_str);
  return 0;
}

int dt_date_to_unix_str(utl_date u_dt, char *dt_str)
{
  utl_datetime u_dtm = { 0 };
  int retval = 0;
  retval = dt_date_to_datetime(u_dt, &u_dtm);
  if(retval < 0)
    return retval;

  dt_datetime_to_unix_str(u_dtm, dt_str);
  return 0;
}

int dt_time_to_str(utl_time u_tm, char *tm_str)
{
  utl_datetime u_dtm = { 0 };
  int retval = 0;
  retval = dt_time_to_datetime(u_tm, &u_dtm); 
  if( retval < 0)
    return retval;

  dt_datetime_to_time_str(u_dtm, tm_str);
  return 0;
}

int dt_format_systm(char *fmt_str, char *dtm_str, int fmt_sz)
{
  utl_datetime sys_dtm = { 0 };
  dt_systm_to_datetime(&sys_dtm);
  return dt_format_epoch(dt_datetime_to_epoch(sys_dtm), fmt_str, dtm_str, fmt_sz);
}

int dt_format_datetime(utl_datetime u_dtm, char *fmt_str, char *dtm_str, int fmt_sz)
{
  if(dt_is_valid(u_dtm))
    return dt_format_epoch(dt_datetime_to_epoch(u_dtm), fmt_str, dtm_str, fmt_sz);
  else
    return ERR_DT_INVALID_DATETIME;
}

int dt_format_epoch(time_t dtm_epoch, char *fmt_str, char *dtm_str, int fmt_sz)
{
  struct tm *tm = NULL;

  tm = localtime(&dtm_epoch);
  if(tm == NULL)
    return ERR_DT_GET_SYS_RTC;


  if (strftime(dtm_str, fmt_sz, fmt_str, tm) == 0) 
    return DT_FAILURE;
  
  return DT_SUCCESS;
}

int dt_gen_dttm_lst_range(utl_datetime *u_dtm_lst, E_DTM_INTRVL intrvl, utl_datetime st_dtm, utl_datetime ed_dtm, int lst_sz)
{
  utl_datetime cur_dtm = { 0 };
  utl_datetime nxt_dtm = { 0 };
  int retval = 0;
  int lst_idx = 0;

  if(u_dtm_lst == NULL)
    return ERR_DT_NULL_VALUE;
  
  if(lst_sz <= 0)
    return ERR_DT_INVALID_LIST_SIZE;
  
  if(!dt_is_valid(st_dtm))
    return ERR_DT_INVALID_DATETIME;

  if(!dt_is_valid(ed_dtm))
    return ERR_DT_INVALID_DATETIME;

  if(dt_diff(ed_dtm, st_dtm) <=0)
    return ERR_DT_INVALID_DATE_RANGE; 
  
  dt_datetime_dup(&nxt_dtm, &st_dtm);

  retval = dt_incr_intrvl(&nxt_dtm, intrvl, 1); 
  if(retval < 0)
    return retval;

  if(dt_diff(nxt_dtm, ed_dtm) > 0)
    return ERR_DT_INVALID_DATE_RANGE; 
    
  dt_datetime_dup(&cur_dtm, &nxt_dtm);
  dt_datetime_dup((u_dtm_lst + lst_idx), &st_dtm);
  lst_idx++;
  while(lst_idx < lst_sz && dt_diff(ed_dtm, cur_dtm) >= 0)
  {
    dt_datetime_dup((u_dtm_lst + lst_idx), &cur_dtm);
    retval = dt_incr_intrvl(&cur_dtm, intrvl, 1);
    if(retval < 0)
      return retval;
    
    lst_idx++;
  }

  return 0;
}

int dt_gen_dttm_lst_count(utl_datetime *u_dtm_lst, E_DTM_INTRVL intrvl, utl_datetime st_dtm, int count)
{
  int retval = 0;
  utl_datetime cur_dtm = { 0 };
  int lst_idx = 0;
  
  if(u_dtm_lst == NULL)
    return ERR_DT_NULL_VALUE;
  
  if(count <= 0)
    return ERR_DT_INVALID_LIST_SIZE;

  if(!dt_is_valid(st_dtm))
    return ERR_DT_INVALID_DATETIME;

  dt_datetime_dup(&cur_dtm, &st_dtm);
  while(lst_idx < count)
  {
    dt_datetime_dup((u_dtm_lst + lst_idx), &cur_dtm);
    retval = dt_incr_intrvl(&cur_dtm, intrvl, 1);
    if(retval < 0)
      return retval;

    lst_idx++;
  }

  return 0;
}
