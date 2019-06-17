#ifndef __EDID_H
#define __EDID_H


struct edid_info {
  char description[32];
  char manufacturer[16];
  char type[16];
  char model[16];
  char serial_number[32];
};


extern
#ifdef __cplusplus
"C"
#endif
int get_edid_info(const char *filename, struct edid_info* info);



#endif
