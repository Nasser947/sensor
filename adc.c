#include <stdio.h>
#include "adc.h"

float adcToVoltage(uint16_t rawValue)
{
    return ((float)rawValue / ADC_MAX) * VREF;
}


float temperatureToCelsius(int16_t rawTemperature)
{
    return (float)rawTemperature/10.0f;
}

void processSample(ADCSample *sample)
{
    if (sample == NULL)
    {
        return;
    }

    sample->voltage = adcToVoltage(sample->record.raw_value);

    sample->temperature_c =
        temperatureToCelsius(sample->record.temperature);
}

void processAllSamples(ADCSample *samples,
                       uint32_t recordCount)
{
    printf("Processing %u samples...\n", recordCount);
    ADCSample *ptr = samples;

    for (uint32_t i = 0; i < recordCount; i++, ptr++)
    {
        processSample(ptr);
    }
}