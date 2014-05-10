#ifndef __EDID_DECODE_H
#define __EDID_DECODE_H


struct edid_info {
  char manufacturer[64];
  int model;
  int serial_number;  
};

extern
#ifdef __cplusplus
"C"
#endif
int get_edid_info(const char *filename, struct edid_info* info);


#endif