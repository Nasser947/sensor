#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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
        stats[i].temp_min = 0.0f;
        stats[i].temp_mean = 0.0f;
        stats[i].temp_max = 0.0f;

        stats[i].sample_count = 0;
        stats[i].over_voltage_count = 0;
        stats[i].under_voltage_count = 0;
        stats[i].sensor_fault_count = 0;
        stats[i].high_temperature_count = 0;
    }
}

void calculateStatistics(ADCSample *samples,
                         uint32_t recordCount,
                         ChannelStats stats[],
                         uint16_t channelCount)
{
    ADCSample *samplePtr = samples;
    float sum[EXPECTED_CHANNELS] = {0.0f};

    initializeStatistics(stats, channelCount);

    for (uint32_t i = 0; i < recordCount; i++, samplePtr++)
    {
        uint8_t channel = samplePtr->channel_id;

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
            stats[channel].mean = sum[channel] / stats[channel].sample_count;
        }
    }

    float variance[EXPECTED_CHANNELS] = {0.0f};
    samplePtr = samples;

    for (uint32_t i = 0; i < recordCount; i++, samplePtr++)
    {
        uint8_t channel = samplePtr->channel_id;

        if (channel >= channelCount)
        {
            continue;
        }

        float difference = samplePtr->voltage - stats[channel].mean;
        variance[channel] += difference * difference;
    }

    for (uint16_t channel = 0; channel < channelCount; channel++)
    {
        if (stats[channel].sample_count > 0)
        {
            variance[channel] /= stats[channel].sample_count;
            stats[channel].std_dev = sqrtf(variance[channel]);
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
        uint8_t channel = ptr->channel_id;

        if (channel >= EXPECTED_CHANNELS)
        {
            continue;
        }

        if (ptr->voltage > 3.0f)
        {
            stats[channel].over_voltage_count++;
        }

        if (ptr->voltage < 0.3f)
        {
            stats[channel].under_voltage_count++;
        }

        if (ptr->status_flags & 0x01)
        {
            stats[channel].sensor_fault_count++;
        }
    }
}

void checkSamplingIntegrity(ADCSample *samples,
                            uint32_t recordCount,
                            int *missingRecords,
                            int *outOfOrderRecords)
{
    ADCSample *ptr = samples;

    *missingRecords = 0;
    *outOfOrderRecords = 0;

    if (recordCount < 2)
    {
        return;
    }

    uint32_t expectedSeq = ptr->sequence_number;
    ptr++;

    for (uint32_t i = 1; i < recordCount; i++, ptr++)
    {
        uint32_t currentSeq = ptr->sequence_number;

        if (currentSeq > expectedSeq + 1)
        {
            *missingRecords += (int)(currentSeq - expectedSeq - 1);
        }

        if (currentSeq < expectedSeq)
        {
            (*outOfOrderRecords)++;
        }

        expectedSeq = currentSeq;
    }
}

void calculateSlidingAverage(ADCSample *samples,
                             uint32_t recordCount,
                             uint16_t windowSize,
                             float *slidingAverages)
{
    if (windowSize == 0 || recordCount == 0 || slidingAverages == NULL)
    {
        return;
    }

    float *windowValues = (float *)calloc((size_t)windowSize * EXPECTED_CHANNELS, sizeof(float));
    float *windowSum = (float *)calloc(EXPECTED_CHANNELS, sizeof(float));
    uint32_t *windowCount = (uint32_t *)calloc(EXPECTED_CHANNELS, sizeof(uint32_t));
    uint32_t *windowHead = (uint32_t *)calloc(EXPECTED_CHANNELS, sizeof(uint32_t));

    if (windowValues == NULL || windowSum == NULL || windowCount == NULL || windowHead == NULL)
    {
        free(windowValues);
        free(windowSum);
        free(windowCount);
        free(windowHead);
        return;
    }

    for (uint32_t i = 0; i < recordCount; i++)
    {
        uint8_t channel = samples[i].channel_id;

        if (channel >= EXPECTED_CHANNELS)
        {
            slidingAverages[i] = 0.0f;
            continue;
        }

        size_t slot = (size_t)channel * windowSize;
        uint32_t currentHead = windowHead[channel];

        if (windowCount[channel] == windowSize)
        {
            float oldValue = windowValues[slot + currentHead];
            windowSum[channel] -= oldValue;
        }
        else
        {
            windowCount[channel]++;
        }

        windowValues[slot + currentHead] = samples[i].voltage;
        windowSum[channel] += samples[i].voltage;
        windowHead[channel] = (currentHead + 1) % windowSize;

        slidingAverages[i] = (windowCount[channel] > 0) ? (windowSum[channel] / windowCount[channel]) : 0.0f;
    }

    free(windowValues);
    free(windowSum);
    free(windowCount);
    free(windowHead);
}

void generateHistogram(ADCSample *samples,
                       uint32_t recordCount,
                       int histogram[EXPECTED_CHANNELS][10])
{
    for (uint16_t channel = 0; channel < EXPECTED_CHANNELS; channel++)
    {
        for (int bin = 0; bin < 10; bin++)
        {
            histogram[channel][bin] = 0;
        }
    }

    for (uint32_t i = 0; i < recordCount; i++)
    {
        uint8_t channel = samples[i].channel_id;

        if (channel >= EXPECTED_CHANNELS)
        {
            continue;
        }

        int bin = (int)((samples[i].voltage / VREF) * 10.0f);

        if (bin < 0)
        {
            bin = 0;
        }
        else if (bin > 9)
        {
            bin = 9;
        }

        histogram[channel][bin]++;
    }
}

void analyseTemperature(ADCSample *samples,
                        uint32_t recordCount,
                        ChannelStats stats[],
                        uint16_t channelCount)
{
    if (recordCount == 0)
    {
        return;
    }

    float temperatureSum[EXPECTED_CHANNELS] = {0.0f};
    uint32_t temperatureCount[EXPECTED_CHANNELS] = {0};

    for (uint16_t channel = 0; channel < channelCount; channel++)
    {
        stats[channel].temp_min = 0.0f;
        stats[channel].temp_mean = 0.0f;
        stats[channel].temp_max = 0.0f;
        stats[channel].high_temperature_count = 0;
    }

    for (uint32_t i = 0; i < recordCount; i++)
    {
        uint8_t channel = samples[i].channel_id;

        if (channel >= channelCount)
        {
            continue;
        }

        float temp = samples[i].temperature;

        temperatureSum[channel] += temp;
        temperatureCount[channel]++;

        if (temperatureCount[channel] == 1)
        {
            stats[channel].temp_min = temp;
            stats[channel].temp_max = temp;
        }
        else
        {
            if (temp < stats[channel].temp_min)
            {
                stats[channel].temp_min = temp;
            }

            if (temp > stats[channel].temp_max)
            {
                stats[channel].temp_max = temp;
            }
        }

        if (temp > 60.0f)
        {
            stats[channel].high_temperature_count++;
        }
    }

    for (uint16_t channel = 0; channel < channelCount; channel++)
    {
        if (temperatureCount[channel] > 0)
        {
            stats[channel].temp_mean = temperatureSum[channel] / temperatureCount[channel];
        }
    }
}