#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include "safeget.h"

float get_f32_limited(float min,float max, const char* msg){
    char rbuf[STACK_BUFFER_SIZE];
    char* endptr = NULL;
    float result;

    while(1){
        printf("%s", msg);
        fgets(rbuf,STACK_BUFFER_SIZE,stdin);

        result = strtof(rbuf,&endptr);
        errno = 0;

        if(endptr > rbuf && errno == 0 && 
           result >= min && result <= max) break;
        rbuf[0]='\0';
    }
    return result;
}

double get_f64_limited(double min,double max, const char* msg){
    char rbuf[STACK_BUFFER_SIZE];
    char* endptr = NULL;
    double result;
    while(1){
        printf(msg);

        fgets(rbuf,STACK_BUFFER_SIZE,stdin);
        errno = 0;
        result = strtod(rbuf,&endptr);
        if(endptr > rbuf && errno == 0 && 
           result >= min && result <= max) break;
        rbuf[0]='\0';
    }
    return result;
}


uint64_t get_u64_limited(uint64_t min,uint64_t max, const char* msg){
    char rbuf[STACK_BUFFER_SIZE];
    char* endptr = NULL;
    uint64_t result;
    while(1){
        printf("%s", msg);

        fgets(rbuf,STACK_BUFFER_SIZE,stdin);
        errno = 0;
        result = (uint64_t)strtoull(rbuf,&endptr,10);
        if(endptr > rbuf && errno == 0 && 
           result >= min && result <= max) break;
        rbuf[0]='\0';
    }
    return result;
}

int64_t get_i64_limited(int64_t min, int64_t max, const char* msg){
    char rbuf[STACK_BUFFER_SIZE];
    char* endptr = NULL;
    int64_t result;

    while(1){

        printf("%s", msg);

        fgets(rbuf,STACK_BUFFER_SIZE,stdin);
        errno = 0;
        //se errno continua 0, significa que não houve excessão em nenhuma função da biblioteca padrão
        result = (int64_t) strtoll(rbuf,&endptr,10);
        if(endptr > rbuf && errno == 0 && 
           result >= min && result <= max) break;
        rbuf[0]='\0';    
    }
    return result;
}

#define TAMANHO_BUFFER 256

int64_t get_i64_specif(int64_t* especif, size_t tamanho, const char* msg) {
    char rbuf[TAMANHO_BUFFER];
    char* endptr = NULL;
    int64_t resultado;

    while(1) {
        errno = 0;
        printf("%s", msg);

        fgets(rbuf, TAMANHO_BUFFER, stdin);
        resultado = strtoll(rbuf, &endptr, 10);

        if (endptr == rbuf || errno != 0) {
            continue;
        }

        int find = 0;
        for(size_t i = 0; i < tamanho; i++) {
            if (resultado == especif[i]) {
                find = 1;
                break;
            }
        }
        if (find) break;
    }
    return resultado;
}

float get_uf32_limited(float min,float max, const char* msg){
    char rbuf[STACK_BUFFER_SIZE];
    char* endptr = NULL;
    float result;
    while(1){
        printf("%s", msg);

        fgets(rbuf,STACK_BUFFER_SIZE,stdin);
        result = strtof(rbuf,&endptr);
        errno = 0;
        if(endptr > rbuf && errno == 0 && 
           result >= min && result <= max && result >= 0) break;
        rbuf[0]='\0';
    }
    return result;
}

char* get_str(const char* msg) {
    printf("%s", msg);

    char** ptr = (char**) malloc(sizeof(char*));
    *ptr = (char*) malloc(STACK_BUFFER_SIZE);

    if(*ptr == NULL) {
        puts("memória insuficiente\n");
        exit(EXIT_FAILURE);
    }

    fgets(*ptr, STACK_BUFFER_SIZE, stdin);

   *(*ptr + strcspn(*ptr, "\n")) = '\0';
   
    return *ptr;
}