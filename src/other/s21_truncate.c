#include <stdint.h>

#include "../helpers/helpers.h"
#include "other.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  // Проверка входных параметров
  if (result == NULL) return 1;

  // Предварительный сброс битов результирующей переменной в ноль
  for (int i = 0; i < 4; i++) {
    result->bits[i] = 0;
  }

  int scale = get_degree(value);
  // Если число дробное, отбрасываем числа после запятой
  while (scale > 0) {
    stupid_divide_by_10(&value);
    scale--;
  }

  // Заполняем результирующую переменную
  for (int i = 0; i < 3; i++) {
    result->bits[i] = value.bits[i];
  }

  // Устанавливаем знак числа
  set_sign(result, get_sign(value));

  return 0;
}
