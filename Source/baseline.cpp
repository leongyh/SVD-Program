#include "baseline.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"

extern const string prmpt;

void baseline(void) {
//	Adjust for baseline drift.	
	
	cls();	
	cout << endl << "Baseline subtraction" << endl << endl;
	if(!ynquery("NOTE:  This routine will replace the first letter\n"  \
				       "       of the filename with $ (dollar sign)!\n"  \
				       "       Do you wish to continue? (y/n): ")) return;
	cout << endl;
	
	string basename;
	//if(!getinpnoerr(&basename, "Enter baseline filename (no extension): ")) return;
	if(!getfiles(&basename, "Enter baseline filename (no extension): ", ".spc")) return;

	ifstream bfile;
	
	if(!ifileopen(basename, ".spc", bfile)) return;
	const spectrum basespec = readspc(bfile);
	cout << endl;
	
	vector<string> fnames;
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;


	for(size_t i=0; i<fnames.size(); i++) {
		ifstream ifile((fnames[i]+".spc").c_str());
		//if(!ifileopen(fnames[i], ".spc", ifile)) return;
		string temp = fnames[i];
		temp[0] = '$';
		ofstream ofile;
		if(!ofileopen(temp, ".spc", ofile)) return;

		cout << "Baseline subtracting " << fnames[i] << endl;

		spectrum spec = readspc(ifile);
		if(!chkinfo(spec, basespec)) return;
		
		spec.sub(basespec);
		writespc(spec, ofile);
	}
	return;
}

