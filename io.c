#include <stdio.h>
#include <stdlib.h>

#include "io.h"

int readBinaryFile(const char *filename,
                   FileHeader *header,
                   ADCSample **samples)
{
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("Error: Unable to open file %s\n", filename);
        return 0;
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

    for (uint32_t i = 0; i < header->record_count; i++)
    {
        ADCRecord rawRecord;

        if (fread(&rawRecord, sizeof(ADCRecord), 1, fp) != 1)
        {
            printf("Error reading records.\n");
            free(*samples);
            *samples = NULL;
            fclose(fp);
            return 0;
        }

        (*samples)[i].timestamp = rawRecord.timestamp;
        (*samples)[i].channel_id = rawRecord.channel_id;
        (*samples)[i].raw_value = rawRecord.raw_value;
        (*samples)[i].voltage = adcToVoltage(rawRecord.raw_value);
        (*samples)[i].temperature = temperatureToCelsius(rawRecord.temperature);
        (*samples)[i].status_flags = rawRecord.status_flags;
        (*samples)[i].sequence_number = rawRecord.sequence_number;
    }

    fclose(fp);
    return 1;
}

void freeSamples(ADCSample *samples)
{
    if (samples != NULL)
    {
        free(samples);
    }
}
