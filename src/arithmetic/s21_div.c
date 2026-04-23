#include "../big_decimal/big_decimal.h"
#include "../comparison/comparison.h"
#include "../conversion/conversion.h"
#include "../helpers/helpers.h"
#include "../other/other.h"
#include "arithmetic.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int error_flag = 0;
  int error = 0;
  if (!result || get_degree(value_1) > 28 || get_degree(value_2) > 28)
    error_flag = 4;
  for (int i = 0; i < 3; i++) {
    if (value_2.bits[i] == 0) error++;
  }
  if (error == 3) error_flag = 3;

  bit3_struct *bit3_1 = (bit3_struct *)&value_1.bits[3];
  bit3_struct *bit3_2 = (bit3_struct *)&value_2.bits[3];
  if (bit3_1->not_used_1 != 0 || bit3_2->not_used_1 != 0 ||
      bit3_1->not_used_2 != 0 || bit3_2->not_used_2 != 0) {
    error_flag = 4;
  }

  if (error_flag == 0 && equal_numbers(value_1, value_2, result) == 0) {
    s21_decimal_init(result);
    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);
    if ((sign1 == 0 && sign2 == 0) || (sign1 == 1 && sign2 == 1)) {
      error_flag = div_positive(value_1, value_2, result);
    } else {
      error_flag = div_positive(value_1, value_2, result);
      s21_negate(*result, result);
    }
  }
  if (error_flag == 1) {
    int sign = get_sign(*result);
    if (sign == 1) {
      error_flag = 2;
    }
  }
  return error_flag;
}

int div_positive(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  value_1 = s21_abs(value_1);
  value_2 = s21_abs(value_2);

  int degree = 0;
  int error_flag = 0;
  s21_big_decimal big_value_1;
  decimal_to_big_decimal(&value_1, &big_value_1);
  s21_big_decimal big_value_2;
  decimal_to_big_decimal(&value_2, &big_value_2);

  normalization_big(&big_value_1, &big_value_2);

  s21_big_decimal big_result, temp_big, temp_result, zero,
      one = {{1, 0, 0, 0, 0, 0, 0, 0}}, five = {{5, 0, 0, 0, 0, 0, 0, 0}},
      ten = {{10, 0, 0, 0, 0, 0, 0, 0}};
  s21_decimal_init_big(&temp_big);
  s21_decimal_init_big(&temp_result);
  s21_decimal_init_big(&zero);
  s21_decimal_init_big(&big_result);

  temp_big = div_big_decimal(big_value_1, big_value_2, &big_result);

  int count = count_numbers(big_result, ten);
  set_degree_big(&zero, get_degree_big(temp_big));

  div_fractional_part(&big_result, &temp_big, &zero, ten, big_value_2, &degree,
                      count);
  set_degree_big(&zero, get_degree_big(temp_big));
  rounding_big_decimal(&big_result, &temp_big, zero, one, five, ten,
                       big_value_2, &degree, count);

  if (error_flag == 0) {
    error_flag = big_decimal_to_decimal(&big_result, result);
  }

  set_degree(result, degree);
  return error_flag;
}

s21_big_decimal div_big_decimal(s21_big_decimal value_1,
                                s21_big_decimal value_2,
                                s21_big_decimal *result) {
  int i = 32 * 7 - 1;
  s21_big_decimal temp_big;
  s21_decimal_init_big(&temp_big);
  set_degree_big(&temp_big, get_degree_big(value_1));
  while (get_bit_big(value_1, i) != 1) {
    i--;
  }

  while (i >= 0) {
    left_shift_big(&temp_big, 1);
    set_bit_big(&temp_big, 0, get_bit_big(value_1, i));
    if (s21_is_greater_or_equal_big(temp_big, value_2)) {
      sub_big_decimal(temp_big, value_2, &temp_big);
      left_shift_big(result, 1);
      set_bit_big(result, 0, 1);
    } else {
      left_shift_big(result, 1);
      set_bit_big(result, 0, 0);
    }
    i--;
  }

  return temp_big;
}

void div_fractional_part(s21_big_decimal *big_result, s21_big_decimal *temp_big,
                         s21_big_decimal *zero, s21_big_decimal ten,
                         s21_big_decimal big_value_2, int *degree, int count) {
  s21_big_decimal temp_result;
  s21_decimal_init_big(&temp_result);
  s21_big_decimal temp_big_result = *big_result;
  int flag = 0;

  while (s21_is_equal_big(*temp_big, *zero) != 1 && *degree < 29 - count) {
    set_degree_big(zero, get_degree_big(*temp_big));
    mul_big_decimal(*temp_big, ten, &temp_result);
    int temp_degree = get_degree_big(*temp_big);
    *temp_big = temp_result;
    set_degree_big(temp_big, temp_degree);
    s21_decimal_init_big(&temp_result);

    mul_big_decimal(temp_big_result, ten, &temp_result);
    temp_big_result = temp_result;
    s21_decimal_init_big(&temp_result);

    if (s21_is_greater_big(*temp_big, big_value_2) == 1) {
      flag = 1;
      *temp_big = div_big_decimal(*temp_big, big_value_2, &temp_result);
      add_big_decimal(temp_big_result, temp_result, &temp_big_result);
      s21_decimal_init_big(&temp_result);
    }
    (*degree)++;
  }
  if (flag == 1) {
    *big_result = temp_big_result;
  } else {
    *degree = 0;
  }
}

