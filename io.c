#include <stdio.h>
#include <stdlib.h>

#include "io.h"

int readBinaryFile(const char *filename,
                   FileHeader *header,
                   ADCSample **samples)
{
    FILE *fp;

    ADCRecord *records;

    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("Error: Unable to open file %s\n", filename);
        return 0;
    }
    else
    {
        printf("File %s opened successfully.\n", filename);
    }

    if (fread(header, sizeof(FileHeader), 1, fp) != 1)
    {
        printf("Error reading file header.\n");
        fclose(fp);
        return 0;
    }

    if (header->magic != MAGIC_NUMBER)
    {
        printf("Invalid magic number.\n");
        fclose(fp);
        return 0;
    }

    if (header->version != EXPECTED_VERSION)
    {
        printf("Unsupported version.\n");
        fclose(fp);
        return 0;
    }

    if (header->channel_count != EXPECTED_CHANNELS)
    {
        printf("Unexpected channel count.\n");
        fclose(fp);
        return 0;
    }

    if (header->sample_rate_hz != EXPECTED_SAMPLE_RATE)
    {
        printf("Unexpected sample rate.\n");
        fclose(fp);
        return 0;
    }

    *samples = (ADCSample *)malloc(header->record_count * sizeof(ADCSample));

    if (*samples == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(fp);
        return 0;
    }

    records = (ADCRecord *)(*samples);

    if (fread(records,
              sizeof(ADCRecord),
              header->record_count,
              fp) != header->record_count)
    {
        printf("Error reading records.\n");

        free(*samples);
        *samples = NULL;

        fclose(fp);
        return 0;
    }

    fclose(fp);

    return 1;
}
