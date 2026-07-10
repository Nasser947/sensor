#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "adc.h"
#include "io.h"
#include "stats.h"

int main(int argc, char *argv[])
{
    FileHeader header;
    ADCSample *samples = NULL;
    ChannelStats stats[EXPECTED_CHANNELS];
    int missingRecords = 0;
    int outOfOrderRecords = 0;
    int histogram[EXPECTED_CHANNELS][10] = {{0}};
    float *slidingAverages = NULL;
    uint16_t windowSize = 10;

    if (argc < 2 || argc > 3)
    {
        printf("Usage: %s <input_file.bin> [window_size]\n", argv[0]);
        return 1;
    }

    if (argc == 3)
    {
        char *end = NULL;
        errno = 0;
        unsigned long parsedWindow = strtoul(argv[2], &end, 10);

        if (errno != 0 || end == argv[2] || *end != '\0' || parsedWindow == 0 || parsedWindow > UINT16_MAX)
        {
            printf("Invalid window size.\n");
            return 1;
        }

        windowSize = (uint16_t)parsedWindow;
    }

    if (!readBinaryFile(argv[1], &header, &samples))
    {
        printf("Failed to read input file.\n");
        return 1;
    }

    processAllSamples(samples, header.record_count);
    calculateStatistics(samples, header.record_count, stats, header.channel_count);
    detectFaults(samples, header.record_count, stats);
    checkSamplingIntegrity(samples, header.record_count, &missingRecords, &outOfOrderRecords);

    if (header.record_count > 0)
    {
        slidingAverages = (float *)malloc(header.record_count * sizeof(float));
        if (slidingAverages == NULL)
        {
            printf("Memory allocation failed for sliding averages.\n");
            freeSamples(samples);
            return 1;
        }
    }

    calculateSlidingAverage(samples, header.record_count, windowSize, slidingAverages);
    generateHistogram(samples, header.record_count, histogram);
    analyseTemperature(samples, header.record_count, stats, header.channel_count);
    
    return 0;
}