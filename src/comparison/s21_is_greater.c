#include "../helpers/helpers.h"
#include "comparison.h"

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int res = FALSE;

  value_1 = remove_trailing_zeros(value_1);
  value_2 = remove_trailing_zeros(value_2);

  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);

  if (s21_is_equal(value_1, value_2)) {
  } else if (sign_1 > sign_2) {
    res = FALSE;
  } else if (sign_1 < sign_2) {
    res = TRUE;
  } else if (sign_1 == sign_2) {
    int is_negative_numbers = FALSE;

    if (sign_1 == MINUS) {
      is_negative_numbers = TRUE;
    }

    normalization(&value_1, &value_2);

    int i = SIZE_INT * 3 - 1;

    while (i >= 0) {
      int bit_1 = get_bit(value_1, i);
      int bit_2 = get_bit(value_2, i);

      if (bit_1 != bit_2) {
        if (is_negative_numbers == TRUE) {
          res = (bit_1 < bit_2) ? TRUE : FALSE;
        } else {
          res = (bit_1 < bit_2) ? FALSE : TRUE;
        }
        break;
      }
      i--;
    }
  }

  return res;
}