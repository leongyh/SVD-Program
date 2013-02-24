#include "getabs.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"
#include <string.h>
#include <stdlib.h>
#include <strstream.h>
#include "table.h"
#include "convert.h"
#include <math.h>

double round(double x, int sig) {
	long t = long(floor(log10(x)));
	double r = x * pow(10, sig-1);
	r /= pow(10,t);
	r += 0.5;
	r = floor(r);
	r *= pow(10, t);
	r /= pow(10, sig-1);
	return r;
}

extern const string prmpt;

void getabs(void) {
//	Retrieve absorbance data
	
	cls();	
	cout << endl << "Get absorbance data" << endl << endl;

	vector<string> fnames;
/*
	do if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	while(!existsfile(fnames, ".spc", " Try again."));
*/
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;

	table_t table;
	vector<float> wave;

	if(!getinpvecnoerr(&wave, "Enter wavelengths: ", false)) return;
	table.colhead = vector<string>(1, string("Spectrum"));
	for(int j=0; j<wave.size(); j++) table.colhead.push_back(numtostr(wave[j], 3, 'f'));
	bool asterisk = false;

	for(vector<string>::size_type i=0; i<fnames.size(); i++) {
		vector<string> v;
		//ifstream ifile;
		//if(!ifileopen(fnames[i], ".spc", ifile)) return;

		ifstream ifile((fnames[i]+".spc").c_str());

		spectrum spec = readspc(ifile);

		for(vector<float>::size_type j=0; j<wave.size(); j++) {
			if(!(spec.inrange(wave[j]))) v.push_back(string("No Data"));
			else {
				v.push_back(numtostr(spec.absorb(wave[j]), 3, 'f'));
				if(round(wave[j], 6) != spec.wavelength(wave[j])) {
					v.back() += " *";
					asterisk = true;
				}
			}
		}
		table.table.push_back(v);
	}
		
	table.title = string("Table of Absorbances");
	if(asterisk) table.title += "\n(Asterisk indicates wavelength was rounded to nearest data point)";
	table.rowhead = fnames;

	savedisk(table);
	cout << endl << table << endl;
	errmess("");
	return;
}



