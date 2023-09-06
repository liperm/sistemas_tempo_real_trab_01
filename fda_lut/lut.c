#include "lut.h"
#include <math.h>

float LUT[256];
float dco (float alphaMinusBeta, float lambda){
    float result = 0.0, temp_result = 0.0;

    temp_result = alphaMinusBeta;
    temp_result = pow(temp_result,0.2);
    temp_result = temp_result/lambda;
    if(temp_result != 0.0){
        temp_result = -(temp_result/5.0);
    }
    result = exp(temp_result);

    return (result);
}


void generateLut(float lambda){
    for(int i = 0; i < 256; i++){
        LUT[i] = (1.0 - exp(-8.0 * 0.5 * dco((float)i, lambda))) / 8.0;
    }
}
