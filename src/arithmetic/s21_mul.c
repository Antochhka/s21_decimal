#include "../big_decimal/big_decimal.h"
#include "../comparison/comparison.h"
#include "../helpers/helpers.h"
#include "../other/other.h"
#include "arithmetic.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error_flag = 0;
  s21_decimal_init(result);
  if (!result || check_decimal(value_1) || check_decimal(value_2)) {
    error_flag = 4;
  } else {
    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);
    if (sign1 == 0 && sign2 == 0) {
      error_flag = mul_positive(value_1, value_2, result);
    } else if (sign1 == 0 && sign2 == 1) {
      error_flag = mul_positive(value_1, s21_abs(value_2), result);
      s21_negate(*result, result);
    } else if (sign1 == 1 && sign2 == 0) {
      error_flag = mul_positive(s21_abs(value_1), value_2, result);
      s21_negate(*result, result);
    } else if (sign1 == 1 && sign2 == 1) {
      error_flag = mul_positive(s21_abs(value_1), s21_abs(value_2), result);
      // s21_negate(*result, result);
    }
    if (error_flag == 1 && get_sign(*result) == 1) {
      error_flag = 2;
    }
    if (null_decimal(*result)) {
      set_degree(result, 0);
      set_sign(result, get_degree(value_2));
    }
    if (null_decimal(value_2)) {
      set_degree(result, 0);
      set_sign(result, get_degree(value_2));
    }
    if (error_flag == 0 && !null_decimal(value_1) && !null_decimal(value_2) &&
        null_decimal(*result)) {
      error_flag = 2;
    }
  }
  return error_flag;
}

int mul_positive(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  int error_flag = 0;
  s21_big_decimal value_1l;
  decimal_to_big_decimal(&value_1, &value_1l);
  s21_big_decimal value_2l;
  decimal_to_big_decimal(&value_2, &value_2l);
  s21_big_decimal resultl;
  s21_decimal_init_big(&resultl);

  mul_big_decimal(value_1l, value_2l, &resultl);
  error_flag = big_decimal_to_decimal(&resultl, result);
  return error_flag;
}

void mul_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2,
                     s21_big_decimal *result) {
  set_degree_big(result, get_degree_big(value_1) + get_degree_big(value_2));
  s21_big_decimal temp_big = value_1;
  for (int i = 0; i <= get_not_zero_bit(value_2); i++) {
    if (get_bit_big(value_2, i) != 0) {
      add_big_decimal(*result, temp_big, result);
    }
    left_shift_big(&temp_big, 1);
  }
}
