#ifndef _SVDSPEC2_H
#define _SVDSPEC2_H

#include "dsvdcmp.h"

struct spcsvdcmp_t {
	svd_t svd;
	float longw;
	float shortw;
};

typedef struct svdspec_t svdspec_t;

spcsvdcmp_t svdspec(void);

#endif



