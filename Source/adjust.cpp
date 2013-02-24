#include "adjust.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"

extern const string prmpt;

void adjust(void) {

//	Adjust for baseline drift.	

	cls();	
	cout << endl << "Adjust spectra" << endl << endl;

	if(!ynquery("NOTE:  This routine will replace the first letter\n"  \
				       "       of the filename with ! (exlamation point)!\n"  \
				       "       Do you wish to continue? (y/n): ")) return;

	cout << endl;
	vector<string> fnames;
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;

	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	
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
		temp[0] = '!';
		
		ofstream ofile;
		if(!ofileopen(temp, ".spc", ofile)) return;
		
		cout << "Adjusting " << fnames[i] << endl;
		spectrum spec = readspc(ifile);
		
		if (!(spec.inrange(longw) && spec.inrange(shortw))) {
			errmess("ERROR: The wavelength adjust limit is out-of-bounds!");
			return;
		}

		long start = spec.which(longw);
		long stop = spec.which(shortw);
		float adj = 0.0;
		for(long j=start; j <= stop; j++) adj += spec[j];
		adj /= stop - start + 1;
		spec.sub(adj);
		writespc(spec, ofile);
	}
	return;
}
