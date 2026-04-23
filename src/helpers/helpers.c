#include "helpers.h"

#include "../arithmetic/arithmetic.h"
#include "../big_decimal/big_decimal.h"
#include "../comparison/comparison.h"

int first_significant_bit(s21_decimal dec) {
  int res = -1;

  for (int i = SIZE_DEC - 1; i >= 0; i--) {
    int temp = get_bit(dec, i);
    if (temp == 1) {
      res = i;
      break;
    }
  }

  return res;
}

int is_zero(s21_decimal value) {
  int result = TRUE;

  for (int i = SIZE_DEC - 1; i >= 0; i--) {
    if (get_bit(value, i) != 0) {
      result = FALSE;
      break;
    }
  }

  return result;
}

void stupid_divide_by_10(s21_decimal *num) {
  // Делим целую часть (биты 0-2) на 10
  uint64_t remainder = 0;

  for (int i = 2; i >= 0; i--) {
    uint64_t value = (remainder << 32) | (uint32_t)num->bits[i];
    uint64_t div = value / 10;
    remainder = value % 10;
    num->bits[i] = (int32_t)div;
  }
}

// Считает остаток от деления числа на 10 в произвольной степени
void get_remainder_by_10_to_the_power(s21_decimal value_1, s21_decimal value_2,
                                      s21_decimal *remainder,
                                      int power_of_ten) {
  // Копия в которой некоторые цифры числа заменятся нулями
  s21_decimal rounded_down = value_1;

  // В зависимости от степени десятки, заменяем некоторые цифры числа нулями
  for (int i = 0; i < power_of_ten; i++) {
    stupid_divide_by_10(&rounded_down);
  }
  s21_mul(rounded_down, value_2, &rounded_down);

  // Отнимаем от начального значения округленное значение и получаем остаток
  s21_sub(value_1, rounded_down, remainder);
}

// Удаляет конечные незначимые нули
s21_decimal remove_trailing_zeros(s21_decimal value) {
  s21_decimal result = value;
  int degree = get_degree(value);
  int sign = get_sign(value);

  // Конечные незначимые нули могут быть только в дробной части числа
  if (degree > 0) {
    s21_decimal ten = {{10, 0, 0, 0}};
    s21_decimal zero = {{0, 0, 0, 0}};
    s21_decimal remainder = {{0, 0, 0, 0}};
    s21_decimal tmp = value;
    tmp.bits[3] = 0;

    while (degree > 0) {
      // Получаем последнюю цифру числа и отбрасываем ее
      get_remainder_by_10_to_the_power(tmp, ten, &remainder, 1);
      stupid_divide_by_10(&tmp);

      // Пока последнее число = 0, уменьшаем порядок
      if (s21_is_equal(remainder, zero)) {
        --degree;
        result = tmp;
      } else {
        // Как только встречаем ненулевое число - останавливаемся
        break;
      }
    }

    set_degree(&result, degree);
    set_sign(&result, sign);
  }

  return result;
}

void normalization(s21_decimal *value_1, s21_decimal *value_2) {
  int degree_1 = get_degree(*value_1);
  int degree_2 = get_degree(*value_2);

  s21_decimal *first_normalization = degree_1 < degree_2 ? value_1 : value_2;
  s21_decimal *second_normalization = degree_1 < degree_2 ? value_2 : value_1;

  int counter = abs(degree_1 - degree_2);

  while (counter != 0) {
    s21_decimal syllable_1 = *first_normalization;
    s21_decimal syllable_2 = *first_normalization;

    s21_decimal res;
    res.bits[3] = first_normalization->bits[3];

    int significant_bit = first_significant_bit(*first_normalization);

    if (significant_bit > SIZE_DEC - 2 || significant_bit > SIZE_DEC - 4) {
      break;
    } else {
      left_shift(&syllable_1, 3);

      left_shift(&syllable_2, 1);
      if (s21_add(syllable_1, syllable_2, &res) != 0 ||
          get_degree(res) < get_degree(*first_normalization)) {
        break;
      } else {
        *first_normalization = res;
        bit3_struct *bit3 = (bit3_struct *)&first_normalization->bits[3];
        (bit3->degree)++;
        counter--;
      }
    }
  }

  divide_by_10_for_dec(second_normalization, counter);
}

void left_shift(s21_decimal *decimal, int shift) {
  unsigned int remains = 0;  // Остаток при смещении
  for (int i = 0; i < SIZE_DECIMAL - 1; i++) {
    unsigned long long temp_bit = (unsigned long long)decimal->bits[i] << shift;
    decimal->bits[i] = (unsigned int)(temp_bit) + remains;
    if (shift <= 32) {
      remains = (unsigned int)(temp_bit >> SIZE_INT);
    }
  }
}

