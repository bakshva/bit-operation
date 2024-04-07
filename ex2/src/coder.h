#ifndef CODER_H
#define CODER_H

#include <inttypes.h>

// Максимальная длина кодовой единицы
enum
{
    MaxCodeLength = 4
};

// Структура, представляющая кодовую единицу
typedef struct
{
    uint8_t code[MaxCodeLength];  // Массив байтов, содержащий кодовую единицу
    size_t length;                 // Длина кодовой единицы
} CodeUnit;

// Функция для кодирования кодовой точки в кодовую единицу
int encode(uint32_t code_point, CodeUnit *code_units);

// Функция для декодирования кодовой единицы в кодовую точку
uint32_t decode(const CodeUnit *code_unit);

// Функция для чтения следующей кодовой единицы из файла
int read_next_code_unit(FILE *in, CodeUnit *code_units);

// Функция для записи кодовой единицы в файл
int write_code_unit(FILE *out, const CodeUnit *code_unit);

#endif  // CODER_H
