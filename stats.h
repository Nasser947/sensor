#ifndef STATS_H
#define STATS_H

#include "adc.h"

void initializeStatistics(ChannelStats stats[],
                          uint16_t channelCount);

void calculateStatistics(ADCSample *samples,
                         uint32_t recordCount,
                         ChannelStats stats[],
                         uint16_t channelCount);

void detectFaults(ADCSample *samples,
                  uint32_t recordCount,
                  ChannelStats stats[]);

void checkSamplingIntegrity(ADCSample *samples,
                            uint32_t recordCount,
                            int *missingRecords,
                            int *outOfOrderRecords);

void calculateSlidingAverage(ADCSample *samples,
                             uint32_t recordCount,
                             uint16_t windowSize,
                             float *slidingAverages);

void generateHistogram(ADCSample *samples,
                       uint32_t recordCount,
                       int histogram[EXPECTED_CHANNELS][10]);

void analyseTemperature(ADCSample *samples,
                        uint32_t recordCount,
                        ChannelStats stats[],
                        uint16_t channelCount);

void writeResultsReport(const char *filename,
                        const FileHeader *header,
                        ChannelStats stats[],
                        uint16_t channelCount,
                        int missingRecords,
                        int outOfOrderRecords,
                        const float *slidingAverages,
                        uint32_t recordCount,
                        const int histogram[EXPECTED_CHANNELS][10]);

void writeFaultReport(const char *filename,
                      ADCSample *samples,
                      uint32_t recordCount);

#endif