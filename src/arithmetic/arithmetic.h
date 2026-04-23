#ifndef SRC_S21_DECIMAL_ARITHMETIC_ARITHMETIC_H_
#define SRC_S21_DECIMAL_ARITHMETIC_ARITHMETIC_H_


#include "../types/types.h"


int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

int s21_div(s21_decimal dividend, s21_decimal divisor, s21_decimal *result);

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// ############################################################### HELP ##############################################################
int add_positive(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int sub_positive(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int mul_positive(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int div_positive(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// ############################################################### ARITHMETIC HELP BIG DECIMAL##############################################################
int add_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2, s21_big_decimal *result);
void mul_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2, s21_big_decimal *result);
void sub_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2, s21_big_decimal *result);
s21_big_decimal div_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2,
    s21_big_decimal *result);
int count_numbers(s21_big_decimal big_decimal, s21_big_decimal ten);
void div_fractional_part(s21_big_decimal *big_result, s21_big_decimal *temp_big, s21_big_decimal *zero, s21_big_decimal ten, s21_big_decimal big_value_2, int *degree, int count);
int equal_numbers(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void extra_zeros_big_decimal(s21_big_decimal *big_result, s21_big_decimal *temp_big, s21_big_decimal ten, s21_big_decimal zero, int *degree);
void rounding_big_decimal(s21_big_decimal *big_result, s21_big_decimal *temp_big,
    s21_big_decimal zero, s21_big_decimal one, s21_big_decimal five,
    s21_big_decimal ten, s21_big_decimal big_value_2, int *degree, int count);

#endif 