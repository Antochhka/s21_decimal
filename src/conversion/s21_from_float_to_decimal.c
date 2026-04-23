#include <math.h>
#include <stdio.h>

#include "../arithmetic/arithmetic.h"
#include "../comparison/comparison.h"
#include "../helpers/helpers.h"
#include "../other/other.h"
#include "conversion.h"

int transform_exponent_from_float_to_int(FloatBits float_value);
void transform_mantissa_from_float_to_s21_decimal(
    FloatBits float_value, s21_decimal *s21_decimal_value);
void count_digits(s21_decimal num, int *digits);
void round_to_seven_significant_digits(s21_decimal *value,
                                       int significant_digits);
s21_decimal get_last_digit(s21_decimal num);

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_conversion_result code = S21_CONVERSION_OK;
  // Проверка корректности указателя
  if (dst == NULL) {
    code = S21_CONVERSION_ERROR;
  }

  // Проверка на бесконечность
  if (isinf(src)) {
    code = S21_CONVERSION_ERROR;
  }

  // Проверка на слишком большие значения
  if (fabs(src) > MAX_FLOAT_TO_CONVERT) {
    code = S21_CONVERSION_ERROR;
  }

  // Проверка на слишком маленькие значения
  if (0 < fabs(src) && fabs(src) < MIN_FLOAT_TO_CONVERT) {
    code = S21_CONVERSION_ERROR;
    s21_decimal_init(dst);
  }

  // Вносим float в объединение чтобы разобрать float по кускам
  FloatBits src_bits;
  src_bits.f = src;

  // Достаем экспоненту из float
  int float_exp = transform_exponent_from_float_to_int(src_bits);

  // Проверка на то, входит ли экспонента в допустимый диапазон
  if (float_exp < -95 || float_exp > 95) {
    // При значениях вне диапазона конвертация в s21_decimal невозможна
    code = S21_CONVERSION_ERROR;
  }

  // Предварительно проверяем на равенство нулю
  if (src == 0.0) {
    code = S21_CONVERSION_OK;
    s21_decimal_init(dst);  // Обнуляем наш s21_decimal
    if (src_bits.fields.sign == 1) {
      set_sign(dst, 1);  // Ставим знак числа
    }
  } else if (code == S21_CONVERSION_OK) {
    // Конвертируем число из float в s21_decimal,
    // учитывая, что число с плавающей точкой представляется так:
    // (-1)^sign * mantissa * 2^exp

    s21_decimal_init(dst);  // Обнуляем наш s21_decimal

    // Конвертируем float-мантиссу и кладем ее в s21_decimal
    transform_mantissa_from_float_to_s21_decimal(src_bits, dst);

    s21_decimal two_to_the_power_of_exp = {0};  // Для хранения (2^exp)

    if (float_exp >= 0) {
      // Если экспонента положительная,
      // домножаем наш s21_decimal, в котором лежит мантисса, на (2^exp)
      set_bit(&two_to_the_power_of_exp, float_exp, 1);

      s21_mul(*dst, two_to_the_power_of_exp, dst);
    } else {
      // Если экспонента отрицательная,
      // делим наш s21_decimal, в котором лежит мантисса, на (2^exp)
      set_bit(&two_to_the_power_of_exp, -float_exp, 1);

      s21_div(*dst, two_to_the_power_of_exp, dst);
    }

    // Обрезаем конечные незначимые нули, если они есть
    *dst = remove_trailing_zeros(*dst);

    int significant_digits;
    count_digits(*dst, &significant_digits);  // Считаем значимые цифры

    // Если более 7 значимых цифр, округляем число
    if (significant_digits > 7) {
      round_to_seven_significant_digits(dst, significant_digits);
    }

    int float_sign = src_bits.fields.sign;
    set_sign(dst, float_sign);  // Устанавливаем знак числа
  }

  return code;
}

// Берет экспоненту из float и преобразует в int
int transform_exponent_from_float_to_int(FloatBits float_value) {
  int exp = float_value.fields.exp;
  exp -= 127;  // Учитываем смещение, т.к. float хранит в формате exp+127

  return exp;
}

