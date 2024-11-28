#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "util.h"
#include "comm_defs.h"
#include "errno_defs.h"
#include "debug_varfn.h"
#include "str_util.h"

static const uint8_t hextab[]="0123456789ABCDEF"; 
static const char MOD_CRC_TYPE_NAME[2][6] = {"ASCII", "HEX"};

void print_debug_data(uint8_t *data, int len)
{
  char __FUNC_NAME[30] = "print_debug_data";
  char msg[4096];
  char ln[80];
  
  if(data == NULL)
    return;
  
  memset(msg, 0x00, sizeof(msg));
  memset(ln, 0x00, sizeof(ln));
  sprintf(ln, "Data Len: %d bytes\n", len);
  strcat(msg, ln);
  for(int i=0; i<len; i++)
  {
    if(i > 0) 
      strcat(msg, ", ");
    memset(ln, 0x00, sizeof(ln));
    sprintf(ln, "%d: %02X",i, data[i]);
    strcat(msg, ln);
  }
  DEBUG_PRINT(__FUNC_NAME, "%s\n", msg);  
}

void mem_zero(void *buf, int sz)
{
   memset(buf, 0x00, sz);
}

void ba2xstr(const uint8_t *ba, const uint16_t len, char *xstr)
{
  int n = len * 2;
  while(--n >=0 ) xstr[n] = hextab[(ba[n>>1] >> ((1 - (n & 1)) << 2)) & 0xf];
}

