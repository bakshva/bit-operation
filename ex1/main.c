#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Функция для кодирования целого числа в переменную длину
size_t encode_varint(uint32_t value, uint8_t *buf)
{
    assert(buf != NULL);
    uint8_t *cur = buf;
    while (value >= 0x80)
    {
        const uint8_t byte = (value & 0x7f) | 0x80; // Устанавливаем старший бит для всех байт, кроме последнего
        *cur = byte;
        value >>= 7; // Сдвигаем число на 7 бит вправо
        ++cur;
    }
    *cur = value; // Записываем последний байт (без установленного старшего бита)
    ++cur;
    return cur - buf; // Возвращаем размер закодированного числа
}
// Функция для декодирования целого числа из переменной длины
uint32_t decode_varint(const uint8_t **bufp)
{
    const uint8_t *cur = *bufp;
    uint8_t byte = *cur++;
    uint32_t value = byte & 0x7f; // Извлекаем младшие 7 бит из первого байта
    size_t shift = 7;
    while (byte >= 0x80)
    {
        byte = *cur++;
        value += (byte & 0x7f) << shift; // Добавляем следующие 7 бит к числу
        shift += 7;
    }
    *bufp = cur;
    return value; // Возвращаем декодированное число
}
// Генерация случайного числа с различными диапазонами
uint32_t generate_number()
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90)
    {
        return r % 128;
    }
    if (p < 95)
    {
        return r % 16384;
    }
    if (p < 99)
    {
        return r % 2097152;
    }
    return r % 268435455;
}

void write_numbers_to_files()
{
    FILE *uncompressed_file = fopen("uncompressed.dat", "wb");
    FILE *compressed_file = fopen("compressed.dat", "wb");

    for (int i = 0; i < 1000000; ++i)
    {
        uint32_t number = generate_number();
        fwrite(&number, sizeof(uint32_t), 1, uncompressed_file);

        uint8_t encoded[10];
        size_t encoded_size = encode_varint(number, encoded);
        fwrite(encoded, encoded_size, 1, compressed_file);
    }

    fclose(uncompressed_file);
    fclose(compressed_file);
}

// Чтение чисел из файлов и сравнение их
void read_and_compare_files()
{
    FILE *uncompressed_file = fopen("uncompressed.dat", "rb");
    FILE *compressed_file = fopen("compressed.dat", "rb");

    for (int i = 0; i < 1000000; ++i)
    {
        uint32_t uncompressed_number, compressed_number;
        fread(&uncompressed_number, sizeof(uint32_t), 1, uncompressed_file);

        uint8_t encoded[10];
        fread(encoded, sizeof(uint8_t), 10, compressed_file);
        const uint8_t *encoded_ptr = encoded;
        compressed_number = decode_varint(&encoded_ptr);

        if (uncompressed_number != compressed_number)
        {
            printf("Numbers at index %d do not match!\n", i);
            break;
        }
    }

    fclose(uncompressed_file);
    fclose(compressed_file);
}
int main()
{
    write_numbers_to_files();

    // Сравнение файлов
    read_and_compare_files();

    return 0;
}
