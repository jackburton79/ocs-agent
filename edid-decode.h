#ifndef __EDID_DECODE_H
#define __EDID_DECODE_H


struct edid_info {
  char description[32];
  char manufacturer[16];
  char type[16];
  int model;
  char serial_number[32];
};

extern
#ifdef __cplusplus
"C"
#endif
int get_edid_info(const char *filename, struct edid_info* info);


#endif
