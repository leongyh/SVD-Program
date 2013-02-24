#include "spcasc.h"
#include <iostream.h>
#include "string"
#include <vector.h>
#include <fstream.h>
#include "cls.h"
#include "spectrum.h"
#include "getinp.h"
#include "fileops.h"

extern const string prmpt;

void spcasc(void) {
//	Translate from spc files to ascii files

	cls();
	cout << endl << "Translate files from SPC to ASCII" << endl << endl;
	
	
	vector<string> fnames;
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;

	for(vector<string>::size_type i=0; i<fnames.size(); i++) {
		ifstream ifile((fnames[i]+".spc").c_str());
		//if(!ifileopen(fnames[i], ".spc", ifile)) return;
		ofstream ofile;
		if(!ofileopen(fnames[i], ".asc", ofile)) return;

		cout << "Translating " << fnames[i] << endl;

		spectrum spec = readspc(ifile);

		writeasc(spec, ofile);
	}
	return;
}


