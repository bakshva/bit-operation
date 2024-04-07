#include <stdio.h>
#include <inttypes.h>
#include "command.h"
#include "coder.h"

int encode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "r");
    if (in == NULL)
    {
        return -1;
    }
    FILE *out = fopen(out_file_name, "w+");
    if (out == NULL)
    {
        return -1;
    }
    while (1)
    {
        char buffer[8];
        fgets(buffer, 8, in);
        if (feof(in))
        {
            break;
        }
        uint32_t code_point;
        sscanf(buffer, "%" SCNx32, &code_point);
        CodeUnit code_unit;
        encode(code_point, &code_unit);
        write_code_unit(out, &code_unit);
    }

    return 0;
}

int decode_file(const char *in_file_name, const char *out_file_name)
{
    FILE *in = fopen(in_file_name, "r");
    if (in == NULL)
    {
        return -1;
    }
    CodeUnit code_unit;
    FILE *out = fopen(out_file_name, "w+");
    if (out == NULL)
    {
        return -1;
    }
    while (!read_next_code_unit(in, &code_unit))
    {
        uint32_t buffer = decode(&code_unit);
        fprintf(out, "%" PRIx32 "\n", buffer);
    }
    fclose(in);
    fclose(out);
    return 0;
}
