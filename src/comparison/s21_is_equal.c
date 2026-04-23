#include "../helpers/helpers.h"
#include "comparison.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = TRUE;

  value_1 = remove_trailing_zeros(value_1);
  value_2 = remove_trailing_zeros(value_2);

  for (int i = SIZE_DECIMAL - 2; i >= 0; i--) {
    if (value_1.bits[i] != value_2.bits[i]) {
      res = FALSE;
      break;
    }
  }

  if (res == TRUE) {
    if (is_zero(value_1) == TRUE && is_zero(value_2) == TRUE) {
    }

    else if (get_sign(value_1) != get_sign(value_2) ||
             get_degree(value_1) != get_degree(value_2)) {
      res = FALSE;
    }
  }
  return res;
}