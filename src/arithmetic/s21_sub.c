#include "../big_decimal/big_decimal.h"
#include "../comparison/comparison.h"
#include "../helpers/helpers.h"
#include "../other/other.h"
#include "arithmetic.h"
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error_flag = 0;
  s21_decimal_init(result);
  if (!result || check_decimal(value_1) || check_decimal(value_2)) {
    error_flag = 4;
  } else {
    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);
    if (sign1 == 0 && sign2 == 0) {
      if (s21_is_greater_or_equal(value_1, value_2)) {
        error_flag = sub_positive(value_1, value_2, result);
      } else {
        error_flag = sub_positive(value_2, value_1, result);
        s21_negate(*result, result);
      }
    } else if (sign1 == 0 && sign2 == 1) {
      error_flag = s21_add(value_1, s21_abs(value_2), result);
    } else if (sign1 == 1 && sign2 == 0) {
      error_flag = s21_add(s21_abs(value_1), value_2, result);
      s21_negate(*result, result);
    } else if (sign1 == 1 && sign2 == 1) {
      if (s21_is_greater_or_equal(value_1, value_2)) {
        error_flag = sub_positive(s21_abs(value_2), s21_abs(value_1), result);
      } else {
        error_flag = sub_positive(s21_abs(value_1), s21_abs(value_2), result);
        s21_negate(*result, result);
      }
    }
    if (null_decimal(*result) && get_sign(value_1) == get_sign(value_2)) {
      set_sign(result, get_sign(value_1));
      // }else if (null_decimal(*result)){
      //   set_sign(result,0);
    }  // костыли

    if (error_flag == 1 && get_sign(*result) == 1) {
      error_flag = 2;
    }
  }

  return error_flag;
}

int sub_positive(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  int error_flag = 0;
  // normalization(&value_1, &value_2);
  // int degree = get_degree(value_1);

  s21_big_decimal value_1l;
  decimal_to_big_decimal(&value_1, &value_1l);
  s21_big_decimal value_2l;
  decimal_to_big_decimal(&value_2, &value_2l);
  s21_big_decimal resultl;
  s21_decimal_init_big(&resultl);
  normalization_big(&value_1l, &value_2l);
  sub_big_decimal(value_1l, value_2l, &resultl);
  error_flag = big_decimal_to_decimal(&resultl, result);
  return error_flag;
}

void sub_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2,
                     s21_big_decimal *result) {
  unsigned temp = 0;
  for (int i = 0; i < 32 * 7; ++i) {
    unsigned result_bit =
        get_bit_big(value_1, i) - get_bit_big(value_2, i) - temp;
    if (result_bit > 1) {
      result_bit += 2;
      temp = 1;
    } else {
      temp = 0;
    }

    set_bit_big(result, i, result_bit);
  }
  set_sign_big(result, get_sign_big(value_1));
  set_degree_big(result, get_degree_big(value_1));
}
