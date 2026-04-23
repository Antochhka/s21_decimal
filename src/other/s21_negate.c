#include "../helpers/helpers.h"
#include "other.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  int error_flag = 0;
  if (!result) {
    error_flag = 1;
  } else {
    *result = value;
    set_sign(result, !get_sign(value));
  }
  return error_flag;
}