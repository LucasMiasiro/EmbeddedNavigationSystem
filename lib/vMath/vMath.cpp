#include "vMath.h"
#include "config.h"
#include <math.h> 
#include "esp_dsp.h"


void quat2Euler(float *q, float *euler_out, const float magDeclination){
    //Phi
    euler_out[0] = atan2f(q[2]*q[3] + q[0]*q[1], 0.5 - (q[1]*q[1] + q[2]*q[2]));
    //Theta
    euler_out[1] = asinf(2*(q[0]*q[2] - q[1]*q[3]));
    //Psi
    /* euler_out[2] = atan2f(q[1]*q[2] + q[0]*q[3], 0.5 - (q[2]*q[2] + q[3]*q[3])); */
    euler_out[2] = atan2f(q[1]*q[2] + q[0]*q[3], 0.5 - (q[2]*q[2] + q[3]*q[3])) + magDeclination;
    if (euler_out[2] < -PI) {
        euler_out[2] += 2*PI;
    } else if (euler_out[2] > PI) {
        euler_out[2] -= 2*PI;
    }
}

void inv2(float M[2][2]){
    float det, M_00;
    M_00 = M[0][0];
    det = M[0][0]*M[1][1] - M[1][0]*M[0][1];
    M[0][0] = M[1][1]/det;
    M[1][0] = -M[1][0]/det;
    M[0][1] = -M[0][1]/det;
    M[1][1] = M_00/det;
}

void normalize(float *in, int len){
    dsps_mulc_f32_ae32(in, in, len, 1/norm2(in, len), 1, 1);
}

void normalizeQuat(float *q){
    quatProdConst(q, 1/norm2(q, 4), q);
}

float norm2(float *in, const int len){
    float norm;
    dsps_dotprod_f32_ae32(in, in, &norm, len);
    return norm = sqrt(norm);
}

void quatConj(float *in, float *out){
    out[0] = in[0];
    out[1] = -in[1];
    out[2] = -in[2];
    out[3] = -in[3];
}

void matProd_3x3x1(float *in1, float *in2, float *out){
    dspm_mult_f32_ae32(in1, in2, out, 3, 3, 1);
}

void matProd_4x6x1(float *in1, float *in2, float *out){
    dspm_mult_f32_ae32(in1, in2, out, 4, 6, 1);
}

void matProd_2x2x2(float *in1, float *in2, float *out){
    dspm_mult_f32_ae32(in1, in2, out, 2, 2, 2);
}

void matProd_2x2x1(float *in1, float *in2, float *out){
    dspm_mult_f32_ae32(in1, in2, out, 2, 2, 1);
}

void quatAdd(float *in1, float *in2, float *out){
    dsps_add_f32_ae32(in1, in2, out, 4, 1, 1, 1);
}

void quatProd(float *a, float *b, float *out){
    out[0] = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
    out[1] = a[0]*b[1] + a[1]*b[0] + a[2]*b[3] - a[3]*b[2];
    out[2] = a[0]*b[2] + a[2]*b[0] + a[3]*b[1] - a[1]*b[3];
    out[3] = a[0]*b[3] + a[3]*b[0] + a[1]*b[2] - a[2]*b[1];
}

void quatProdConst(float *in, float K, float *out){
    dsps_mulc_f32_ae32(in, out, 4, K, 1, 1);
}

float sq(float in){
    return in*in;
}
