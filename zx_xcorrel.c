/*
 * FileName : zx_xcorrel.c
 * Author   : xiahouzuoxin
 * Date     : 2013.11.24
 * Version  : v1.0
 * Brief    :  
 */
#include "zx_xcorrel.h"

/*
 *
 */
static inline void adjust_xcorrel_output(COMPLEX x[], uint32_t n)
{
    float       swap;
    uint32_t    i = 0;

    n = (n+1) >> 1;
    for (i=0; i<n; i++) {
        swap        = x[i].real;
        x[i].real   = x[i+n].real;
        x[i+n].real = swap; 
    }
}

/*
 * @brief  To estimate the biased cross-correlation function
 *   of TYPE_CORREL arrays x and y. 
 *   the result will store in x, size of x must be >=2*m
 * @input params 
     x : n dimensioned TYPE_CORREL array. 
     y : n dimensioned TYPE_CORREL array.
     m : the dimension of x and y.    
     n : point numbers of correlation.
     icorrel: icorrel=1, cross-correlation; icorrel=0, auto-correlation
 * @retval None
 *
 * ====
 * TEST OK 2013.01.14
 */
void zx_xcorrel(TYPE_CORREL x[], TYPE_CORREL y[], int m, int n, int icorrel)
{
    int s,k;
    TYPE_CORREL z;

//     assert_param(n >= 2*m);

    /* n must be power of 2 */
    s = n;
    do {
        s = s >> 1;
        k = s - 2;
    } while (k > 0);
    if (k<0) return;

	/* Padding 0 */
	for (k=m; k<n; k++) {
        x[k].real = 0.;
		x[k].imag = 0.0f;
	}
    fft(x, n);
      
    if (1 == icorrel) {  
	    /* Padding 0 */
		for (k=m; k<n; k++) {
            y[k].real = 0.;
			y[k].imag = 0.0f;
		}
		fft(y, n);

        /* conjuction */
        for (k=0; k<n; k++) {
            z.real = x[k].real; 
            z.imag = x[k].imag;
            x[k].real = (z.real*y[k].real + z.imag*y[k].imag)/(float)m;
            x[k].imag = (z.real*y[k].imag - z.imag*y[k].real)/(float)m;
        } 
    } else {
        for (k=0; k<n; k++) {
            x[k].real = (x[k].real*x[k].real+x[k].imag*x[k].imag) / (float)(m);
			x[k].imag = 0.0f;
		}
	}

    ifft(x, n);

    /*
     * If input is real
     */
    for (k=0; k<n; k++) {
        x[k].imag = 0;
    }

//    adjust_xcorrel_output(x, n); 

	return;   
}

/*
 * @brief   Get the delay between two signal
 * @input params 
 *   x  :n dimensioned TYPE_CORREL array. 
 *   y  :n dimensioned TYPE_CORREL array.
 *   n  :the dimension of x and y.
 *   freq:sample rate in Hz.
 * @output params 
 * @retval
 *   delay: delay time in unit ms, may be positive or negtive.
 *          positive means x is earlier than y, and negtive means x later than y.
 *
 * ====
 * TEST OK 2013.01.14
 */
float get_signal_delay(TYPE_CORREL x[], TYPE_CORREL y[], int n, uint32_t freq)
{
    int32_t delay = 0;
	int i = 0;
    int m = 0;
 	TYPE_CORREL_E maxval = 0;
	TYPE_CORREL_E temp = 0;

    /* cross-correlation */
    m = n >> 1;
    zx_xcorrel(x, y, m, n, 1);   

    /* find max value in Rxy */
	for (i=0; i<m; i++) {
	    temp = sqrt(x[i].real*x[i].real+x[i].imag*x[i].imag);
        if (temp > maxval) {
            maxval = temp;
            delay = i;
		}
	}  

    delay = (float)delay * 1000 / freq;  /* unit of delay is ms */

	return delay;
}

/* End of file */

