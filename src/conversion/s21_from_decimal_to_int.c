#include "../helpers/helpers.h"
#include "../other/other.h"
#include "conversion.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  // Проверка входных параметров
  if (dst == NULL) return 1;

  int src_scale = get_degree(src);
  int src_sign = get_sign(src);
  // Переменная для хранения числа без дробной части
  s21_decimal truncated_src = {{0, 0, 0, 0}};
  int error_flag = 0;

  if (src_scale > 28) error_flag = 1;

  if (!error_flag) {
    if (src_scale > 0) {
      s21_truncate(src,
                   &truncated_src);  // Отбрасываем дробную часть, если есть
    } else {
      truncated_src = src;  // Если нет дробной части, присваиваем src
    }

    // Проверка на переполнение
    if (truncated_src.bits[1] != 0 || truncated_src.bits[2] != 0) {
      error_flag = 1;  // Устанавливаем флаг ошибки
    }

    // Если не было ошибки, заполняем число и ставим знак
    if (!error_flag) {
      *dst = truncated_src.bits[0];
      if (src_sign) *dst *= -1;
    }
  }

  return error_flag;
}
