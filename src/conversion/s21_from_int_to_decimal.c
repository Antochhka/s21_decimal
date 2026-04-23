#include "../helpers/helpers.h"
#include "conversion.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  // Проверка входных параметров
  if (dst == NULL) return 1;

  // Предварительный сброс битов в ноль
  for (int i = 0; i < 4; i++) {
    dst->bits[i] = 0;
  }

  // Проверка знака числа
  if (src < 0) {
    set_sign(dst, 1);
    dst->bits[0] = -src;  // Установка положительного значения
  } else {
    dst->bits[0] = src;
  }

  return 0;
}