uint8_t xchr2num(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

void byte2xstr(uint8_t b, char *hex)
{
  hex[0] = hextab[(b >> 4) & 0xf];
  hex[1] = hextab[b & 0xf];
}

uint8_t xstr2byte(char *hex)
{
	int a, b;
	a = xchr2num(*hex++);
	if (a < 0)
		return -1;

	b = xchr2num(*hex);
	if (b < 0)
		return -1;

	return (a << 4) | b;
}

int xstr2ba(char *hex, int len, uint8_t *ba)
{
	int ba_len = 0;
  int hex_len = 0;

  if((hex == NULL || hex[0]=='\0') || ba == NULL || (len % 2))
    return -1;
  
  hex_len = strlen(hex);
  if(hex_len < len || len > hex_len)
    return -1;
  
  ba_len = len/2;
  for(int i=0; i < ba_len; i++) 
  {
    if((ba[i]=xstr2byte(hex + (i*2))) < 0)
      return -1;
  }
  return 0;
}

int xstr2ui32ba(char *hex, uint8_t *ba)
{
  return xstr2ba(hex, sizeof(uint32_t) * 2, ba);
}

int xstr2ui16ba(char *hex, uint8_t *ba)
{
  return xstr2ba(hex, sizeof(uint16_t) * 2, ba);
}

uint32_t xstr2ui32(char *hex)
{
  uint8_t ba[4] = { 0 };
  xstr2ui32ba(hex, ba);
  return ba2ui32(ba, 0);
}

uint16_t xstr2ui16(char *hex)
{
  uint8_t ba[2] = { 0 };
  xstr2ui16ba(hex, ba);
  return ba2ui16(ba, 0);
}

uint32_t ba2ui32(uint8_t *bytes, uint8_t spos)
{
    u_i32 ui32;
    uint8_t *buf = bytes + spos;
    ui32.ba[3]=*buf++;
    ui32.ba[2]=*buf++;
    ui32.ba[1]=*buf++;
    ui32.ba[0]=*buf;
    return ui32.n;
}

uint16_t ba2ui16(uint8_t *bytes, uint8_t spos)
{
    u_i16 ui16;
    uint8_t *buf = bytes + spos;
    ui16.ba[1]=*buf++;
    ui16.ba[0]=*buf;
    return ui16.n;
}

void ui322ba(const uint32_t val, uint8_t *buf)
{
    u_i32 ui32;
    ui32.n=val;

    buf[0] = ui32.ba[3];
    buf[1] = ui32.ba[2];
    buf[2] = ui32.ba[1];
    buf[3] = ui32.ba[0];
}

void ui162ba(const uint16_t val, uint8_t *buf)
{
    u_i16 ui16;
    ui16.n=val;

    buf[0] = ui16.ba[1];
    buf[1] = ui16.ba[0];
}

void ui322xstr(const uint32_t val, char *buf)
{
   uint8_t ba[4] = { 0 };
   ui322ba(val, ba);
   ba2xstr(ba, 4, buf);
}

void ui162xstr(const uint16_t val, char *buf)
{
   uint8_t ba[2] = { 0 };
   ui162ba(val, ba);
   ba2xstr(ba, 2, buf);
}

void i162ba32(int16_t val, uint8_t *ba)
{
   if(val < 0)
   {
    ba[0]=0xFF;
    ba[1]=0xFF;
    ba[2]=(uint8_t)(val>>8);
    ba[3]=(uint8_t)val;
   }
   else
   {
     ba[0]=0;
     ba[1]=0;
     ba[2]=(uint8_t)(val>>8);
     ba[3]=(uint8_t)val;
   }
}

void i82ba32(int8_t val, uint8_t *ba)
{
   if(val < 0)
   {
    ba[0]=0xFF;
    ba[1]=0xFF;
    ba[2]=0xFF;
    ba[3]=(uint8_t)val;
   }
   else
   {
     ba[0]=0;
     ba[1]=0;
     ba[2]=0;
     ba[3]=(uint8_t)val;
   }
}

uint16_t ca2ui16(const char *chars, const uint8_t start, const uint8_t len)
{
    uint16_t num = 0;
    uint16_t nv = 0;
    uint16_t n10 = 0;
    uint8_t se = 0;
    uint8_t sl = strlen(chars);
    char nc;

    if(chars == NULL)
       return -1;
    
    if(start >= sl)
       return -1;

    sl = strlen(chars + start);

    if((len == 1) || (sl==1) )
    {  
      nc = chars[start];
      if(isdigit(nc))
        return (uint16_t)(nc - '0');
      else
        return 0;
    }

    se = start + ((sl < len)?sl:len);
    for (int i = start; i < se; i++)
    {
       nc = chars[i];
       
       if(isdigit(nc))
       {
         nv = (uint16_t)(nc - '0');
         n10 = (se-i);
         if(n10 > 1) 
         {
           for(int j=1; j<n10; j++)
              nv*=10;
         }
         
         num += nv;
       }
       else
         return 0;
    }
    return num;
}

uint16_t crc16_x25(const uint8_t *pData, int length)
{
    int i;
    uint16_t wCrc = 0xffff;
    while (length--) {
        wCrc ^= *(uint8_t *)pData++ << 0;
        for (i=0; i < 8; i++)
            wCrc = wCrc & 0x0001 ? (wCrc >> 1) ^ 0x8408 : wCrc >> 1;
    }
    return wCrc ^ 0xffff;
}

static int check_mod_crc(E_CHECK_MOD_CRC_TYPE crc_type, uint8_t *data, int data_len)
{
  char __FUNC_NAME[] = "check_mod_crc";
  uint8_t *crc_buf = NULL;
  int crc_len;

  int retval = 0;
  uint16_t crc_val = 0;
  uint8_t dat_crc_ba[2] = { 0 };
  uint8_t crc_ba[2] = { 0 };
  
  if(crc_type == MOD_CRC_ASCII)
  {
    crc_len = (data_len - 2);
    dat_crc_ba[0] = data[(crc_len + 1)];
    dat_crc_ba[1] = data[crc_len];
  }
  else
  {
    crc_len = (data_len - 4);
    dat_crc_ba[0] = xstr2byte((char *)(data + crc_len + 2));
    dat_crc_ba[1] = xstr2byte((char *)(data + crc_len));
  }

  crc_buf = (uint8_t *)calloc(crc_len, sizeof(uint8_t));
  if(crc_buf == NULL) return ERR_MEM_ALLOC_FAILED;

  memcpy(crc_buf, data, crc_len);
  if(crc_buf == NULL) return ERR_EMPTY_STRING;

  if(crc_type == MOD_CRC_ASCII)
    crc_val = modcrc_ascii(crc_buf, crc_len);
  else
    crc_val = modcrc_hex(crc_buf, crc_len);
  
  crc2ba(crc_val, crc_ba);
  
  if(IS_DEBUG()==TRUE)
  {
    DEBUG_PRINT(__FUNC_NAME,"Calc CRC[%s]: %02X %02X\n", MOD_CRC_TYPE_NAME[crc_type], crc_ba[0], crc_ba[1]);
    DEBUG_PRINT(__FUNC_NAME,"Data CRC[%s]: %02X %02X\n", MOD_CRC_TYPE_NAME[crc_type], dat_crc_ba[0], dat_crc_ba[1]);
  }

  // printf("Calc CRC[%s]: %02X %02X\n", MOD_CRC_TYPE_NAME[crc_type], crc_ba[0], crc_ba[1]);
  // printf("Data CRC[%s]: %02X %02X\n", MOD_CRC_TYPE_NAME[crc_type], dat_crc_ba[0], dat_crc_ba[1]);

  if((crc_ba[0] == dat_crc_ba[0]) && (crc_ba[1] == dat_crc_ba[1]))
     retval = SUCCESS;
  else
     retval = ERR_CRC_NOT_MATCH;
  
  free(crc_buf);
  return retval;
}

int check_mod_crc_ascii(uint8_t *data, int data_len)
{
  return check_mod_crc(MOD_CRC_ASCII, data, data_len);
}

int check_mod_crc_hex(char *data)
{
  int data_len = 0;
  if(str_isempty(data))
    return ERR_EMPTY_STRING;
  
  data_len = strlen(data);

  return check_mod_crc(MOD_CRC_HEX, (uint8_t *)data, data_len);
}

static uint16_t modcrc(const uint8_t *dpacket, const uint16_t len)	 // CRC Function(Error calcualtion)
{
  uint16_t crc = 0xffff, poly = 0xa001;
  uint16_t i = 0, j = 0;  

	for(i = 0; i < len; i++)
	{
	   crc^= (uint16_t)dpacket[i];
	   for(j = 0; j < 8; j++)
	   {
		   if(crc & 0x01)
		   {
		      crc >>= 1;
		      crc ^= poly;
		   }
		   else
		      crc >>= 1;
		}
	}

	return (crc);	  
}

uint16_t modcrc_ascii(uint8_t *dpacket, const uint16_t len)	 // CRC Function(Error calcualtion)
{
 	return modcrc(dpacket, len);	  
}

uint16_t modcrc_hex(uint8_t *dpacket, const uint16_t len)	 // CRC Function(Error calcualtion)
{
  uint16_t crc;
  uint8_t *crc_buf = NULL;
  int crc_len = 0;
  
  crc_len = len / 2;
  crc_buf = (uint8_t *)calloc(crc_len, sizeof(uint8_t));
  if(crc_buf == NULL) return ERR_MEM_ALLOC_FAILED;
  xstr2ba((char *)dpacket, len, crc_buf);
  crc = modcrc(crc_buf, crc_len);
  free(crc_buf);
  return (crc);	  
}

void crc2ba(uint16_t val, uint8_t *bytes)
{
    bytes[0]=(uint8_t)(val>>8);
    bytes[1]=(uint8_t)val;
}

void crc2bahex(uint16_t val, char *hex)
{
    uint8_t crc_ba[2] = { 0 };
    crc2ba(val, crc_ba);
    ba2xstr(crc_ba, 2, hex);
}

void get_sys_rtc_epoch_hex(char *hex)
{
    struct timespec ts_rtc;

    if(hex == NULL)
      return;
    
    setenv("TZ", TIMEZONE, 1);
    if(clock_gettime(CLOCK_REALTIME, &ts_rtc) < 0)
      hex[0] ='\0';
    else
      ui322xstr((uint32_t) ts_rtc.tv_sec, hex);
}


time_t get_sys_rtc_epoch()
{
    struct timespec ts_rtc;
    
    setenv("TZ", TIMEZONE, 1);
    if(clock_gettime(CLOCK_REALTIME, &ts_rtc) < 0)
      return errno;
    
    return ts_rtc.tv_sec;
}

int set_sys_rtc_epoch(const time_t epoch_tm)
{
  char __FUNC_NAME[30] = "set_sys_rtc_epoch";
  int retval;
  struct timespec ts_rtc;
  char *errstr;

  setenv("TZ", TIMEZONE, 1);
  ts_rtc.tv_sec = epoch_tm;
  ts_rtc.tv_nsec = 0;
  retval = clock_settime(CLOCK_REALTIME, &ts_rtc);
  if(retval < 0)
  {
     if(IS_DEBUG() == TRUE)
     {
        DEBUG_PRINT(__FUNC_NAME, "SYSTEM RTC set failed, errno: %d\n", errno);
        errstr = strerror(errno);
        DEBUG_PRINT(__FUNC_NAME, "SYSTEM RTC error msg: %s\n", errstr);
     }
     return retval;
  }
  return SUCCESS; 
}

int set_sys_rtc_epoch_hex(char *hexrtc)
{
  time_t rtc_epoch;
  // xstr2ui32ba(hexrtc, ba_epoch);
  // rtc_epoch = ba2ui32(ba_epoch, 0);
  rtc_epoch =  xstr2ui32(hexrtc);
  return set_sys_rtc_epoch(rtc_epoch);
}

int get_log_tm_str(char *tm_str)
{
  time_t tm_epoch;
  long tm_ms;
  struct timespec tm_spec;
  struct tm *tm;
  int year;

  setenv("TZ", TIMEZONE, 1);
  //tm_epoch = time(NULL);
  if(clock_gettime(CLOCK_REALTIME, &tm_spec) == -1)
    return -1;

  tm_epoch = tm_spec.tv_sec;
  tm_ms = round(tm_spec.tv_nsec/1000000);
  tm = localtime(&tm_epoch);
  if(tm == NULL)
    return ERR_GET_SYS_RTC;

  year = tm->tm_year + 1900;
  sprintf(tm_str, "%4d-%02d-%02d %02d:%02d:%02d.%03ld",
                  year, (tm->tm_mon + 1), tm->tm_mday, 
                  tm->tm_hour, tm->tm_min, tm->tm_sec, tm_ms);
  return SUCCESS;  
}

int epoch2str(time_t tm_epoch, char *tm_str)
{
  struct tm *tm;
  int year;
  setenv("TZ", TIMEZONE, 1);
  tm = localtime(&tm_epoch);
  if(tm == NULL)
    return ERR_GET_SYS_RTC;

  year = tm->tm_year + 1900;
  sprintf(tm_str, "%4d-%02d-%02d %02d:%02d:%02d",
                  year, (tm->tm_mon + 1), tm->tm_mday, 
                  tm->tm_hour, tm->tm_min, tm->tm_sec);
  return SUCCESS;
}

int is_leap_year_epoch(time_t tm_epoch)
{
  struct tm *tm = NULL;
  int year;

  setenv("TZ", TIMEZONE, 1);
  tm = localtime(&tm_epoch);
  if(tm == NULL)
    return ERR_GET_SYS_RTC;

  year = tm->tm_year + 1900;
  return ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0));
}

