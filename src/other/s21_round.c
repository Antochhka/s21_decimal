#include "../arithmetic/arithmetic.h"
#include "../comparison/comparison.h"
#include "../helpers/helpers.h"
#include "other.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  // Проверка входных параметров
  if (result == NULL) return 1;

  // Знак входного параметра s21_decimal
  int value_sign = get_sign(value);
  // Модуль входного параметра s21_decimal
  s21_decimal unsigned_value = s21_abs(value);
  // Входной параметр s21_decimal с отброшенной дробной частью
  s21_decimal unsigned_tr_value = {0};
  // Переменная для хранения дробной части
  s21_decimal fractional = {0};
  // Переменная для хранения числа 0.5 в форме s21_decimal
  s21_decimal point_five = {{5, 0, 0, 0x00010000}};
  // Переменная для хранения числа 1 в форме s21_decimal
  s21_decimal one = {{1, 0, 0, 0}};
  // Переменная для хранения кода ошибки
  int code = 0;

  // Предварительный сброс битов результирующей переменной в ноль
  for (int i = 0; i < 4; i++) {
    result->bits[i] = 0;
  }

  // Отбрасываем дробную часть и записываем целое число в unsigned_tr_value
  s21_truncate(unsigned_value, &unsigned_tr_value);
  // Вычисляем дробную часть
  code = s21_sub(unsigned_value, unsigned_tr_value, &fractional);

  if (code == 0) {
    // Сравниваем дробную часть и 0.5
    if (s21_is_greater_or_equal(fractional, point_five)) {
      // Если дробная часть >= 0.5, округляем вверх
      code = s21_add(unsigned_tr_value, one, result);
    } else {
      // Если дробная часть < 0.5, округляем вниз
      *result = unsigned_tr_value;
    }

    if (code == 0) {
      // Устанавливаем знак
      set_sign(result, value_sign);
    }
  }

  return code;
}
