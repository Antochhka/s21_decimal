#include "../big_decimal/big_decimal.h"
#include "../comparison/comparison.h"
#include "../helpers/helpers.h"
#include "../other/other.h"
#include "arithmetic.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error_flag = 0;
  s21_decimal_init(result);
  if (!result || check_decimal(value_1) || check_decimal(value_2)) {
    error_flag = 4;
  } else {
    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);
    if (sign1 == 0 && sign2 == 0) {
      error_flag = add_positive(value_1, value_2, result);
    } else if (sign1 == 0 && sign2 == 1) {
      error_flag = s21_sub(value_1, s21_abs(value_2), result);
    } else if (sign1 == 1 && sign2 == 0) {
      error_flag = s21_sub(s21_abs(value_1), value_2, result);
      s21_negate(*result, result);
    } else if (sign1 == 1 && sign2 == 1) {
      error_flag = add_positive(s21_abs(value_1), s21_abs(value_2), result);
      s21_negate(*result, result);
    }
    if (error_flag == 1 && get_sign(*result) == 1) {
      error_flag = 2;
    }
    // Если оба числа нулевые
    if (null_decimal(value_1) && null_decimal(value_2)) {
      // Если степень value_1 не равна нулю, устанавливаем знак value_1
      if (get_degree(value_1) != 0) {
        set_sign(result, get_sign(value_1));
      }
      // Если степень value_2 не равна нулю, устанавливаем знак value_2
      else if (get_degree(value_2) != 0) {
        set_sign(result, get_sign(value_2));
      }
      // Если знак value_1 равен 0, а value_2 равен 1, устанавливаем знак
      // value_2
      else {
        set_sign(result, get_sign(value_1));
      }
    }
    // Если результат нулевой и знаки value_1 и value_2 не равны, устанавливаем
    // знак value_1
    else if (null_decimal(*result) && get_sign(value_1) != get_sign(value_2)) {
      set_sign(result, get_sign(value_1));
    }
    // Если результат нулевой, устанавливаем знак 0
    else if (null_decimal(*result)) {
      set_sign(result, 0);
    }
  }
  return error_flag;
}

int add_positive(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  int error_flag = 0;
  // normalization(&value_1, &value_2);

  s21_big_decimal value_1l;
  decimal_to_big_decimal(&value_1, &value_1l);
  s21_big_decimal value_2l;
  decimal_to_big_decimal(&value_2, &value_2l);
  s21_big_decimal resultl;
  s21_decimal_init_big(&resultl);
  normalization_big(&value_1l, &value_2l);
  add_big_decimal(value_1l, value_2l, &resultl);
  error_flag = big_decimal_to_decimal(&resultl, result);
  return error_flag;
}

int add_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2,
                    s21_big_decimal *result) {
  int error_flag = 0;
  unsigned temp = 0;
  for (int i = 0; i < 32 * 7; ++i) {
    unsigned result_bit =
        get_bit_big(value_1, i) + get_bit_big(value_2, i) + temp;
    temp = result_bit / 2;
    result_bit %= 2;
    set_bit_big(result, i, result_bit);
  }
  if (temp != 0) {
    error_flag = 1;
  }
  set_sign_big(result, get_sign_big(value_1));
  set_degree_big(result, get_degree_big(value_1));
  return error_flag;
}