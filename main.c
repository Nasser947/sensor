#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "io.h"

int main(int argc, char *argv[])
{
    FileHeader header;
    ADCSample *samples = NULL;

    if (argc != 2)
    {
        printf("Usage: %s <input_file.bin>\n", argv[0]);
        return 1;
    }

    if (!readBinaryFile(argv[1], &header, &samples))
    {
        printf("Failed to read input file.\n");
        return 1;
    }

    return 0;
}