// Берет мантиссу из float, конвертирует и заносит в s21_decimal
void transform_mantissa_from_float_to_s21_decimal(
    FloatBits float_value, s21_decimal *s21_decimal_value) {
  unsigned mantissa = float_value.fields.mantissa;
  int bit_value = 0;
  s21_decimal transformed_bit_value = {0};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal two_to_the_power = {0};

  // Проходим по всем 23 битам float-мантиссы слева направо
  for (int i = 22, j = 1; i >= 0 && j <= 23; i--, j++) {
    // Проверяем значение бита
    bit_value = (mantissa >> i) & 1;

    // Если бит включен, прибавляем (2^позиция_бита) к s21_decimal
    if (bit_value) {
      two_to_the_power.bits[0] = 1 << j;  // (2^модуль_позиция_бита)

      // В float-мантиссе позиции от -1 до -23,
      // поэтому делим 1 на (2^модуль_позиции_бита), т.к. 2^-3 = 1/(2^3)
      s21_div(one, two_to_the_power, &transformed_bit_value);

      s21_add(*s21_decimal_value, transformed_bit_value, s21_decimal_value);
    }
  }
  // В конце прибавляем 1.0, т.е. скрытый бит мантиссы
  s21_add(*s21_decimal_value, one, s21_decimal_value);
}

// Считает количество цифр в числе
void count_digits(s21_decimal num, int *digits) {
  s21_decimal zero = {{0, 0, 0, 0}};
  *digits = 0;

  // Проверяем, является ли число нулем
  if (num.bits[0] == 0 && num.bits[1] == 0 && num.bits[2] == 0) {
    *digits = 1;  // Ноль имеет одну цифру
  } else {
    // Делим на 10 и считаем цифры пока число больше нуля
    while (s21_is_greater(num, zero)) {
      (*digits)++;
      stupid_divide_by_10(&num);
    }
  }
}

// Округляет число к ближайшему числу, у которого 7 значимых цифр
void round_to_seven_significant_digits(s21_decimal *value,
                                       int significant_digits) {
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal five = {{5, 0, 0, 0}};
  s21_decimal ten = {{10, 0, 0, 0}};

  int trailing_zeros = 0;  // Нули в конце целой части числа
  int value_scale = get_degree(*value);
  // Обнуляем порядок для корректности арифметических операций
  set_degree(value, 0);

  // Обрезаем число, пока в нем не останется 8 значимых цифр
  while (significant_digits > 8) {
    // Если порядок > нуля, отбрасываем последнюю цифру и уменьшаем порядок
    if (value_scale > 0) {
      stupid_divide_by_10(value);
      value_scale--;
    } else {
      // Если порядок = нулю, заменяем последнюю цифру нулем
      trailing_zeros++;
    }

    significant_digits--;
  }

  // Копия с обрезанными нулями в конце целой части числа
  s21_decimal value_without_trailing_zeros = *value;
  for (int i = 0; i < trailing_zeros; i++) {
    stupid_divide_by_10(&value_without_trailing_zeros);
  }

  s21_decimal eights_digit = get_last_digit(value_without_trailing_zeros);

  // Обрезаем число до 7 значимых цифр
  stupid_divide_by_10(&value_without_trailing_zeros);
  trailing_zeros++;  // Увеличиваем число нулей в конце целой части

  // Округляем число по восьмой цифре
  if (s21_is_greater_or_equal(eights_digit, five)) {
    s21_add(value_without_trailing_zeros, one, &value_without_trailing_zeros);
  }

  *value = value_without_trailing_zeros;

  // Возвращаем из обрезанной формы, дописываем все нули в конец
  for (int i = 0; i < trailing_zeros; i++) {
    s21_mul(*value, ten, value);
  }

  set_degree(value, value_scale);  // Ставим порядок

  // Если порядок > 0, удаляем незначимые конечные нули
  if (value_scale > 0) {
    *value = remove_trailing_zeros(*value);
  }
}

// Возвращает последнюю цифру десятичной формы числа
s21_decimal get_last_digit(s21_decimal num) {
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal remainder = {0};

  // Обнуляем порядок и знак чтобы избежать арифметических ошибок
  set_degree(&num, 0);
  set_sign(&num, 0);

  get_remainder_by_10_to_the_power(num, ten, &remainder, 1);

  return remainder;
}
