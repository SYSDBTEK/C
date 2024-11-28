#ifndef _UTIL_HDR_
#define _UTIL_HDR_ 1

#include<stdint.h>
#include<time.h>

enum CHECK_MOD_CRC_TYPE { 
       MOD_CRC_ASCII = 0, 
       MOD_CRC_HEX = 1 
};
typedef enum CHECK_MOD_CRC_TYPE E_CHECK_MOD_CRC_TYPE;

typedef union {
  uint32_t n;
  unsigned char ba[4];
} u_i32;

typedef union {
  uint16_t n;
  unsigned char ba[2];
} u_i16;

void print_debug_data(uint8_t *, int);
void mem_zero(void *, int);

uint32_t ba2ui32(uint8_t *, uint8_t);
uint16_t ba2ui16(uint8_t *, uint8_t);
void ui322ba(const uint32_t, uint8_t *);
void ui162ba(const uint16_t, uint8_t *);
void ui322xstr(const uint32_t, char *);
void ui162xstr(const uint16_t, char *);
void ba2xstr(const uint8_t *, const uint16_t, char *);

uint8_t xchr2num(const char);
void byte2xstr(uint8_t, char *);
uint8_t xstr2byte(char *);
int xstr2ba(char *, int , uint8_t *);
int xstr2ui32ba(char *hex, uint8_t *ba);
int xstr2ui16ba(char *hex, uint8_t *ba);
uint32_t xstr2ui32(char *hex);
uint16_t xstr2ui16(char *hex);
void i162ba32(int16_t val, uint8_t *ba);
void i82ba32(int8_t val, uint8_t *ba);

uint16_t ca2ui16(const char *, const uint8_t, const uint8_t);
int check_mod_crc_ascii(uint8_t *, int);
int check_mod_crc_hex(char *);
uint16_t modcrc_ascii(uint8_t *, const uint16_t);
uint16_t modcrc_hex(uint8_t *, const uint16_t);
uint16_t crc16_x25(const uint8_t *, int);
void crc2ba(const uint16_t, uint8_t *);
void crc2bahex(uint16_t, char *);
void get_sys_rtc_epoch_hex(char *hex);
time_t get_sys_rtc_epoch();
int set_sys_rtc_epoch(const time_t);
int set_sys_rtc_epoch_hex(char *hexrtc);
int get_log_tm_str(char *);
int is_leap_year_epoch(time_t tm_epoch);
int is_leap_year_sys_rtc();
int epoch2str(time_t, char *);

int get_iface_macid_xstr(char * ifname, char *macid);
int get_eth_macid_xstr(char *macid);
int get_wlan_macid_xstr(char *macid);

int is_file_exists(char *);
int is_file_readable(char *);
int is_file_writable(char *);
int get_file_size(char *);
int file_delete(char *fn);
#endif
