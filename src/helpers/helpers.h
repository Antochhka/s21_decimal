#ifndef SRC_S21_DECIMAL_HELPERS_HELPERS_H_
#define SRC_S21_DECIMAL_HELPERS_HELPERS_H_

#include "../types/types.h"
#include "stdio.h"
#include <stdlib.h>

//Выводит все число
void print_bits_big(int value, int is_bits_3);
void print_decimal_big(s21_big_decimal value);

//Меняет весь Децимал
void set_decimal(s21_decimal *decimal, int sign, int degree, unsigned int mantissa[3]);

//Выводит все число
void print_decimal(s21_decimal value);
void print_bits(int value, int is_bits_3);

//Возвращает степень
int get_degree(s21_decimal decimal);

//Меняет степень
void set_degree(s21_decimal *decimal,int degree);

//Возвращет знак
int get_sign(s21_decimal decimal);

//Меняет знак
void set_sign(s21_decimal *num,int sign_value);

//Возвращет бит по индексу
int get_bit(s21_decimal value,int i);

//Меняет бит по индексу
void set_bit(s21_decimal *result,int i, int result_bit);

//модуль числа
s21_decimal s21_abs(s21_decimal value);

// Смещение влево
void left_shift(s21_decimal *decimal, int shift);

// Удаляет незначимые нули
s21_decimal remove_trailing_zeros(s21_decimal value);

void stupid_divide_by_10(s21_decimal *num);

void get_remainder_by_10_to_the_power(s21_decimal value_1, s21_decimal value_2,
    s21_decimal *remainder,
    int power_of_ten);

// Проверяет, является ли число нулем
int is_zero(s21_decimal value);

// Нормализация
void normalization(s21_decimal *value_1, s21_decimal *value_2);

//иницилизирует число
void s21_decimal_init(s21_decimal *dec);

//находит старший бит числа
int first_significant_bit(s21_decimal dec);

//деление на 10
void divide_by_10_for_dec(s21_decimal *num, int index);

//Проверка на корректный формат числа
int check_decimal(s21_decimal decimal);

//Проверка что decimal == 0
int null_decimal(s21_decimal value);


#endif