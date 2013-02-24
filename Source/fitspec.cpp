#include "fitspec.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include <strstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"
#include "dsvdcmp.h"
#include "table.h"
#include "convert.h"

extern const string prmpt;

void fitspec(void) {
//	Spectral fitting

	cls();	

	cout << endl << "Spectral fitting" << endl << endl;

	vector<string> components;
	cout << "Enter component filenames:" << endl;
	//if(!getinpvecnoerr(&components, prmpt.c_str())) return;
	if(!getfiles(&components, prmpt.c_str(), ".spc")) return;
	
	matrix<double> mat;
	spectrum::info_t info;

	for(vector<string>::size_type i=0; i<components.size(); i++) {

		ifstream ifile((components[i]+".spc").c_str());
		//if(!ifileopen(components[i], ".spc", ifile)) return;

		cout << "Reading " << components[i] << endl;

		spectrum spec = readspc(ifile);
		if(i == 0) info = spec.info();
		if(!chkinfo(info, spec)) return;
		mat.push_col(vecftod(spec.get_abs()));
	}

	vector<vector<double> > b;

	vector<string> fitspec;
	cout << endl << "Enter filenames of spectra to fit:" << endl << flush;
	//if(!getinpvecnoerr(&fitspec, prmpt.c_str())) return;
	if(!getfiles(&fitspec, prmpt.c_str(), ".spc")) return;
	
	for(vector<string>::size_type j=0; j<fitspec.size(); j++) {
		ifstream ifile((fitspec[j]+".spc").c_str());
		//if(!ifileopen(fitspec[j], ".spc", ifile)) return;
		cout << "Reading " << fitspec[j] << endl;
		spectrum spec = readspc(ifile);
		if(!chkinfo(info, spec)) return;
		b.push_back(vecftod(spec.get_abs()));
	}

	svd_t svd = dsvdcmp(mat);
	vector<vector<string> > results;

	for(vector<double>::size_type k=0; k<b.size(); k++) {
		vector<double> x = dsvbksb(svd, b[k]);
		vector<string> str;
		for(vector<double>::size_type j=0; j< x.size(); j++) str.push_back(numtostr(x[j], 3, 's'));
		results.push_back(str);
	}

	table_t table;
	table.title = "Table of Component Coefficients";
	table.colhead.push_back(string("Spectrum"));
	table.colhead.insert(table.colhead.end(), components.begin(), components.end());
	table.rowhead = fitspec;
	table.table = results;

	savedisk(table);
	cout << endl << table << endl;
	errmess("");
	return;
}


