#include "cropper.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"

extern const string prmpt;

void cropper(void) {
//	Crop or pad spectra

	cls();	
	cout << endl << "Crop or pad spectra" << endl << endl;
	if(!ynquery("NOTE:  This routine will replace the first letter\n"  \
				       "       of the filename with - (minus sign)!\n"  \
				       "       Do you wish to continue? (y/n): ")) return;
	cout << endl;

	vector<string> fnames;
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;

	float longw, shortw;

	do{
		cout << endl << "Enter adjustment range:" << endl;
		if(!getinpnoerr(&longw, " Long wavelength: ")) return;
		if(!getinpnoerr(&shortw, " Short wavelength: ")) return;
		cout << endl;
	} while(longw <= shortw);

	for(size_t i=0; i<fnames.size(); i++) {
		ifstream ifile((fnames[i]+".spc").c_str());
		//if(!ifileopen(fnames[i], ".spc", ifile)) return;
		string temp = fnames[i];
		temp[0] = '-';

		ofstream ofile;
		if(!ofileopen(temp, ".spc", ofile)) return;

		cout << "Cropping " << fnames[i] << endl;

		spectrum spec = readspc(ifile);
		spec.range(longw, shortw);
		writespc(spec, ofile);
	}

	return;
}


