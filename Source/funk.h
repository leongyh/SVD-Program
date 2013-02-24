#ifndef _FUNK_H
#define _FUNK_H

#include <vector.h>
#include "matrix.h"

class funk_t {
private:
	matrix<double> general;
	matrix<double> v;
	matrix<double> vt;
	vector<int> stoich;
	matrix<double> x;
	matrix<double> conc;
	
	
	matrix<double> _gen_solution(const matrix<double>&);
	matrix<double> _getconc(const matrix<double>&, const matrix<double>&, const vector<double>&, const vector<int>&);
	
	void _solve(const vector<double>& parm) {
		if(stoich.empty() || parm.empty()) return;
		conc = _getconc(v, general, parm, stoich);
		x = mul(vt, conc);
		conc = mul(v, x);
		return;
	}
public:
	funk_t(const matrix<double>& vin) : v(vin), vt(transpose(vin)), general(_gen_solution(vin)), 
		stoich(), x(), conc() {return;}
	funk_t(const matrix<double>& vin, const vector<int> s) : v(vin), vt(transpose(vin)), general(_gen_solution(vin)), 
		stoich(s), x(), conc() {return;}
	funk_t(const funk_t& f) : v(f.v), vt(f.vt), general(f.general), stoich(f.stoich), x(f.x), conc(f.conc) {return;}
	~funk_t(void) {return;}
	
	
	funk_t& operator=(const funk_t& f) {
		if(&f == this) return *this;
		general = f.general;
		v = f.v;
		vt = f.vt;
		stoich = f.stoich;
		x = f.x;
		conc = f.conc;
		return *this;
	}
	
	double operator()(const vector<double>&);
	matrix<double> getconc(void) const {return conc;}
	matrix<double> gettransform(void) const {return x;}
	void solve(const vector<double>& p) {_solve(p); return;}
	void putstoich(const vector<int>& s) {stoich = s; return;}
	vector<int> getstoich(void) const {return stoich;}
};


#endif
