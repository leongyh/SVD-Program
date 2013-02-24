#include "svdspec.h"
#include <iostream.h>
#include <algo.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"
#include "indexx.h"
#include "convert.h"

extern const string prmpt;

spcsvdcmp_t svdspec(void) {
//	Compute singular value decomposition
	cls();	

	cout << endl << "Compute SVD" << endl << endl;

	vector<string> fnames;
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return spcsvdcmp_t();
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return spcsvdcmp_t();
	svd_t svd;
	matrix<double> &mat = svd.u;
	spectrum::info_t info;

	for(vector<string>::size_type i=0; i<fnames.size(); i++) {
		ifstream ifile((fnames[i]+".spc").c_str());
		//if(!ifileopen(fnames[i], ".spc", ifile)) return spcsvdcmp_t();

		cout << "Reading " << fnames[i] << endl;

		spectrum spec = readspc(ifile);
		if(i == 0) info = spec.info();
		if(!chkinfo(spec, info)) return spcsvdcmp_t();
		mat.push_col(vecftod(spec.get_abs()));
	}
	
	cout << endl << "Computing SVD..." << endl << flush;

	svdcmp_inplace(svd);

	cout << "Sorting results..." << endl << endl << flush;
	
	vector<unsigned long> index = rindexx(svd.s);
	vector<unsigned long> indxindx = indexx(index);
	
	for(unsigned long j=0; j<index.size()-1; j++) {
		swap(svd.s[j], svd.s[index[j]]);
		svd.u.swap_col(j, index[j]);
		svd.v.swap_col(j, index[j]);
		swap(indxindx[j], indxindx[index[j]]);
		swap(index[j], index[indxindx[index[j]]]);
	}
	
	int last = (10 < svd.s.size()) ? 10 : svd.s.size();
	cout << "Largest 10 singular values:" << endl;
	for(unsigned long k=0; k<last; k++) cout << svd.s[k] << endl;
	cout << endl;
	errmess("");
	
	spcsvdcmp_t spcsvd;
	spcsvd.svd = svd;
	spcsvd.longw = info.longw;
	spcsvd.shortw = info.shortw;

//	Write output to disk
	ofstream wfile;
	string temp = "svdw";
	if(!ofileopen(temp, ".asc", wfile)) return spcsvd;

	cout << "Writing output..." << endl;

	const int len = int(floor(log10(fnames.size()-1))) + 1;

	for(unsigned long m=0; m<mat.cols(); m++) {
		spectrum spec(vecdtof(svd.u.get_col(m)), info.longw, info.shortw);
		string temp = "svdu" + numtostr(m, len);
		ofstream ufile;
		if(!ofileopen(temp, ".spc", ufile, true)) return spcsvd;
		writespc(spec, ufile);
		temp = "svdv" + numtostr(m, len);
		ofstream vfile;
		if(!ofileopen(temp, ".asc", vfile, true)) return spcsvd;
		vfile << svd.v.get_col(m);
		wfile << svd.s[m] << endl;
	}

	
	return spcsvd;
}


