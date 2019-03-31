#ifndef HEADERs_H
#define HEADERs_H

#define byte uint8


#include "param.h"
#include "typedef.h"



extern inline int16 Abs(int16 input) {
    return input >= 0 ? input : -input;
}

extern inline int16 Min(int16 a, int16 b) {
    return a > b ? b : a;
}

extern inline int16 Max(int16 a, int16 b) {
    return a > b ? a : b;
}

extern inline float Min_f(float a, float b) {
    return a > b ? b : a;
}

extern inline float Max_f(float a, float b) {
    return a > b ? a : b;
}

extern inline bool OpstSign(int16 a, int16 b) {
    return (a ^ b) & 0x8000;
}

extern inline bool InRange(int16 value, int16 lbound, int16 hbound) {
    return value > lbound && value < hbound;
}

extern inline bool InRange_f(float value, float lbound, float hbound) {
    return value > lbound && value < hbound;
}

extern inline int16 Limit(int16 value, int16 lbound, int16 hbound) {
    return value > hbound ? hbound : value < lbound ? lbound : value;
}


extern inline int16 LimitH(int16 value, int16 hbound) {
    return value > hbound ? hbound : value;
}

extern inline int16 LimitL(int16 value, int16 lbound) {
    return value < lbound ? lbound : value;
}

extern inline float Limit_f(float value, float lbound, float hbound) {
    return value > hbound ? hbound : value < lbound ? lbound : value;
}




#endif // HEADER_H
