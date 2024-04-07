#include <stdio.h>
#include <inttypes.h>
#include "coder.h"

// Функция кодирования кодовой точки Unicode в последовательность кодовых единиц UTF-8
int encode(uint32_t code_point, CodeUnit *code_units)
{
    uint8_t count = 0;

    // Определение количества бит, необходимых для представления кодовой точки
    for (uint32_t i = code_point; i > 0; i >>= 1)
    {
        count++;
    }

    // Кодирование в зависимости от количества бит
    if (count <= 7)
    {
        code_units->code[0] = code_point; // Простая кодовая точка, влезает в один байт
        code_units->length = 1;
        return 0;
    }
    else if (count <= 11)
    {
        // Кодирование для двухбайтовой последовательности
        code_units->code[0] = 0xC0 | (code_point >> 6);        // Первый байт
        code_units->code[1] = 0x80 | (code_point & 0x3F);      // Второй байт
        code_units->length = 2;
        return 0;
    }
    else if (count <= 16)
    {
        // Кодирование для трехбайтовой последовательности
        code_units->code[0] = 0xE0 | (code_point >> 12);       // Первый байт
        code_units->code[1] = 0x80 | ((code_point & 0xFC0) >> 6); // Второй байт
        code_units->code[2] = 0x80 | (code_point & 0x3F);      // Третий байт
        code_units->length = 3;
        return 0;
    }
    else if (count <= 21)
    {
        // Кодирование для четырехбайтовой последовательности
        code_units->code[0] = 0xF0 | (code_point >> 18);       // Первый байт
        code_units->code[1] = 0x80 | ((code_point & 0x3F000) >> 12); // Второй байт
        code_units->code[2] = 0x80 | ((code_point & 0xFC0) >> 6); // Третий байт
        code_units->code[3] = 0x80 | (code_point & 0x3F);      // Четвертый байт
        code_units->length = 4;
        return 0;
    }
    return -1; // Неверная кодовая точка
}

// Функция декодирования последовательности кодовых единиц UTF-8 в кодовую точку Unicode
uint32_t decode(const CodeUnit *code_unit)
{
    uint32_t code_point;

    // Декодирование в зависимости от значения первого байта
    if ((code_unit->code[0] >> 7) == 0)
    {
        // Однобайтовая последовательность
        return (code_point = code_unit->code[0]);
    }
    else if (code_unit->code[0] <= 0xDF)
    {
        // Двухбайтовая последовательность
        return (code_point = (((code_unit->code[0] & 0x1F) << 6) | (code_unit->code[1] & 0x3F)));
    }
    else if (code_unit->code[0] <= 0xEF)
    {
        // Трехбайтовая последовательность
        return (((code_unit->code[0] & 0xF) << 12) | ((code_unit->code[1] & 0x3F) << 6) | (code_unit->code[2] & 0x3F));
    }
    else if (code_unit->code[0] <= 0xF7)
    {
        // Четырехбайтовая последовательность
        return (((code_unit->code[0] & 0x7) << 18) | ((code_unit->code[1] & 0x3F) << 12) | ((code_unit->code[2] & 0x3F) << 6) | (code_unit->code[3] & 0x3F));
    }
    return 0; // Недопустимая последовательность
}

// Функция чтения следующей кодовой единицы из файла
int read_next_code_unit(FILE *in, CodeUnit *code_units)
{
    uint8_t buffer = 0;
    fread(&buffer, 1, 1, in);

    while (!feof(in))
    {
        uint8_t enum_bite = 0;
        while (buffer & (1 << (7 - enum_bite)))
        {
            enum_bite++;
        }
        if (enum_bite == 1)
        {
            fread(&buffer, 1, 1, in);
            continue;
        }
        if (enum_bite == 0)
        {
            enum_bite = 1;
        }
        if (enum_bite <= MaxCodeLength)
        {
            code_units->length = 0;
            for (int i = 1; i <= enum_bite; i++)
            {
                code_units->code[i - 1] = buffer;
                code_units->length++;
                if (i == enum_bite)
                {
                    return 0;
                }
                fread(&buffer, 1, 1, in);
                if ((buffer & 0xC0) != 0x80)
                {
                    // Определение конца последовательности
                    break;
                }
            }
        }
    }
    return -1; // Не удалось прочитать следующую кодовую единицу
}

// Функция записи кодовой единицы в файл
int write_code_unit(FILE *out, const CodeUnit *code_unit)
{
    // Запись в файл
    if (fwrite(code_unit->code, 1, code_unit->length, out) == code_unit->length)
    {
        return 0;
    }
    return -1; // Ошибка записи
}
