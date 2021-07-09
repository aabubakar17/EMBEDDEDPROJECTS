// Header file for TSI
//   Function prototypes

#ifndef TSI_DEFS_H
#define TSI_DEFS_H

#include <MKL25Z4.h>
#include <stdbool.h>

// values used for eol parameter
//#define NOLINE (0)

void TSI_init(void) ;
uint8_t readTSIDistance();

#endif
