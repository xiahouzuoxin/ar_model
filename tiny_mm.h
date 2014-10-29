/*
 * FileName : E:\MyDesigner\dsp\dsp_sound_v2\User\recognition.h
 * Author   : xiahouzuoxin @163.com
 * Version  : v1.0
 * Date     : 2014/7/12 17:35:20
 * Brief    : 
 * 
 * Copyright (C) MICL,USTB
 */
#ifndef _RECOGNITION_H 
#define _RECOGNITION_H 

#ifdef __cplusplus
  extern "C" {
#endif

#include "Config.h"

struct st_recg_buf {
    uint8_t *buf;
    uint32_t cur_len;
    uint32_t max_len;
};

extern void recg_buf_arrange(struct st_recg_buf *recg_buf, uint8_t *buf, uint32_t n_bytes);
extern void *get_recg_buf(struct st_recg_buf *recg_buf, int n_bytes);
extern void put_recg_buf(struct st_recg_buf *recg_buf, int n_bytes);

extern struct st_recg_buf    recg_buf;

#ifdef __cplusplus
  }
#endif  /* extern "C" */
#endif

