#include <stdio.h>
#include <stdlib.h>

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

    processAllSamples(samples, header.record_count);

    calculateStatistics(samples,
                        header.record_count,
                        stats,
                        header.channel_count);

    detectFaults(samples,
                 header.record_count,
                 stats);

    
    checkSamplingIntegrity(samples,
                           header.record_count,
                           &missingRecords,
                           &outOfOrderRecords);
    return 0;
}