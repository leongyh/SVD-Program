#ifndef _DSVDCMP_H
#define _DSVDCMP_H

#include "matrix.h"

struct svd_t {
	matrix<double> u;
	matrix<double> v;
	vector<double> s;
};

typedef struct svd_t svd_t;

svd_t dsvdcmp(const matrix<double>&);
void svdcmp_inplace(svd_t&);
vector<double> dsvbksb(const svd_t&, const vector<double>&);

#endif


