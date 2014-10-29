/*
 * FileName : ar_model.c
 * Author   : xiahouzuoxin @163.com
 * Version  : v1.0
 * Date     : 2014/3/2 20:14:11
 * Brief    : 

 * Copyright (C) USTB
 */
 
#include "ar_model.h"
#include "tiny_mm.h"
#include "zx_fft.h"

/*
 * @brief   
 *   To determine the autoregressive coefficients by solving 
 *   Yule-Walker equation with Levinson algorithm. 
 * @inputs  
 *   rx[] :auto-correlation
 *   n    :dimension of rx[]
 *   p    :order of AR model
 * @outputs 
 *   a[]  :Array of complex autoregressive coefficients, a(0) to a(p-1), total p
 *   ep   :Driving noise variance (real)
 *   err  :*err=0 no errors, else ep<=0
 * @retval  
 *
 * @test log
 * 2014.06.24 Test for real rx[] input ok.
 */
uint16_t Levinson_Durbin(
        // Inputs
        TYPE_AR rx[],
        uint16_t n,
        uint16_t p,
        // Outputs
        TYPE_AR a[],
        TYPE_AR_E *ep,
        uint8_t *err)
{
    float rx0 = 0.0f;
    float tmp_ep = 0.0f;
    int16_t i = 0;
    int16_t k = 0;
    TYPE_AR sum;
    COMPLEX *p_part0 = NULL;

    a[0].real = 1.0f;
    a[0].imag = 0.0f;
    rx0 = rx[0].real;
    a[1].real = -rx[1].real/rx0;
    a[1].imag = -rx[1].imag/rx0;
    *ep = rx0 * (1.0f - (a[1].real*a[1].real+a[1].imag*a[1].imag) );   

    /* block size of g_mem_part0 should be big enough */
    //p_part0 = (COMPLEX *)OSMemGet(g_mem_part0, err);
    
    p_part0 = (COMPLEX *)get_recg_buf(&recg_buf, p*sizeof(COMPLEX));
    if (!p_part0) {
        printf("ar_psd error.\n");
        return k;
    }

    for (k=2; k<p; k++) {
        sum.real = 0.0f;
        sum.imag = 0.0f;
        for (i=1; i<k; i++) {
            sum.real += rx[k-i].real*a[i].real - rx[k-i].imag*a[i].imag;
            sum.imag += rx[k-i].real*a[i].imag + rx[k-i].imag*a[i].real;
        }
        sum.real += rx[k].real;
        sum.imag += rx[k].imag;
        // Get a[k]
        a[k].real = -sum.real/(*ep);
        a[k].imag = -sum.imag/(*ep);
        // Next *ep
		tmp_ep = 1.0f - (a[k].real*a[k].real+a[k].imag*a[k].imag);
		if (tmp_ep <= 0.0f) {
            return k;
		} else {
            (*ep) *= tmp_ep;
		}
        //(*ep) *= (1.0f - (a[k].real*a[k].real+a[k].imag*a[k].imag) );
        //if (*ep<=0.0f) {
        //    return k;
		//}
        // Recalculate a[1] ~ a[k-1], will 
        for (i=1; i<k; i++) {
            p_part0[i].real = a[i].real + a[k].real*a[k-i].real + a[k].imag*a[k-i].imag;
            p_part0[i].imag = a[i].imag + a[k].imag*a[k-i].real - a[k].real*a[k-i].imag;
        }
        for (i=1; i<k; i++) {
            a[i].real = p_part0[i].real;
            a[i].imag = p_part0[i].imag;
        }
    }

    //OSMemPut(g_mem_part0, p_part0);
    put_recg_buf(&recg_buf, p*sizeof(COMPLEX));

    *err = 0;

	return k;
}


/*
 * @brief   To compute the power spectum by AR-model parameters.
 * @inputs  
 *   a[]   Complex array of AR  parameters a(0) to a(p-1)
 *   p     AR model order (integer)
 *   n     Sample numbers of power spectum.
 *   ep    White noise variance of model input (real)
 * @outputs 
 *   psd   Power spectum
 *   err  :*err=0 no errors
 * @retval  
 *
 * @test log
 * 2014.06.24 Test for real rx[] input ok.
 */
