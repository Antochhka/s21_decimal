#ifndef SRC_S21_DECIMAL_BIGDECIMAL_BIGDECIMAL_H_
#define SRC_S21_DECIMAL_BIGDECIMAL_BIGDECIMAL_H_


#include "../types/types.h"

int big_decimal_to_decimal(s21_big_decimal *src, s21_decimal *dest);
void decimal_to_big_decimal(s21_decimal *src, s21_big_decimal *dest);

// Возвращает степень
int get_degree_big(s21_big_decimal decimal);
// Меняет степень
void set_degree_big(s21_big_decimal *decimal,int degree);
// Возвращет знак
int get_sign_big(s21_big_decimal decimal);
// Меняет знак
void set_sign_big(s21_big_decimal *num,int sign_value);
// Возвращет бит по индексу
int get_bit_big(s21_big_decimal value,int i);
// Меняет бит по индексу
void set_bit_big(s21_big_decimal *result,int i, int result_bit);

// Инцилизирует число
void s21_decimal_init_big(s21_big_decimal *dec);

// Сдвиг 
void left_shift_big(s21_big_decimal *decimal, int shift);

// Максимальный бит в числе
int get_not_zero_bit(s21_big_decimal decimal);

void divide_by_10_big(s21_big_decimal *num, int  index);

// Равно
int s21_is_equal_big(s21_big_decimal value_1, s21_big_decimal value_2);

// Больше
int s21_is_greater_big(s21_big_decimal value_1, s21_big_decimal value_2);

// Больше или равно
int s21_is_greater_or_equal_big(s21_big_decimal value_1, s21_big_decimal value_2);

// Нормализация
void normalization_big(s21_big_decimal *value_1, s21_big_decimal *value_2);

//находит старший бит числа
int first_significant_bit_big(s21_big_decimal dec);
//проверка влезает ли big_decimal в decimal
 int is_big_decimal_zero(s21_big_decimal src);
#endif 