#include "avespec.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"
#include "strstream.h"

extern const string prmpt;

void avespec(void) {

//	Average spectra
	cls();	
	cout << endl << "Average spectra" << endl << endl;

	vector<string> fnames;
	
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;

	
	spectrum ave;

	for(size_t i=0; i<fnames.size(); i++) {
		ifstream ifile((fnames[i]+".spc").c_str());
		//if(!ifileopen(fnames[i], ".spc", ifile)) return;

		cout << "Reading " << fnames[i] << endl;
		spectrum spec = readspc(ifile);

		if(i == 0) ave = spec;
		else {
			if(!chkinfo(ave, spec)) return;
			ave += spec;
		}
	}
	ave /= fnames.size();

	string outfile;
	if(!getinp(&outfile, "\nEnter a filename for output (no extension): ")) return;
	ofstream ofile;
	if(!ofileopen(outfile, ".spc", ofile)) return;

	writespc(ave, ofile);

	return;
}


