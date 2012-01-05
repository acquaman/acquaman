#ifndef DACQLIB_ACQHELPER_H
#define DACQLIB_ACQHELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acquisitionLib.internal.h"

int deleteEventPv( acqEvent_t *ev, int rowNum);
int addIndexEventPv( acqEvent_t *ev, int index, char *name, int norecord, char *fmt, acqPvReady_t ready, int isSpectrum);

#ifdef __cplusplus
}
#endif

#endif
