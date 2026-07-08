#include <stdio.h>
#include <math.h>

#include "stats.h"


void initializeStatistics(ChannelStats stats[],
                          uint16_t channelCount)
{
    for (uint16_t i = 0; i < channelCount; i++)
    {
        stats[i].mean = 0.0f;
        stats[i].minimum = VREF;
        stats[i].maximum = 0.0f;
        stats[i].std_dev = 0.0f;

        stats[i].sample_count = 0;

        stats[i].over_voltage_count = 0;
        stats[i].under_voltage_count = 0;
        stats[i].sensor_fault_count = 0;
    }
}


void calculateStatistics(ADCSample *samples,
                         uint32_t recordCount,
                         ChannelStats stats[],
                         uint16_t channelCount)
{
    ADCSample *samplePtr;

    float sum[EXPECTED_CHANNELS] = {0.0f};

    initializeStatistics(stats, channelCount);

    samplePtr = samples;

    for (uint32_t i = 0; i < recordCount; i++, samplePtr++)
    {
        uint8_t channel = samplePtr->record.channel_id;

        if (channel >= channelCount)
        {
            continue;
        }

        float voltage = samplePtr->voltage;

        sum[channel] += voltage;

        stats[channel].sample_count++;

        if (voltage < stats[channel].minimum)
        {
            stats[channel].minimum = voltage;
        }

        if (voltage > stats[channel].maximum)
        {
            stats[channel].maximum = voltage;
        }
    }

    for (uint16_t channel = 0; channel < channelCount; channel++)
    {
        if (stats[channel].sample_count > 0)
        {
            stats[channel].mean =
                sum[channel] /
                stats[channel].sample_count;
        }
    }

    float variance[EXPECTED_CHANNELS] = {0.0f};

    samplePtr = samples;

    for (uint32_t i = 0; i < recordCount; i++, samplePtr++)
    {
        uint8_t channel = samplePtr->record.channel_id;

        if (channel >= channelCount)
        {
            continue;
        }

        float difference =
            samplePtr->voltage -
            stats[channel].mean;

        variance[channel] += difference * difference;
    }

    for (uint16_t channel = 0; channel < channelCount; channel++)
    {
        if (stats[channel].sample_count > 0)
        {
            variance[channel] /= stats[channel].sample_count;

            stats[channel].std_dev =
                sqrtf(variance[channel]);
        }
    }
}

void detectFaults(ADCSample *samples,
                  uint32_t recordCount,
                  ChannelStats stats[])
{
    ADCSample *ptr = samples;

    for (uint32_t i = 0; i < recordCount; i++, ptr++)
    {
        uint8_t channel = ptr->record.channel_id;

        if (channel >= EXPECTED_CHANNELS)
        {
            continue;
        }

        float voltage = ptr->voltage;

        if (voltage > 3.0f)
        {
            stats[channel].over_voltage_count++;
        }

        if (voltage < 0.3f)
        {
            stats[channel].under_voltage_count++;
        }

        if (ptr->record.status_flags & 0x01)
        {
            stats[channel].sensor_fault_count++;
        }
    }
}