#include "aggreg.h"
#include <iostream.h>
#include <math.h>
#include <fstream.h>
#include "matrix.h"
#include "svdspec.h"
#include "dsvdcmp.h"
#include "string"
#include "getinp.h"
#include "funk.h"
#include "amoeba.h"
#include "fileops.h"
#include "spectrum.h"
#include "convert.h"
#include "cls.h"
#include "table.h"

matrix<double> leftinv(const matrix<double>& m) {
	svd_t svd = dsvdcmp(m);
	for(unsigned long i=0; i<svd.s.size(); i++) svd.s[i] = 1.0/svd.s[i];
	
	matrix<double> r = mul(svd.v, diag(svd.s));
	r = mul(r, svd.u.transpose());
	return r;
}
	
struct workdata_t {
	matrix<double> u;
	matrix<double> v;
};


typedef struct workdata_t workdata_t;

workdata_t getdata(const spcsvdcmp_t& svd, int numcmp) {	
	matrix<double> vtemp;
	matrix<double> utemp;
	vector<double> st;
	
	for(int i=0; i<numcmp; i++) {
		vtemp.push_col(svd.svd.v.get_col(i));
		utemp.push_col(svd.svd.u.get_col(i));
		st.push_back(svd.svd.s[i]);
	}
	
	
	utemp = mul(utemp, diag(st));
	
	workdata_t r;
	r.u = utemp;
	r.v = vtemp;
	return r;
}

double minamoeba(funk_t& funk, vector<double>& parm) {
	const int numcmp = funk.getstoich().size();
	const double ftol = 1e-12;
	matrix<double> p(numcmp, parm);
	vector<double> yt;	

	yt.push_back(funk(p[0]));
	for(int i=1; i<numcmp; i++) {
		p[i][i-1] = 0.001;
		yt.push_back(funk(p[i]));
	}
		
	int nfunk = 0;
	bool notdone = amoeba(&p, &yt, ftol, funk, &nfunk);
	while(notdone) {
		// Restart
		const double oldfunk = yt[0];
		p = matrix<double>(numcmp, p[0]);
		yt = vector<double>(1, oldfunk);
		for(int j=1; j<numcmp; j++) {
			p[j][j-1] += 0.001;
			yt.push_back(funk(p[j]));
		}
		nfunk = 0;
		notdone = amoeba(&p, &yt, ftol, funk, &nfunk);
		const double newfunk = yt[0];
		
		const double rtol = 2.0*fabs(oldfunk - newfunk)/(fabs(oldfunk) + fabs(newfunk));
		if(rtol < ftol) notdone = false;
	};
	
	parm = p[0];
	return yt[0];
}

void writeoutput(const vector<string>& fname, const matrix<double>& ut, float longw, float shortw) {
	for(unsigned long j=0; j < ut.cols(); j++) {
		spectrum spec(vecdtof(ut.get_col(j)), longw, shortw);
		ofstream ofile;
		if(!ofileopen(fname[j], ".spc", ofile)) return;
		writespc(spec, ofile);
	}
	return;
}

vector<double> trunc(const vector<double>& v, unsigned long n) {
	vector<double> r(n, 0.0);
	unsigned long last = (v.size() < n) ? v.size() : n;
	for(unsigned long i=0; i<n; i++) r[i] = v[i];
	return r;
}

void writeparms(const vector<double>& parm) {
	cout << endl << "Current fit parameters:" << endl;
	const long saveflags = cout.flags();
	cout.setf(ios::scientific, ios::floatfield);
	for(int i=0; i<parm.size(); i++) cout << setw(15) << parm[i] << endl;
	cout.setf(saveflags);
	return;
}

