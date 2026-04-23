#include <math.h>

#include "../helpers/helpers.h"
#include "conversion.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  // Проверка входных параметров
  if (dst == NULL) return 1;

  int src_scale = get_degree(src);
  int src_sign = get_sign(src);
  int error_flag = 0;

  if (src_scale > 28) error_flag = 1;

  if (!error_flag) {
    // Временный double для лучшей точности и предотвращения переполнения
    double temp = 0.0;

    // Постепенно заносим значение мантиссы из s21_decimal
    for (int i = 0; i < 96; i++) {
      if (get_bit(src, i)) {
        temp += pow(2.0, i);
      }
    }

    // Учитывая порядок числа, изменяем положение запятой
    while (src_scale > 0) {
      temp /= 10;
      src_scale--;
    }

    // Проверяем и устанавливаем знак числа
    if (src_sign) {
      *dst = (float)-temp;
    } else {
      *dst = (float)temp;
    }
  }

  return error_flag;
}