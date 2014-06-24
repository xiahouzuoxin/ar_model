/*
 * FileName : ar_model.h
 * Author   : xiahouzuoxin @163.com
 * Version  : v1.0
 * Date     : 2014/3/2 20:01:28
 * Brief    : 

 * Copyright (C) USTB
 */

#ifndef _AR_MODEL_H
#define _AR_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Global.h"  

typedef float   TYPE_AR_E;
typedef COMPLEX TYPE_AR;

/* Extern Functions */
extern void Levinson_Durbin(
        // Inputs
        TYPE_AR rx[],
        uint16_t n,
        uint16_t p,
        // Outputs
        TYPE_AR a[],
        TYPE_AR_E *ep,
        uint8_t *err);

extern void ar_psd(
        TYPE_AR a[], 
        uint16_t p, 
        TYPE_AR_E *ep, 
        float psd[], 
        uint16_t n);

extern void pyulear(TYPE_AR x[], int n_x, int p, int n_fft, float psd[]); 

#ifdef __cplusplus
}
#endif  // extern "C"
#endif