void aggreg(const spcsvdcmp_t& spcsvd) {
	static bool firstrun = true;
	static vector<string> agg_name;
	if(firstrun) {
		agg_name.push_back("Monomer");
		agg_name.push_back("Dimer");
		agg_name.push_back("Trimer");
		agg_name.push_back("Tetramer");
		agg_name.push_back("Pentamer");
		agg_name.push_back("Hexamer");
		agg_name.push_back("Heptamer");
		agg_name.push_back("Octomer");
		agg_name.push_back("Nonamer");
		agg_name.push_back("Decamer");
		agg_name.push_back("Undecamer");
		agg_name.push_back("Dodecamer");
	}
	firstrun = false;
		
		
	cls();
	cout << "Compute aggregate spectra." << endl << endl;
	
	cout << "Largest 10 singular values:" << endl;
	int last = (10 < spcsvd.svd.s.size()) ? 10 : spcsvd.svd.s.size();
	for(int i=0; i<last; i++) cout << spcsvd.svd.s[i] << endl;
	cout << endl;
	
	table_t table;
	vector<string> components;
	matrix<double> ut;
	vector<double> parm;
	int oldnumcmp = 0;
	do {
		bool frozen = false;
		vector<int> stoich;
		if(!getinpvecnoerr(&stoich, "Enter stoichiometry: ", false, 2)) return;
		const int numcmp = stoich.size();
		const workdata_t data = getdata(spcsvd, numcmp);
		const matrix<double> &v = data.v, &u = data.u;	
		
		if(oldnumcmp != numcmp) parm = vector<double>(numcmp-1, 0.0);
		else {
			writeparms(parm);
			if(ynquery("Manually reset parameters (y/n)? ")) {
				string prompt = string("Enter ") + numtostr(numcmp-1) + " parameters: ";
				if(!getinpvecnoerr(&parm, prompt.c_str(), false, numcmp-1)) return;
				parm = trunc(parm, numcmp-1);
				if(ynquery("\nFreeze the parameters (y/n)? ")) frozen = true;
			}
		}
		oldnumcmp = numcmp;
	
		funk_t funk(v, stoich);
		
		const double chisqu = (frozen) ? funk(parm) : minamoeba(funk, parm);
		const matrix<double> c = funk.getconc();
		const matrix<double> x = funk.gettransform();
		const matrix<double> xinv = leftinv(transpose(x));
		ut = mul(u, xinv);
	
		components = vector<string>();
		int length = int(ceil(log10(ut.cols())));
		for(unsigned long j=0; j < ut.cols(); j++) {
			string fname = string("cmp") + numtostr(j+1);
			components.push_back(fname);
		}

		vector<vector<string> > results;
		for(unsigned long k=0; k< c.rows(); k++) {
			vector<string> str;
			for(vector<double>::size_type j=0; j< c.cols(); j++) str.push_back(numtostr(c[k][j], 4, 'f'));
			results.push_back(str);
		}
		table.title = "Aggregation Computation for Stoichiometry ";
		for(int ii=0; ii < stoich.size(); ii++) table.title += numtostr((long)(stoich[ii])) + ' ';
		table.title += '\n';
		if(frozen) table.title += "Parameters are not optimized.\n";
		string strtemp = (frozen) ? "Unoptimized parameters: " : "Optimized parameters: ";
		table.title += strtemp + numtostr(parm[0], 6, 's', 14);
		for(int kk=1; kk<parm.size(); kk++) 
			table.title += '\n' + string(' ', strtemp.length()) + numtostr(parm[kk], 6, 's', 14);
		table.title += "\n\nNormalized Concentration Courses";
		table.colhead = vector<string>(1, string(" "));
		for(int jj=0; jj < stoich.size(); jj++) {
			if(stoich[jj] <= agg_name.size())
				table.colhead.push_back(agg_name[stoich[jj]-1]);
			else
				table.colhead.push_back(numtostr((long)(stoich[jj])) + "-mer");
		}
		table.rowhead = vector<string>(c.rows(), " ");
		table.table = results;
	
		cout << endl << table << endl;
	} while(!ynquery("\nAccept the results? (y/n)? "));

	cout << endl << "Aggregate spectrum files will be named " << components[0] << ", " << components[1];
	if(components.size() > 2) cout << ", etc.";
	cout << endl;
	
	writeoutput(components, ut, spcsvd.longw, spcsvd.shortw);
	savedisk(table);
	return;
}


	


	
