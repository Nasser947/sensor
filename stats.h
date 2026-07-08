#ifndef STATS_H
#define STATS_H

#include "adc.h"

void initializeStatistics(ChannelStats stats[],
                          uint16_t channelCount);

void calculateStatistics(ADCSample *samples,
                         uint32_t recordCount,
                         ChannelStats stats[],
                         uint16_t channelCount);

#endif