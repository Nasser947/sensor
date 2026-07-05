#ifndef IO_H
#define IO_H

#include "adc.h"

int readBinaryFile(const char *filename,
                   FileHeader *header,
                   ADCSample **samples);

#endif