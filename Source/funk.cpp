#include "funk.h"
#include <math.h>

unsigned long indxmax(const vector<double>& v) {
	double r = fabs(v[0]);
	unsigned long max = 0;
	for(unsigned long i=1; i < v.size(); i++) if(fabs(v[i]) > fabs(v[max])) max = i;
	return max;
}


matrix<double> funk_t::_gen_solution(const matrix<double>& v) {
	matrix<double> general;
	int numcmp = v.cols();
	
	const unsigned long max = indxmax(v.get_col(0));
	const vector<double> vr = v.get_row(max);
	
	for(int i=1; i<numcmp; i++) {		
		vector<double> homogeneous(numcmp, 0.0);
		homogeneous[0] = vr[i]/vr[0];
		homogeneous[i] = -1;
		general.push_col(homogeneous);
	}
	
	vector<double> particular(numcmp, 0.0);
	particular[0] = 1.0/vr[0];
	general.push_col(particular);
	return general;
}

matrix<double> funk_t::_getconc(const matrix<double>& v, const matrix<double>& general, 
	const vector<double>& parm, const vector<int>& stoich) {
	
	vector<double> t = parm;
	t.push_back(1.0);
	
	const vector<double> current = mul(general, t);
	const vector<double> c1 = mul(v, current);
	
	matrix<double> c;
	c.push_col(c1);
	for(int i=1; i<stoich.size(); i++) {
		vector<double> ct = c1;
		for(unsigned long j=0; j<ct.size(); j++) ct[j] = pow(ct[j], double(stoich[i])/stoich[0]);
		c.push_col(ct);
	}
	return c;
}

double funk_t::operator()(const vector<double>& parm) {
	_solve(parm);
	double sum = 0.0;
		
	for(unsigned long i=0; i<conc.rows(); i++) {
		double t = conc[i][0];
		for(unsigned long j=1; j<conc.cols(); j++) {
			double p = double(stoich[j])/stoich[0];
			double s = pow(t, p) - conc[i][j];
			sum += s*s;
		}
	}
	return sum;
}
