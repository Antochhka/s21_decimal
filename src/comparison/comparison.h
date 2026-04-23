#ifndef SRC_S21_DECIMAL_COMPARISON_COMPARISON_H_
#define SRC_S21_DECIMAL_COMPARISON_COMPARISON_H_

#include "../types/types.h"

// Равно
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);

// Неравно
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

// Меньше
int s21_is_less(s21_decimal value_1, s21_decimal value_2);

// Меньше или равно
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);

// Больше
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);

// Больше или равно
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
#endif