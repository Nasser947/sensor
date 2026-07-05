#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// Defining all the constants
#define MAGIC_NUMBER       0xADC1BEEF
#define EXPECTED_VERSION   1
#define EXPECTED_CHANNELS  4
#define EXPECTED_SAMPLE_RATE 1000

#pragma pack(push, 1)

//Structure for file header
typedef struct
{
    uint32_t magic;             
    uint16_t version;           
    uint16_t channel_count;     
    uint32_t record_count;      
    uint32_t sample_rate_hz;    
    uint8_t reserved[8];        
} FileHeader;

//Structure for Data Record
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
    ADCRecord record;
    float voltage;
    float temperature_c;

} ADCSample;

#endif