#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_file.bin>\n", argv[0]);
        return 1;
    } else {
        printf("Input file is: %s\n", argv[1]);
    }

    return 0;
}