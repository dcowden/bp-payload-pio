#include "util.h"

long map(long x, long in_min, long in_max, long out_min, long out_max){
    return ( x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long constrain_value(long value, long min_value, long max_value){
    if ( value < min_value) return min_value;
    if ( value > max_value) return max_value;
    return value;
}