void rounding_big_decimal(s21_big_decimal *big_result,
                          s21_big_decimal *temp_big, s21_big_decimal zero,
                          s21_big_decimal one, s21_big_decimal five,
                          s21_big_decimal ten, s21_big_decimal big_value_2,
                          int *degree, int count) {
  s21_big_decimal temp_result;
  s21_big_decimal max = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0, 0x0}};

  s21_decimal_init_big(&temp_result);

  if (s21_is_equal_big(*temp_big, zero) != 1 && *degree == 29 - count) {
    mul_big_decimal(*temp_big, ten, &temp_result);
    int temp_degree = get_degree_big(*temp_big);
    *temp_big = temp_result;
    set_degree_big(temp_big, temp_degree);
    s21_decimal_init_big(&temp_result);

    *temp_big = div_big_decimal(*temp_big, big_value_2, &temp_result);
    set_degree_big(&zero, get_degree_big(*temp_big));

    if (s21_is_greater_big(temp_result, five) == 1) {
      add_big_decimal(*big_result, one, big_result);
    } else if (s21_is_equal_big(temp_result, five) == 1) {
      if (s21_is_equal_big(*temp_big, zero) == 0 ||
          (get_bit_big(*big_result, 0) == 1)) {
        add_big_decimal(*big_result, one, big_result);
      }
    }
  }

  s21_decimal_init_big(temp_big);
  s21_decimal_init_big(&zero);
  s21_decimal_init_big(&temp_result);

  extra_zeros_big_decimal(big_result, temp_big, ten, zero, degree);

  s21_decimal_init_big(temp_big);

  if (s21_is_greater_big(*big_result, max) == 1 && *degree > 0) {
    *temp_big = div_big_decimal(*big_result, ten, &temp_result);
    (*degree)--;
    if (s21_is_greater_big(*temp_big, five) == 1) {
      add_big_decimal(temp_result, one, &temp_result);
    } else if (s21_is_equal_big(*temp_big, five) == 1 &&
               get_bit_big(temp_result, 0) == 1) {
      add_big_decimal(temp_result, one, &temp_result);
    }
    *big_result = temp_result;
  }

  s21_decimal_init_big(temp_big);
  s21_decimal_init_big(&temp_result);
  extra_zeros_big_decimal(big_result, temp_big, ten, zero, degree);
}

void extra_zeros_big_decimal(s21_big_decimal *big_result,
                             s21_big_decimal *temp_big, s21_big_decimal ten,
                             s21_big_decimal zero, int *degree) {
  s21_big_decimal temp_big_result = *big_result;
  s21_big_decimal temp_result;
  s21_decimal_init_big(&temp_result);
  while (s21_is_equal_big(*temp_big, zero) == 1 && *degree > 0) {
    *temp_big = div_big_decimal(temp_big_result, ten, &temp_result);
    temp_big_result = temp_result;
    s21_decimal_init_big(&temp_result);
    if (s21_is_equal_big(*temp_big, zero) == 1) {
      (*degree)--;
      *big_result = temp_big_result;
    }
  }
}

int count_numbers(s21_big_decimal big_result, s21_big_decimal ten) {
  s21_big_decimal temp;
  s21_big_decimal one = {{10, 0, 0, 0, 0, 0, 0, 0}};
  s21_decimal_init_big(&temp);

  int count = 1;
  while (s21_is_greater_or_equal_big(big_result, one) == 1) {
    count++;
    if (count == 29) {
      break;
    }
    mul_big_decimal(one, ten, &temp);
    one = temp;
    s21_decimal_init_big(&temp);
  }
  return count;
}

int equal_numbers(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result) {
  int flag = 0;
  int degree1 = get_degree(value_1);
  int degree2 = get_degree(value_2);
  set_degree(&value_1, 0);
  set_degree(&value_2, 0);
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal_init(result);

  if (s21_is_equal(value_1, value_2) == 1) {
    flag = 1;
    (*result).bits[0] = 1;
    if (degree1 < degree2) {
      for (int i = 0; i < degree2 - degree1; i++) {
        s21_mul(*result, ten, result);
      }
    } else {
      set_degree(result, degree1 - degree2);
    }
  }
  if (flag == 1) {
    if ((get_sign(value_1) == 0 && get_sign(value_2) == 1) ||
        (get_sign(value_1) == 1 && get_sign(value_2) == 0)) {
      set_sign(result, 1);
    } else {
      set_sign(result, 0);
    }
  }

  set_degree(&value_1, degree1);
  set_degree(&value_2, degree2);
  return flag;
}
