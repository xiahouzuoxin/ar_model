/*
 * FileName : zx_xcorrel.h
 * Author   : xiahouzuoxin
 * Date     : 2013.11.24
 * Version  : v1.0
 * Brief    :  
 */
#ifndef _ZX_XCORREL_H
#define _ZX_XCORREL_H

#ifdef __cpulsplus
extern "C" {
#endif

#include "Global.h"

typedef float TYPE_CORREL_E;    

typedef COMPLEX TYPE_CORREL;

extern void zx_xcorrel(TYPE_CORREL x[], TYPE_CORREL y[], int n, int lag, int icorrel);
extern float get_signal_delay(TYPE_CORREL x[], TYPE_CORREL y[], int n, uint32_t freq);

#ifdef __cplusplus
}
#endif

#endif

