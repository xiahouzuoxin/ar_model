/*
 * FileName : User\test_mem_alloc.c
 * Author   : xiahouzuoxin @163.com
 * Version  : v1.0
 * Date     : 2014/3/2 22:23:13
 * Brief    : 

 * Copyright (C) USTB
 */
#include "Config.h" 
#include "mem_alloc.h"
#include "ar_model.h"
#include "time.h"  

#include "wave.h"

#include "testcase.h"

OS_MEM *g_mem_part0;
uint8_t partition0[8][8192];
uint8_t err;
uint8_t *pblk;

#define N             (1024)  
#define ORDER         (256)  
#define N_FFT         (256)
COMPLEX x[N*2];  
COMPLEX a[ORDER]; 
float psd[N_FFT];
  
/* 
 * generate random number which satify guass distribution 
 */  
double guass_rand(void)  
{  
    static double V1, V2, S;  
    static int phase = 0;  
    double X;  
  
    if ( phase == 0 ) {  
        do {  
            double U1 = (double)rand() / RAND_MAX;  
            double U2 = (double)rand() / RAND_MAX;  
  
            V1 = 2 * U1 - 1;  
            V2 = 2 * U2 - 1;  
            S = V1 * V1 + V2 * V2;  
        } while(S >= 1 || S == 0);  
  
        X = V1 * sqrt(-2 * log(S) / S);  
    } else {  
        X = V2 * sqrt(-2 * log(S) / S);  
    }  
  
    phase = 1 - phase;  
  
    return X;  
}  

int main(void)
{
    int i = 0;
    uint8_t err;
    float ep = 0;
    static uint32_t *pwave = NULL;
    FILE *fp = NULL;

    OS_MemInit();
    g_mem_part0 = OSMemCreate(partition0, 8, 8192, &err);




    /*
     * Print arguments
     */
    printf("%d\n", N);   // size of input signal
    printf("%d\n", ORDER);  // order of AR model coeff
    printf("%d\n", N_FFT);  // Calculate FFT nodes in PSD estmate

    /* 
     * generate x[N] 
     */  
#if 1
    srand(time(NULL));  
    for (i=0; i<N; i++) {  
        x[i].real = sin(10*2*PI*i/N)+guass_rand();  
        x[i].imag = 0;  
        printf("%.4f\n", x[i].real);
    }     
#else
    for (i=0; i<N; i++) {
        x[i].real = testcase[i];
        x[i].imag = 0.0f;
        printf("%.4f\n", x[i].real);
    }
#endif
    
    pyulear(x, N, ORDER, N_FFT, psd);
    for (i=0; i<N_FFT; i++) {
        printf("%.4f\n", psd[i]);
    }

    return 0;
}

