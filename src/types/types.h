#ifndef SRC_S21_DECIMAL_TYPES_H_
#define SRC_S21_DECIMAL_TYPES_H_

#include <stdint.h>
#define SIZE_DECIMAL 4
#define SIZE_DECIMAL_BIG 8
#define SIZE_DEC 96
#define SIZE_DEC_BIG 224
#define TRUE 1
#define FALSE 0
#define MINUS 1
#define PLUS 0
#define SIZE_INT sizeof(int) * 8

typedef struct {
    unsigned int bits[4];
} s21_decimal;

typedef union {
    unsigned int value;
    struct {
        unsigned int not_used_1 : 16; //  0 - 15 Не используется
        unsigned int degree: 8; // 16 - 23 Биточки для степени
        unsigned int not_used_2 : 7 ; // 24 - 30 Не используется
        unsigned int sign : 1; // 31 Биточек для знака
    };
    
} bit3_struct;

typedef struct {
    unsigned int bits[8];
} s21_big_decimal;

#endif