int is_leap_year_sys_rtc()
{
 return is_leap_year_epoch(get_sys_rtc_epoch());
}

int get_iface_macid_xstr(char * ifname, char *macid)
{
  struct ifreq s;
  int ret, fd;
  
  if(ifname == NULL || ifname[0]=='\0' || macid == NULL)
    return ERR_NULL_VALUE; 
  
  fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if(fd == -1)
    return -1;

  strcpy(s.ifr_name, ifname);
  ret = ioctl(fd, SIOCGIFHWADDR, &s);
  if (ret == -1)
    return -1;
   
  ba2xstr((uint8_t *)s.ifr_addr.sa_data, 6, macid);
  // for (int i = 0; i < 6; ++i)
  // {
  //   byte2xstr((uint8_t)s.ifr_addr.sa_data[i], macid);
  //   macid += 2;
  // }
  return 0;
}

int get_eth_macid_xstr(char *macid)
{
  if(macid == NULL)
    return -1;

  return get_iface_macid_xstr(ETH_IFNAME, macid);
}

int get_wlan_macid_xstr(char *macid)
{
  if(macid == NULL)
    return -1;

  return get_iface_macid_xstr(WLAN_IFNAME, macid);
}

int is_file_exists(char *fn)
{
  if(fn == NULL || fn[0]=='\0')
    return ERR_FILE_NOT_FOUND;

  return (access(fn, F_OK) == 0);
}

int is_file_readable(char *fn)
{
  if(fn == NULL || fn[0]=='\0')
    return ERR_FILE_NOT_FOUND;

  return ((access(fn, F_OK) == 0) && (access(fn, F_OK | R_OK) == 0));
}

int is_file_writable(char *fn)
{
  if(fn == NULL || fn[0]=='\0')
    return ERR_FILE_NOT_FOUND;

  return ((access(fn, F_OK) == 0) && (access(fn, F_OK | W_OK) == 0));
}

int get_file_size(char *fn)
{
  struct stat sb;
  if(fn == NULL || fn[0]=='\0' || !is_file_exists(fn))
    return ERR_FILE_NOT_FOUND;

  if(lstat(fn, &sb))
    return -1;
  
  return sb.st_size;
}

int file_delete(char *fn)
{
  if(fn == NULL || fn[0]=='\0')
    return ERR_FILE_NOT_FOUND;

  if(is_file_exists(fn))
    return unlink(fn);
  else
    return ERR_FILE_NOT_FOUND;
}
