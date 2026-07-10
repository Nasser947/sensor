#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define MAGIC_NUMBER         0xADC1BEEF
#define EXPECTED_VERSION     1
#define EXPECTED_CHANNELS    4
#define EXPECTED_SAMPLE_RATE 1000
#define VREF                 3.3f
#define ADC_MAX              4095.0f

#pragma pack(push, 1)

typedef struct
{
    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate_hz;
    uint8_t reserved[8];
} FileHeader;

typedef struct
{
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    int16_t temperature;
    uint8_t status_flags;
    uint32_t sequence_number;
    uint8_t reserved[2];
} ADCRecord;
#pragma pack(pop)

typedef struct
{
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    float voltage;
    float temperature;
    uint8_t status_flags;
    uint32_t sequence_number;
} ADCSample;

typedef struct
{
    float mean;
    float minimum;
    float maximum;
    float std_dev;
    float temp_min;
    float temp_mean;
    float temp_max;

    int sample_count;

    int over_voltage_count;
    int under_voltage_count;
    int sensor_fault_count;
    int high_temperature_count;
} ChannelStats;

float adcToVoltage(uint16_t rawValue);
float temperatureToCelsius(int16_t rawTemperature);
void processSample(ADCSample *sample);
void processAllSamples(ADCSample *samples,
                       uint32_t recordCount);

#endif