void ar_psd(TYPE_AR a[], uint16_t p, TYPE_AR_E *ep, float psd[], uint16_t n)
{
    COMPLEX *p_part0 = NULL;
    int k = 0;
    float power = 0;
//    uint8_t err = 0;

    /* block size of g_mem_part0 should be big enough */
    //p_part0 = OSMemGet(g_mem_part0, &err);
    p_part0 = (COMPLEX *)get_recg_buf(&recg_buf, p*sizeof(COMPLEX));
    if (!p_part0) {
        printf("ar_psd error.\n");
        return;
    }

    for (k=0; k<p; k++) {
        p_part0[k].real = a[k].real;
        p_part0[k].imag = a[k].imag;
    }
    for (k=p; k<n; k++) {  /* Padding zeros */ 
        p_part0[k].real = 0.0f;
        p_part0[k].imag = 0.0f;
    }

    fft(p_part0, n);

    for (k=0; k<n; k++) {
        power = p_part0[k].real*p_part0[k].real + p_part0[k].imag*p_part0[k].imag;
        psd[k]= (*ep) / power; 
    }

    //OSMemPut(g_mem_part0, p_part0);
    put_recg_buf(&recg_buf, p*sizeof(COMPLEX));
}

/*
 * @brief   
 * @inputs  
 * @outputs 
 * @retval  
 */
void pyulear_corr(TYPE_AR x_corr[], int n_x, int p, int n_fft, float psd[])
{
    float ep = 0;
    uint8_t err = 0;
    TYPE_AR *a_coeff = NULL;

    /* block size of g_mem_part0 should big enough */
    // a_coeff = OSMemGet(g_mem_part0, &err);
    a_coeff = (TYPE_AR *)get_recg_buf(&recg_buf, p*sizeof(TYPE_AR));
    if (!a_coeff) {
        printf("alloc a_coeff error.\n");
        return;
    }
    Levinson_Durbin(x_corr, n_x, p, a_coeff, &ep, &err); 
    ar_psd(a_coeff, p, &ep, psd, n_fft);

    // OSMemPut(g_mem_part0, a_coeff);
    put_recg_buf(&recg_buf, p*sizeof(TYPE_AR));
}

/*
 * @brief   
 * @inputs  
 * @outputs 
 * @retval  
 */
void pyulear(COMPLEX x[], int n_x, int p, int n_fft, float psd[])
{
    float ep = 0;
    uint8_t err = 0;
    TYPE_AR *a_coeff = NULL;
	int  i = 0;
	uint16_t k = 0; 

#if 0
	FILE *fp = NULL;

	fp = fopen("fft.txt", "w+");
	if (!fp) {
	    printf("open fft.txt error.\n");
		return;
	}
#endif 
    /* auto-correaltion */
    fft_real(x, n_x);	
    for (i=0; i<n_x; i++) {
        x[i].real = x[i].real * x[i].real + x[i].imag * x[i].imag;
        x[i].imag = 0;
    }
    ifft_real(x, n_x);
    
	//zx_xcorrel(x, x, n_x, n_x*2, 0);
#if 0
	for (i=0; i<n_x; i++) {
	    fprintf(fp, "%.4f\n", x[i].real);
	}
	fclose(fp);
#endif

    a_coeff = (TYPE_AR *)get_recg_buf(&recg_buf, p*sizeof(TYPE_AR));
    if (!a_coeff) {
        printf("alloc a_coeff error.\n");
        return;
    }
    k = Levinson_Durbin(x, n_x, p, a_coeff, &ep, &err); 
	for (; k<p; k++) {
        a_coeff[k].real = 0.0f;
		a_coeff[k].imag = 0.0f;
	}
    ar_psd(a_coeff, p, &ep, psd, n_fft);

    put_recg_buf(&recg_buf, p*sizeof(TYPE_AR));
}