s21_decimal s21_abs(s21_decimal value) {
  s21_decimal result = value;
  set_sign(&result, 0);
  return result;
}

// 6 базовых функций
int get_sign(s21_decimal decimal) {
  bit3_struct *bit3 = (bit3_struct *)&decimal.bits[3];
  return (int)bit3->sign;
}
void set_sign(s21_decimal *num, int sign_value) {
  if (sign_value == 1)
    num->bits[3] |= (1U << 31);
  else
    num->bits[3] &= ~(1U << 31);
}

int get_degree(s21_decimal decimal) {
  bit3_struct *bit3 = (bit3_struct *)&decimal.bits[3];
  return (int)bit3->degree;
}
void set_degree(s21_decimal *decimal, int degree) {
  bit3_struct *bit3 = (bit3_struct *)&decimal->bits[3];
  bit3->degree = degree & 0xFF;
}

int get_bit(s21_decimal value, int i) {
  int bitIndex = i / 32;
  int bitPosition = i % 32;
  return (value.bits[bitIndex] >> bitPosition) & 1;
}

void set_bit(s21_decimal *result, int i, int result_bit) {
  int bitIndex = i / 32;
  int bitPosition = i % 32;
  if (result_bit == 1) {
    result->bits[bitIndex] |= (1 << bitPosition);
  } else {
    result->bits[bitIndex] &= ~(1 << bitPosition);
  }
}

// удобное заполнение decimal
void set_decimal(s21_decimal *decimal, int sign, int degree,
                 unsigned int mantissa[3]) {
  bit3_struct *bit3 = (bit3_struct *)&decimal->bits[3];
  bit3->sign = sign & 0x1;
  bit3->degree = degree & 0xFF;
  bit3->not_used_1 = 0;
  bit3->not_used_2 = 0;
  for (int i = 0; i < 3; i++) {
    decimal->bits[i] = mantissa[i];
  }
}

// Вывод decimal
void print_bits(int value, int is_bits_3) {
  unsigned int buffer = sizeof(int) * 8;
  for (int i = buffer - 1; i >= 0; i--) {
    int mask = 1 << i;
    int bit = 0;
    if (mask & value) {
      bit = 1;
    }
    if (is_bits_3 == TRUE) {
      if (i == 15) {
        printf("%s", " | not_used: ");
      } else if (i == 23) {
        printf("%s", " | degree: ");
      } else if (i == 30) {
        printf("%s", " | not_used: ");
      } else if (i == 31) {
        printf("%s", "sign ");
      }
    }
    printf("%d", bit);
  }
}

void print_decimal(s21_decimal value) {
  printf("%c", '|');
  for (int i = SIZE_DECIMAL - 1; i >= 0; i--) {
    if (i == 3) {
      print_bits(value.bits[i], TRUE);
    } else {
      print_bits(value.bits[i], FALSE);
    }
    printf("\n");
  }
}

void s21_decimal_init(s21_decimal *dec) {
  if (dec != NULL) {
    dec->bits[0] = 0;  // Младшие 32 бита мантиссы
    dec->bits[1] = 0;  // Средние 32 бита
    dec->bits[2] = 0;  // Старшие 32 бита
    dec->bits[3] = 0;  // Бит знака + масштаб + зарезервированные биты
  }
}

void divide_by_10_for_dec(s21_decimal *num, int index) {
  int diff = FALSE;

  while (index != 0) {
    int degree = get_degree(*num);
    int sign = get_sign(*num);
    uint64_t remainder = 0;

    for (int i = 2; i >= 0; i--) {
      uint64_t value = (remainder << 32) | (uint32_t)num->bits[i];
      remainder = value % 10;
      num->bits[i] = (int32_t)(value / 10);
    }

    if (remainder != 0 && diff == FALSE && index > 1) {
      diff = TRUE;
    }

    if (remainder >= 5 && index == 1 && diff == TRUE) {
      uint64_t carry = 1;
      for (int i = 0; i < 3 && carry; i++) {
        uint64_t sum = (uint64_t)num->bits[i] + carry;
        num->bits[i] = sum & 0xFFFFFFFF;
        carry = sum >> 32;
      }
    }

    // Обновляем масштаб и знак
    set_degree(num, degree);
    set_sign(num, sign);

    bit3_struct *bit3 = (bit3_struct *)&num->bits[3];
    (bit3->degree)--;
    index--;
  }
}

int check_decimal(s21_decimal decimal) {
  int error_flag = 0;
  bit3_struct *bit3 = (bit3_struct *)&decimal.bits[3];

  if (bit3->not_used_1 != 0 || bit3->not_used_2 != 0) {
    error_flag = 1;
  }
  if (get_degree(decimal) > 28) {
    error_flag = 1;
  }
  return error_flag;
}
int null_decimal(s21_decimal value) {
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) {
    return 1;
  } else
    return 0;
}
