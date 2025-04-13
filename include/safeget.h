#ifndef _SAFEGET_H
#define _SAFEGET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>

#define STACK_BUFFER_SIZE 1024
float get_uf32_limited(float min,float max, const char* msg);
int64_t get_i64_limited(int64_t min,int64_t max, const char* msg);
uint64_t get_u64_limited(uint64_t min,uint64_t max, const char* msg);
float  get_f32_limited(float min,float max, const char* msg);
double get_f64_limited(double min,double max, const char* msg);
int64_t get_i64_specif(int64_t* especif, size_t tamanho, const char* msg);
char* get_str(const char* msg);

static inline float get_uf32(const char* msg){
    return(get_uf32_limited(0, FLT_MAX, msg));
}

static inline uint8_t get_u8(const char* msg){
    return (uint8_t)get_u64_limited(0,UINT8_MAX, msg);
}

static inline uint16_t get_u16(const char* msg){
    return (uint16_t)get_u64_limited(0,UINT16_MAX, msg);
}

static inline uint32_t get_u32(const char* msg){
    return (uint32_t)get_u64_limited(0,UINT32_MAX, msg);
}

static inline uint64_t get_u64(const char* msg){
    return get_u64_limited(0,UINT64_MAX, msg);
}

static inline int8_t get_i8(const char* msg){
    return (int8_t)get_i64_limited(INT8_MIN, INT8_MAX, msg);
}

static inline int16_t get_i16(const char* msg){
    return (int16_t)get_i64_limited(INT16_MIN,INT16_MAX, msg);
}

static inline int32_t get_i32(const char* msg){
    return (int32_t)get_i64_limited(INT32_MIN,INT32_MAX, msg);
}
static inline int64_t get_i64(const char* msg){
    return get_i64_limited(INT64_MIN,INT64_MAX, msg);
}

static inline float get_f32(const char* msg){
    return get_f32_limited(-FLT_MAX,FLT_MAX, msg);
}

static inline double get_f64(const char* msg){
    return get_f64_limited(-DBL_MAX,DBL_MAX, msg);
}

#define input(T, msg)_Generic(T, \
    int8_t: get_i8,              \
    int16_t: get_i16,            \
    int32_t: get_i32,            \
    int64_t: get_i64,            \
    uint8_t: get_u8,             \
    uint16_t: get_u16,           \
    uint32_t: get_u32,           \
    uint64_t: get_u64,           \
    float: get_f32,              \
    double: get_f64,             \
    char*: get_str,              \
    char : get_ch)(msg)          

#endif